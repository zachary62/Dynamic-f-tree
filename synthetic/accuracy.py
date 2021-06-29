#  use python > 3.7
import statsmodels.api as sm
import statsmodels.formula.api as smf
import pandas as pd
import numpy as np
import random
import copy
import math 
from sklearn import linear_model

def linear_repair(data, attribute, aux_attribute):
    # train model to get repair
    train = np.array(data[aux_attribute]).reshape([-1,1])
    reg = linear_model.LinearRegression()
    reg.fit(train, data[attribute])
    return reg.predict(train)

def mixed_repair(data, attribute, aux_attribute, group):
    md = smf.mixedlm(attribute + " ~ " + aux_attribute, data, groups=data[group])
    mdf = md.fit()
    return mdf.fittedvalues

# no auxiliary data
def mixed_none_repair(data, attribute, group):
    md = smf.mixedlm(attribute + " ~ 1", data, groups=data[group])
    mdf = md.fit()
    return mdf.fittedvalues

def acc(answers, results):
    dcg = 0
    idcg = 0
    for i in range(len(results)):
        if results[i] in answers:
            dcg += 1
        idcg +=1
    return dcg/idcg

def group_id(i):
    return i%10;

def missing(data, answer):
    data[answer] = data[answer][0:int(len(data[answer])/2)]
    
def duplicate(data, answer):
    data[answer] = np.append(data[answer], data[answer][0: int(len(data[answer])/2)])
    
def driftup(data, answer):
    data[answer] = data[answer] + 5
    
def driftdown(data, answer):
    data[answer] = data[answer] - 5

def run_exp(itr, complained_agg, low, corrupt_funcs, f):

    for correlation in range(100,59,-10):

        success_itr = itr

        reptile = 0    
        linear = 0
        noneaux = 0
        raw = 0
        scorp = 0
        sup = 0

        for j in range(itr):
            try:
                # generate the expected stats
                std = 20
                expected_stats = pd.DataFrame(columns=['mean','std','count','group'])
                for i in range(100):
                    expected_stats = expected_stats.append({'mean':100, 'std':20, 
                                                            'count':abs(int(np.random.normal(100, 20, 1)[0])),
                                                            'group':group_id(i)},ignore_index=True)

                # for each region, generate variance
                factor = []
                for i in range(10):
                    factor.append(random.uniform(0.5, 1.5))

                for i in range(100):
                    expected_stats.iloc[i]['mean'] = expected_stats.iloc[i]['mean'] * factor[group_id(i)]

                # generate the data according to the expected stats
                data = [None] * 100
                for i in range(100):
                    data[i] = np.random.normal(expected_stats.iloc[i]['mean'],
                                               expected_stats.iloc[i]['std'],
                                               expected_stats.iloc[i]['count'])

                true_stats = pd.DataFrame(columns=['mean','std','count','group'])

                for i in range(100):
                    true_stats = true_stats.append({'mean': data[i].mean(), 
                                                    'std':  data[i].std(), 
                                                    'sum':  data[i].sum(),
                                                    'count': len(data[i]),
                                                    'group':group_id(i)},ignore_index=True)

                # independet random variable
                Count_Ref = np.random.normal(100, std, 100)
                Mean_Ref = np.random.normal(100, std, 100)
                for i in range(100):
                    Mean_Ref[i] = np.random.normal(100 * factor[group_id(i)], std/10, 1)[0] 

                corr_ratio = (correlation -1)/100
                # correlated random variable
                true_stats["count_aux"] = true_stats["count"]*corr_ratio + (1-corr_ratio**2)**0.5*Count_Ref
                true_stats["mean_aux"] = true_stats["mean"]*corr_ratio + (1-corr_ratio**2)**0.5*Mean_Ref

                # for each region, generate variance for aux
                factor2 = []
                for i in range(10):
                    factor2.append(random.uniform(0.5, 1.5))

                for i in range(100):
                    true_stats.at[i,'mean_aux'] = true_stats.iloc[i]['mean_aux'] * factor2[group_id(i)]
                    true_stats.at[i,'count_aux'] = true_stats.iloc[i]['count_aux'] * factor2[group_id(i)]

                true_stats.group = true_stats.group.astype(int)

                # repair 
                linear_stat = pd.DataFrame()
                linear_stat['count'] = linear_repair(true_stats, "count", "count_aux")
                linear_stat['mean'] = linear_repair(true_stats, "mean", "mean_aux")
                linear_stat['sum'] = linear_stat['count'] *  linear_stat['mean']

                mixed_stat = pd.DataFrame()
                mixed_stat['count'] = mixed_repair(true_stats, "count", "count_aux", "group")
                mixed_stat['mean'] = mixed_repair(true_stats, "mean", "mean_aux", "group")
                mixed_stat['sum'] = mixed_stat['count'] *  mixed_stat['mean']

                mixed_none_stat = pd.DataFrame()
                mixed_none_stat['count'] = mixed_none_repair(true_stats, "count", "group")
                mixed_none_stat['mean'] = mixed_none_repair(true_stats, "mean", "group")
                mixed_none_stat['sum'] = mixed_none_stat['count'] *  mixed_none_stat['mean']

                # choose groups to corrupt
                answers = random.sample(range(0, 100), 3)

                # impute errors

                for answer in answers:
                    for func in corrupt_funcs:
                        func(data, answer)

                # update stats
                corrupt_stats = pd.DataFrame(columns=['mean','std','count','group'])

                for i in range(100):
                    corrupt_stats = corrupt_stats.append({'mean': data[i].mean(), 
                                                        'std':  data[i].std(), 
                                                        'sum':  data[i].sum(),
                                                        'count': len(data[i]),
                                                        'group':group_id(i)},ignore_index=True)

                # repair at raw data
                Raw_Repair = copy.deepcopy(data)
                for i in range(100):
                    for j in range(len(Raw_Repair[i])):
                        if(Raw_Repair[i][j] > mixed_stat.iloc[i]["mean"] + true_stats.iloc[i]["std"]):
                            Raw_Repair[i][j] = mixed_stat.iloc[i]["mean"] + true_stats.iloc[i]["std"]
                        if(Raw_Repair[i][j] < mixed_stat.iloc[i]["mean"] - true_stats.iloc[i]["std"]):
                            Raw_Repair[i][j] = mixed_stat.iloc[i]["mean"] - true_stats.iloc[i]["std"]

                raw_repair_stat = pd.DataFrame()
                raw_repair_stat['count'] = np.array([len(item) for item in Raw_Repair])
                raw_repair_stat['mean'] = np.array([item.mean() for item in Raw_Repair])
                raw_repair_stat['sum'] = np.array([item.sum() for item in Raw_Repair])

                # repair by deletion
                delete_repair_stat = pd.DataFrame()
                delete_repair_stat['count'] = corrupt_stats["count"].sum() - corrupt_stats["count"]
                delete_repair_stat['sum'] = corrupt_stats["sum"].sum() - corrupt_stats["sum"]
                delete_repair_stat['mean'] = delete_repair_stat['sum']/delete_repair_stat['count']
                
