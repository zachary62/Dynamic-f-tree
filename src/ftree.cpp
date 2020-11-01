#include "feature.h"
#include "attribute.h"
#include "dimension.h"
#include "ftree.h"
#include "matrix.h"
#include "utl.h"

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>

static const char COMMENT_CHAR = '#';
static const char PARAMETER_SEPARATOR_CHAR = ' ';

static const bool VERBOSE = true;

using namespace std;

Ftree::Ftree(string dir)
{   
    buildFromFile(dir);
}

Ftree::~Ftree()
{   
    for(unsigned int i = 0; i <_f.size(); i++){
        delete _f[i];
    }

    for(unsigned int i = 0; i <_a.size(); i++){
        delete _a[i];
    }

    for(unsigned int i = 0; i <_d.size(); i++){
        delete _d[i];
    }
}

void Ftree::buildFromFile(string dir){
    ifstream input(dir + "schema");
    string line;
    stringstream ssLine;

    if (!input)
	{
		cout<< dir << "schema" <<" does not exist. \n";
		exit(1);
	}

    while (getline(input, line))
	{
		if (line[0] == COMMENT_CHAR || line == "")
			continue;

		break;
	}

    ssLine << line;

    /* Number of dimensions, attributes, and features. */
	string dString, aString, fString;

    getline(ssLine, dString, PARAMETER_SEPARATOR_CHAR);
	getline(ssLine, aString, PARAMETER_SEPARATOR_CHAR);
    getline(ssLine, fString, PARAMETER_SEPARATOR_CHAR);

    ssLine.clear();

    _num_d = stoi(dString);
	_num_a = stoi(aString);
    _num_f = stoi(fString);

    if(VERBOSE){
        cout << "There are "<< _num_d  << " dimensions, "
                            << _num_a << " attributes, "
                            << _num_f << " features. \n";   
    }

    for(int i = 1; i <= _num_f; i++){

        vector<double> vec = buildFeatureFromFile(dir + "f" + to_string(i));
        Feature* f = new Feature(vec,i);
        _f.push_back(f);

        if(VERBOSE){
            cout << "Feature "<< i  << " has " <<  vec.size() << " values. \n";   
        }

    }

    
    for(int i = 1; i <= _num_a; i++){

        while (getline(input, line))
        {
            if (line[0] == COMMENT_CHAR || line == "")
                continue;

            break;
        }

        ssLine << line;

        string nString;
        int num_values;

        getline(ssLine, nString, PARAMETER_SEPARATOR_CHAR);
        num_values = stoi(nString);

        string fidString;
        int f_id;
        vector<Feature*> f_vec;

        while(getline(ssLine, fidString, PARAMETER_SEPARATOR_CHAR)){
            f_id = stoi(fidString);
            Feature* f = _f[f_id-1];
            if(f->_value.size() != unsigned(num_values)){
                cout<< "Feature " << f_id << " doesn't have equal number of values. \n";
		        exit(1);
            }
            f_vec.push_back(f);
        }

        Attribute* a = new Attribute(num_values, i, f_vec);
        _a.push_back(a);

        if(VERBOSE){
            cout << "Attribute "<<  i << " has " <<num_values << " values, " << a->_fs.size() << " features: \n";   
        }

        ssLine.clear();

    }

    for(int i = 1; i <= _num_d; i++){

        while (getline(input, line))
        {
            if (line[0] == COMMENT_CHAR || line == "")
                continue;

            break;
        }

        ssLine << line;
        
        string aidString;
        int a_id;
        vector<Attribute*> a_vec;

        while(getline(ssLine, aidString, PARAMETER_SEPARATOR_CHAR)){
            a_id = stoi(aidString);
            Attribute* a = _a[a_id-1];
            a_vec.push_back(a);
        }

        if(a_vec.size()<1){
            cout<< "Dimension " << i << " doesn't has attribute.\n";
		    exit(1);
        }

        Attribute* first = a_vec[0];

        vector<int> first_e;
        first_e.push_back(first->_num_values);
        first->setEnd(first_e);

        for(unsigned int i = 1; i < a_vec.size(); i++){
            Attribute* a = a_vec[i];
           vector<int> e = buildRelationFromFile(dir + "r" + to_string(a->_id));
            
            if(e[e.size()-1] !=  a->_num_values) {
                cout<< "Relation " << a->_id << " doesn't contain the last value of the attribute.\n";
		        exit(1);
            } 

            Attribute* prev_a = a_vec[i-1];
            if(e.size() != unsigned(prev_a->_num_values)){
                cout<< "Relation " << a->_id << " doesn't has the same number of values as the previous attribute.\n";
		        exit(1);
            }
            a->setEnd(e);
        }

        Dimension* d = new Dimension(a_vec, i);
        _d.push_back(d);

        if(VERBOSE){
            cout << "Dimension "<<  i << " has " << a_vec.size() << " attributes. \n";   
        }

        ssLine.clear();
    }

    if(VERBOSE){

        for(Feature* f: _f){
            f->printFeature();
        }

        for(Attribute* a: _a){
            a->printEnd();
        }
    }

}

