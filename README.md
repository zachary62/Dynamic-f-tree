# Reptile: Aggregation-level Explanations for Hierarchical Data

Reptile is an explanation system for hierarchical data. Given an anomalous aggregate query result,Reptile recommends the next drill-down attribute, and ranks the drill-down groups based on the extent repairing the group’s statistics to itsexpected values resolves the anomaly. Reptile efficiently trains amulti-level model that leverages the data’s hierarchy to estimatethe expected values, and uses a factorised representation of the feature matrix to remove redundancies due to the data’s hierarchical structure. We further extend model training to support factorised data, and develop a suite of optimizations that leverage the data’s hi-erarchical structure.

## Running Reptile

You need to have Lapack installed. Then, run
```
make
```

For performance test, you need to run the following command:
```
// build factorised feature matrix
./reptile build dataset

// compute factorised feature matrix operations
./reptile matrix dataset

// compute drill down optimization
./reptile drilldown dataset

// multilevel model training
./reptile train dataset

// visualize result
./reptile visualize dataset

```

The Explanation Accuracy is tested with python 3.7.8 because run time is concerned for this experiment. Please check the python program in the synthetic folder.