#                 print(corrupt_stats["count"])
#                 print(true_stats["count"])
#                 print(mixed_stat["count"])
#                 print(mixed_none_stat["count"])
                
                if low:
                    minmax = 1
                else:
                    minmax = -1

                # reptile
                results = list(np.argsort(minmax * (corrupt_stats[complained_agg] - mixed_stat[complained_agg]))[:3])
#                 print(answers)
#                 print(results)
                reptile += acc(answers, results)

                # linear
                results = list(np.argsort(minmax * (corrupt_stats[complained_agg] - linear_stat[complained_agg]))[:3])
                linear += acc(answers, results)

                # nonaux
                results = list(np.argsort(minmax * (corrupt_stats[complained_agg] - mixed_none_stat[complained_agg]))[:3])
                noneaux += acc(answers, results)
#                 print(results)
                # repair at row data level
                results = list(np.argsort(minmax * (corrupt_stats[complained_agg] - raw_repair_stat[complained_agg]))[:3])
                raw += acc(answers, results)

                # repair by deletion
                results = list(np.argsort(minmax * (corrupt_stats[complained_agg] - delete_repair_stat[complained_agg]))[:3])
                scorp += acc(answers, results)

                # support searches for max count
                results = list(np.argsort(-corrupt_stats["count"])[:3])
                sup += acc(answers, results)
            except:
                success_itr =  success_itr - 1
        f.write(""+ str(reptile/success_itr) +","+ str(linear/success_itr)+","+str(noneaux/success_itr)+","+str(raw/success_itr)+","+str(scorp/success_itr)+","+str(sup/success_itr)+"\n")

f = open("result.txt", "a")
run_exp(1000,"count",True,[missing], f)
run_exp(1000,"mean",True,[driftdown], f)
run_exp(1000,"sum",True,[missing,driftdown], f)
run_exp(1000,"count",False,[duplicate], f)
run_exp(1000,"mean",False,[driftup], f)
run_exp(1000,"sum",False,[duplicate, driftup], f)
f.close()