vector<double> Ftree::buildFeatureFromFile(string dir){
    ifstream input(dir);
    string line;
    stringstream ssLine;

    if (!input)
	{
		cout<< dir << "schema" <<" does not exist. \n";
		exit(1);
	}

    vector<double> f;
    double value;

    while (getline(input, line))
	{   
        ssLine << line;
        ssLine >> value;

        f.push_back(value);
        ssLine.clear();
    }
    return f;
    
}

vector<int> Ftree::buildRelationFromFile(string dir){

    ifstream input(dir);
    string line;
    stringstream ssLine;

    if (!input)
	{
		cout<< dir << "schema" <<" does not exist. \n";
		exit(1);
	}

    vector<int> s;
    int value;

    while (getline(input, line))
	{   
        ssLine << line;
        ssLine >> value;

        s.push_back(value);

        ssLine.clear();
    }
    return s;
    
}

void Ftree::initalize(FtreeState init){
    _state = init;

    unordered_map<int,int> att_to_dim;
    // location of att in dim._as
    unordered_map<int,int> att_to_index;

    for(Dimension* d: _d){
        for(unsigned int i = 0; i < d->_as.size(); i++){
            Attribute* a =  d->_as[i];
            att_to_dim[a->_id] = d->_id;
            att_to_index[a->_id] = i;
        }
        _toDrillDown[d->_id] = d->_as[0]->_id;
        
    }

    // reverse traversal
    for(unsigned int i =0; i <  _state._attr_order.size()  ; i++){
   
        Attribute* a =  _state._attr_order[i];
        Dimension* d = _d[att_to_dim[a->_id]-1];
       
        if(_toDrillDown[d->_id] != a->_id){
            cout<< "Attribute order is illegal! Should: "<< _toDrillDown[d->_id]<< " get " << a->_id<<"\n";
            exit(1);
        }

        // if a is the last attribute in d
        if(d->_as[d->_as.size() - 1]->_id == a->_id){
            // further drill down is invalid
            _toDrillDown[d->_id] = -1;
        } else{
            _toDrillDown[d->_id] = d->_as[att_to_index[a->_id] + 1]->_id;
        }
    }    

    
    
    for(Dimension* d: _d){
        // currently no attribute in the order
        if (_toDrillDown[d->_id] == d->_as[0]->_id){
            _state.countDiv[d->_id] = 1;
        } 
        // all attributes in the order
        else if(_toDrillDown[d->_id] == -1){
            _state.countDiv[d->_id] = d->_as[d->_as.size()-1]->_num_values;
        } else{
            
            int a_id = _toDrillDown[d->_id];
            _state.countDiv[d->_id] = d->_as[att_to_index[a_id]-1]->_num_values;
        }
  
        // count
        for(Attribute* a: d->_as){
            if(a->_id ==  _toDrillDown[d->_id]){
                break;
            }
            Count c{};
            c.id = a->_id;
            c.value = _state.countDiv[d->_id];
            _state.cs[a->_id] = c;
        }

        // cout<< "for dim " << d->_id<< " to drill down " << _toDrillDown[d->_id] << "\n";
        // countAtt
        if(_toDrillDown[d->_id] == -1){
            CountAtt last{};
            Attribute* last_a = d->_as[d->_as.size() - 1];
            last.id = last_a->_id;
            last.allOne = true;
            _state.cas[last_a->_id] = last;
            
            for(int i = signed(d->_as.size()) - 2; i >= 0; i--){
                Attribute* prev = d->_as[i + 1];
                Attribute* cur = d->_as[i];
                CountAtt ca{};
                ca.id = cur->_id;
                ca.allOne = false;
                if(_state.cas[prev->_id].allOne){
                    ca.prefix_sum = prev->_endVec;
                }else{
                    vector<int> prefix_sum;
                    vector<int> pre_prefix_sum = _state.cas[prev->_id].prefix_sum;
                    for(int i : prev->_endVec){
                        prefix_sum.push_back(pre_prefix_sum[i - 1]);
                    }
                    ca.prefix_sum = prefix_sum;
                }
                _state.cas[cur->_id] = ca;
            }
        // check if this dimension is even involved in current order
        }else if(_toDrillDown[d->_id] != d->_as[0]->_id){
            CountAtt last{};
            Attribute* last_a = d->_as[att_to_index[_toDrillDown[d->_id]] - 1];
            last.id = last_a->_id;
            last.allOne = true;
            _state.cas[last_a->_id] = last;

            for(int i = signed(att_to_index[_toDrillDown[d->_id]]) - 2; i >= 0; i--){
                Attribute* prev = d->_as[i + 1];
                Attribute* cur = d->_as[i];
                CountAtt ca{};
                ca.id = cur->_id;
                ca.allOne = false;

                if(_state.cas[prev->_id].allOne){
                    ca.prefix_sum = prev->_endVec;
                }else{
                    vector<int> prefix_sum;
                    vector<int> pre_prefix_sum = _state.cas[prev->_id].prefix_sum;
                    for(int i : prev->_endVec){
                        prefix_sum.push_back(pre_prefix_sum[i - 1]);
                    }
                    ca.prefix_sum = prefix_sum;
                }
                _state.cas[cur->_id] = ca;
            }

        }

        
    }

    int leftC = 1;
    vector<int> prefix_product;
    unordered_map<int, int> dim_index;
    int prev_dim = -1;
    for(int i = _state._attr_order.size() - 1; i >= 0; i--){
        Attribute* a = _state._attr_order[i];
        int cur_dim = att_to_dim[a->_id];

        if(prev_dim == -1 || prev_dim != cur_dim){
            dim_index[cur_dim] = prefix_product.size();
            prev_dim = cur_dim;
            prefix_product.push_back(leftC);
            leftC *=  _state.countDiv[cur_dim];
        }
        _state.cas[a->_id].leftCount = prefix_product[dim_index[cur_dim]];
        _state.cs[a->_id].leftCount = prefix_product[dim_index[cur_dim]];
    }

    for(int i = _state._attr_order.size() - 1; i >= 0; i--){
        for(int j = i - 1; j >= 0; j--){
            Attribute* a_i = _state._attr_order[i];
            Attribute* a_j = _state._attr_order[j];
            
            int dim_i = att_to_dim[a_i->_id];
            int dim_j = att_to_dim[a_j->_id];

            if(dim_i != dim_j){
                CountCof ccf{};
                ccf.id1 = a_i->_id;
                ccf.id2 = a_j->_id;
                ccf.leftCount = prefix_product[dim_index[dim_i]];
                ccf.middleCount = prefix_product[dim_index[dim_j]] / prefix_product[dim_index[dim_i]+1];
                ccf.cartesian = true;
                _state.ccofs[i][j] = ccf;
            }else{
                CountCof ccf{};
                ccf.id1 = a_i->_id;
                ccf.id2 = a_j->_id;
                ccf.leftCount = prefix_product[dim_index[dim_i]];
                ccf.middleCount = 1;
                ccf.cartesian = false;
                _state.ccofs[i][j] = ccf;
            }

        }
    }



}

 void Ftree::printState(){

     cout << "Attribute Order:\n";
    for(Attribute* a: _state._attr_order){
        cout<< a->_id << " ";
    }
    cout << "\n";

    unordered_map<int, Count>::iterator it;
    cout << "Count:\n";
    for (it = _state.cs.begin(); it != _state.cs.end(); it++ )
    {
        cout << it->first  
                << " leftCount: "
                << it->second.leftCount  
                << " value: "
                << it->second.value << "\n";
    }

    unordered_map<int, CountAtt>::iterator it2;


    cout << "CountAtt:\n";
    for (it2 = _state.cas.begin(); it2 != _state.cas.end(); it2++ )
    {
        cout << it2->first  
                << " leftCount: "
                << it2->second.leftCount;

        if( it2->second.allOne){
            cout << " allOne";
        }else{
            cout << " value: ";
            for(int i: it2->second.prefix_sum){
                cout << i << " ";
            }

        }
        cout<< "\n";

    }

    cout << "CountCof:\n";
    for(auto const &ent1 : _state.ccofs) {
        // auto const &outer_key = ent1.first;
        auto const &inner_map = ent1.second;
        for(auto const &ent2 : inner_map) {
            // auto const &inner_key   = ent2.first;
            auto const &inner_value = ent2.second;

            cout<< inner_value.id1 << " " << inner_value.id2
                << " leftCount " << inner_value.leftCount
                << " middleCount " << inner_value.middleCount;
            if(inner_value.cartesian){
                cout<<" is cartesian";
            } else{
                cout<<" is not cartesian";
            }
            cout<< "\n";
        }
    }
  

 }

RowIter::RowIter(const FtreeState& ts):
    _ts(ts)
{
    for(unsigned int i = 0; i < _ts._attr_order.size(); i++){
        _iters.push_back(new AttributeRowIter( *_ts._attr_order[i]));
    }
    _hasNext = true;
}

unordered_map<int,int> RowIter::next()
{
    unordered_map<int,int> update;
    for(int i = _iters.size() - 1; i >= 0; i--){
        // note that here need to use reference not copy!!
        AttributeRowIter* iter = _iters[i];
        IterReply r =  iter->next();
        update[i] = r.value;

        if(i == 0 && r.carry){
            _hasNext = false;
        }else if(!r.carry){
            break;
        }
    }
    return update;
}

bool RowIter::hasNext()
{
    return _hasNext;
}

unsigned int RowIter::rowSize()
{
    return _iters.size();
}

RowIter::~RowIter()
{
    for(unsigned int i = 0; i < _iters.size(); i++){
        delete _iters[i];
    }
}


Matrix* FtreeToAttrMatrix::toMatrix()
{   
    vector<vector<double>> m;
    vector<double> r(rowSize(), 1);
    m.push_back(r);

    while(true){
        unordered_map<int,int> map = next();
        
        //note check hasNext here, not in the while!
        if(!hasNext()){
            break;
        }
        unordered_map<int, int>::iterator it;
        for ( it = map.begin(); it != map.end(); it++ )
        {
            r[it->first] = it->second;
        }
        m.push_back(r);
    }

    Matrix* mx = new Matrix(m);
    return mx;
}