def run_exp2(itr, complained_agg, low, corrupt_funcs1, corrupt_funcs2, f):

    for correlation in range(100,59,-10):

        success_itr = itr

        reptile = 0    
        outlier = 0
        for j in range(itr):
            try:
                # generate the expected stats
                std = 20
                expected_stats = pd.DataFrame(columns=['mean','std','count','group'])
                for i in range(100):
                    expected_stats = expected_stats.append({'mean':100, 'std':20, 
                                                            'count':abs(int(np.random.normal(100, 20, 1)[0])),
                                                            'group':group_id(i)},ignore_index=True)

                # for each region, generate variance
                factor = []
                for i in range(10):
                    factor.append(random.uniform(0.5, 1.5))

                for i in range(100):
                    expected_stats.iloc[i]['mean'] = expected_stats.iloc[i]['mean'] * factor[group_id(i)]

                # generate the data according to the expected stats
                data = [None] * 100
                for i in range(100):
                    data[i] = np.random.normal(expected_stats.iloc[i]['mean'],
                                               expected_stats.iloc[i]['std'],
                                               expected_stats.iloc[i]['count'])

                true_stats = pd.DataFrame(columns=['mean','std','count','group'])

                for i in range(100):
                    true_stats = true_stats.append({'mean': data[i].mean(), 
                                                    'std':  data[i].std(), 
                                                    'sum':  data[i].sum(),
                                                    'count': len(data[i]),
                                                    'group':group_id(i)},ignore_index=True)

                # independet random variable
                Count_Ref = np.random.normal(100, std, 100)
                Mean_Ref = np.random.normal(100, std, 100)
                for i in range(100):
                    Mean_Ref[i] = np.random.normal(100 * factor[group_id(i)], std/10, 1)[0] 

                corr_ratio = (correlation -1)/100
                # correlated random variable
                true_stats["count_aux"] = true_stats["count"]*corr_ratio + (1-corr_ratio**2)**0.5*Count_Ref
                true_stats["mean_aux"] = true_stats["mean"]*corr_ratio + (1-corr_ratio**2)**0.5*Mean_Ref

                # for each region, generate variance for aux
                factor2 = []
                for i in range(10):
                    factor2.append(random.uniform(0.5, 1.5))

                for i in range(100):
                    true_stats.at[i,'mean_aux'] = true_stats.iloc[i]['mean_aux'] * factor2[group_id(i)]
                    true_stats.at[i,'count_aux'] = true_stats.iloc[i]['count_aux'] * factor2[group_id(i)]

                true_stats.group = true_stats.group.astype(int)

                # repair 
#                 linear_stat = pd.DataFrame()
#                 linear_stat['count'] = linear_repair(true_stats, "count", "count_aux")
#                 linear_stat['mean'] = linear_repair(true_stats, "mean", "mean_aux")
#                 linear_stat['sum'] = linear_stat['count'] *  linear_stat['mean']

                mixed_stat = pd.DataFrame()
                mixed_stat['count'] = mixed_repair(true_stats, "count", "count_aux", "group")
                mixed_stat['mean'] = mixed_repair(true_stats, "mean", "mean_aux", "group")
                mixed_stat['sum'] = mixed_stat['count'] *  mixed_stat['mean']

                mixed_none_stat = pd.DataFrame()
                mixed_none_stat['count'] = mixed_none_repair(true_stats, "count", "group")
                mixed_none_stat['mean'] = mixed_none_repair(true_stats, "mean", "group")
                mixed_none_stat['sum'] = mixed_none_stat['count'] *  mixed_none_stat['mean']

                # choose groups to corrupt
                answers = random.sample(range(0, 100), 3)

                # impute errors

                for answer in answers[0:2]:
                    for func in corrupt_funcs1:
                        func(data, answer)
                
                for answer in answers[2:3]:
                    for func in corrupt_funcs2:
                        func(data, answer)

                # update stats
                corrupt_stats = pd.DataFrame(columns=['mean','std','count','group'])

                for i in range(100):
                    corrupt_stats = corrupt_stats.append({'mean': data[i].mean(), 
                                                        'std':  data[i].std(), 
                                                        'sum':  data[i].sum(),
                                                        'count': len(data[i]),
                                                        'group':group_id(i)},ignore_index=True)


                if low:
                    minmax = 1
                else:
                    minmax = -1

                # reptile
                results = list(np.argsort(minmax * (corrupt_stats[complained_agg] - mixed_stat[complained_agg]))[:2])
#                 print(results)
                reptile += acc(answers[0:2], results)

                # ourlier
                results = list(np.argsort( np.absolute(corrupt_stats[complained_agg] - mixed_stat[complained_agg])*-1)[:2])
#                 print(results)
                outlier += acc(answers[0:2], results)
#                 print(answers)

            except:
                success_itr =  success_itr - 1
        print( reptile/success_itr, outlier/success_itr)
        f.write(""+ str(reptile/success_itr) +","+ str(outlier/success_itr)+"\n")

f = open("result_hierarchy.txt", "a")
run_exp2(1000,"count",True,[missing],[duplicate], f)
run_exp2(1000,"mean",True,[driftdown],[driftup], f)
run_exp2(1000,"sum",True,[missing,driftdown],[duplicate, driftup], f)
f.close()