Matrix* FtreeToFeatureMatrix::toMatrix()
{
    vector<vector<double>> m;
    vector<double> r;
    vector<int> prefix_sum_f;
    int prefix_sum = 0;

    // build a map between attribute -> feature prefix sum
    for(unsigned int i = 0; i < _ts._attr_order.size(); i++){
        prefix_sum_f.push_back(prefix_sum);
        prefix_sum += _ts._attr_order[i]->_fs.size();

        vector<double> features = _ts._attr_order[i]->getFeatures(0);
        for(double feature: features){
            r.push_back(feature);
        }
    }

    m.push_back(r);

        while(true){
        unordered_map<int,int> map = next();
        
        //note check hasNext here, not in the while!
        if(!hasNext()){
            break;
        }

        unordered_map<int, int>::iterator it;
        for ( it = map.begin(); it != map.end(); it++ )
        {
            vector<double> features = _ts._attr_order[it->first]->getFeatures(it->second - 1);
            for(unsigned int i = 0; i < features.size(); i++){
                r[i + prefix_sum_f[it->first]] = features[i];
            }
        }
        m.push_back(r);
    }

    Matrix* mx = new Matrix(m);
    return mx;

}

Matrix* FtreeRightMultiplication::RightMultiply(Matrix* right)
{
    vector<vector<double>> right_vec = right->_m;
    vector<vector<double>> result;
    
    // unsigned int b = _t._f.size();
    // if(b != right_vec.size()){
    //     cout<< "Invalid multiplication!\n";
	// 	exit(1);
    // }

    // current attribute ids
    vector<double> r;

    vector<double> first_row(right_vec[0].size(),0);
    int k = 0;

    // build a map between attribute id -> feature id prefix sum
    vector<int> prefix_sum_f;
    int prefix_sum = 0;

    for(unsigned int i = 0; i < _ts._attr_order.size(); i++){

        prefix_sum_f.push_back(prefix_sum);
        prefix_sum += _ts._attr_order[i]->_fs.size();

        vector<double> features = _ts._attr_order[i]->getFeatures(0);
        for(double feature: features){
            for(unsigned int j = 0; j < right_vec[0].size(); j++){
                first_row[j] += feature * right_vec[k][j];
            }
            k++;
            r.push_back(feature);
        }
    }

    result.push_back(first_row);



    // for rows after
    while(true){
        unordered_map<int,int> map = next();
        
        //note check hasNext here, not in the while!
        if(!hasNext()){
            break;
        }

        vector<double> result_row = result[result.size() - 1];

        vector<double> before(right_vec[0].size(),0);
        vector<double> after(right_vec[0].size(),0);

        // first is position of change. second is the value changed to.
        unordered_map<int, int>::iterator it;
        for ( it = map.begin(); it != map.end(); it++ )
        {
            vector<double> features = _ts._attr_order[it->first]->getFeatures(it->second - 1);

            for(unsigned int i = 0; i < features.size(); i++){

                for(unsigned int j = 0; j < right_vec[0].size(); j++){
                    before[j] += r[i + prefix_sum_f[it->first]] * right_vec[i + prefix_sum_f[it->first]][j];
                }

                r[i + prefix_sum_f[it->first]] = features[i];

                for(unsigned int j = 0; j < right_vec[0].size(); j++){
                    after[j] += r[i + prefix_sum_f[it->first]] * right_vec[i + prefix_sum_f[it->first]][j];
                }
            }
        }

        // propogate change
        for(unsigned int j = 0; j < right_vec[0].size(); j++){
            result_row[j] -= before[j];
            result_row[j] += after[j];
        }

        result.push_back(result_row);
    }

    Matrix* mx = new Matrix(result);
    return mx;

}