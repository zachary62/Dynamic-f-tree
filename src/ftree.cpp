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
#include <chrono>

static const char COMMENT_CHAR = '#';
static const char PARAMETER_SEPARATOR_CHAR = ' ';

static const bool VERBOSE = false;

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

        vector<double>* vec = buildFeatureFromFile(dir + "f" + to_string(i));
        Feature* f = new Feature(vec,i);
        _f.push_back(f);

        if(VERBOSE){
            cout << "Feature "<< i  << " has " <<  vec->size() << " values. \n";   
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
            if(f->_value->size() != unsigned(num_values)){
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

    // for each dimension, read relation file
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

        // read all the attributes in this dimension
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

        // first dimension has end {1}
        vector<int>* first_e = new vector<int>();
        first_e->push_back(first->_num_values);
        first->setEnd(first_e);

        for(unsigned int i = 1; i < a_vec.size(); i++){
            Attribute* a = a_vec[i];
            vector<int>* e = buildRelationFromFile(dir + "r" + to_string(a->_id));
            
            if(e->at(e->size()-1) !=  a->_num_values) {
                cout<< "Relation " << a->_id << " doesn't contain the last value of the attribute.\n";
		        exit(1);
            } 

            Attribute* prev_a = a_vec[i-1];
            if(e->size() != unsigned(prev_a->_num_values)){
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

vector<double>* Ftree::buildFeatureFromFile(string dir){
    ifstream input(dir);
    string line;
    stringstream ssLine;

    if (!input)
	{
		cout<< dir << "schema" <<" does not exist. \n";
		exit(1);
	}

    vector<double>* f = new vector<double>();
    double value;

    while (getline(input, line))
	{   
        ssLine << line;
        ssLine >> value;

        f->push_back(value);
        ssLine.clear();
    }
    return f;
    
}

vector<int>* Ftree::buildRelationFromFile(string dir){

    ifstream input(dir);
    string line;
    stringstream ssLine;

    if (!input)
	{
		cout<< dir << "schema" <<" does not exist. \n";
		exit(1);
	}

    vector<int>* s = new vector<int>;
    int value;

    while (getline(input, line))
	{   
        ssLine << line;
        ssLine >> value;

        s->push_back(value);

        ssLine.clear();
    }
    return s;
    
}

void Ftree::initalize(FtreeState init){
    _state = init;

    for(Dimension* d: _d){
        for(unsigned int i = 0; i < d->_as.size(); i++){
            Attribute* a =  d->_as[i];
            att_to_dim[a->_id] = d->_id;
            att_to_index[a->_id] = i;
        }
        _state._toDrillDown[d->_id] = d->_as[0]->_id;
        
    }

    // reverse traversal
    for(unsigned int i =0; i <  _state._attr_order.size()  ; i++){
   
        Attribute* a =  _state._attr_order[i];
        Dimension* d = _d[att_to_dim[a->_id]-1];
       
        if( _state._toDrillDown[d->_id] != a->_id){
            cout<< "Attribute order is illegal! Should: "<< _state._toDrillDown[d->_id]<< " get " << a->_id<<"\n";
            exit(1);
        }

        // if a is the last attribute in d
        if(d->_as[d->_as.size() - 1]->_id == a->_id){
            // further drill down is invalid
            _state._toDrillDown[d->_id] = -1;
        } else{
            _state._toDrillDown[d->_id] = d->_as[att_to_index[a->_id] + 1]->_id;
        }
    }    


    
    for(Dimension* d: _d){
        // currently no attribute in the order
        if (_state._toDrillDown[d->_id] == d->_as[0]->_id){
            _state.countDiv[d->_id] = 1;
        } 
        // all attributes in the order
        else if(_state._toDrillDown[d->_id] == -1){
            _state.countDiv[d->_id] = d->_as[d->_as.size()-1]->_num_values;
        } else{
            
            int a_id = _state._toDrillDown[d->_id];
            _state.countDiv[d->_id] = d->_as[att_to_index[a_id]-1]->_num_values;
        }
  
        // count
        for(Attribute* a: d->_as){
            if(a->_id ==  _state._toDrillDown[d->_id]){
                break;
            }
            Count c{};
            c.id = a->_id;
            c.value = _state.countDiv[d->_id];
            _state.cs[a->_id] = c;
        }

        
        // countAtt
        if(_state._toDrillDown[d->_id] == -1){
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
                    // memory leak!!!
                    vector<int>* prefix_sum = new vector<int>();
                    vector<int>* pre_prefix_sum = _state.cas[prev->_id].prefix_sum;
                    
                    for(int i : *(prev->_endVec)){
                        prefix_sum->push_back(pre_prefix_sum->at(i - 1));
                    }
                    ca.prefix_sum = prefix_sum;
                }
                _state.cas[cur->_id] = ca;
            }
        // check if this dimension is even involved in current order
        }else if(_state._toDrillDown[d->_id] != d->_as[0]->_id){
            CountAtt last{};
            Attribute* last_a = d->_as[att_to_index[_state._toDrillDown[d->_id]] - 1];
            last.id = last_a->_id;
            last.allOne = true;
            _state.cas[last_a->_id] = last;

            for(int i = signed(att_to_index[_state._toDrillDown[d->_id]]) - 2; i >= 0; i--){
                Attribute* prev = d->_as[i + 1];
                Attribute* cur = d->_as[i];
                CountAtt ca{};
                ca.id = cur->_id;
                ca.allOne = false;

                if(_state.cas[prev->_id].allOne){
                    ca.prefix_sum = prev->_endVec;
                }else{
                    vector<int>* prefix_sum = new vector<int>();
                    vector<int>* pre_prefix_sum = _state.cas[prev->_id].prefix_sum;
       
                    for(int i : *(prev->_endVec)){
                        prefix_sum->push_back(pre_prefix_sum->at(i - 1));
                    }
                    ca.prefix_sum = prefix_sum;
                }
                _state.cas[cur->_id] = ca;
            }

        }

        
    }

    long long leftC = 1;
    vector<long long> prefix_product;
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
                // ccf.leftCount = prefix_product[dim_index[dim_i]];
                ccf.middleCount = prefix_product[dim_index[dim_j]] / prefix_product[dim_index[dim_i]+1];
                ccf.cartesian = true;
                _state.ccofs[a_i->_id][a_j->_id] = ccf;
            }else{
                CountCof ccf{};
                ccf.id1 = a_i->_id;
                ccf.id2 = a_j->_id;
                // ccf.leftCount = prefix_product[dim_index[dim_i]];
                ccf.middleCount = 1;
                ccf.cartesian = false;
                _state.ccofs[a_i->_id][a_j->_id] = ccf;
            }
        }
    }



}

FtreeState Ftree::attemptDrillDown(int dim_id, bool cache_allowed){

    if(_state._toDrillDown[dim_id] == -1){
        cout << "Invalid drill down operation\n";
        exit(1);
    }
    
    unordered_map<int, int> att_to_old_index;

    for(unsigned int i = 0; i < _state._attr_order.size(); i++){
        att_to_old_index[_state._attr_order[i]->_id] = i;
    }

    FtreeState after = _state;

    // update after order
    std::vector<Attribute*> _after_order;
    
    // other attirbutes in the same order as before
    for(Attribute* a: _state._attr_order){
        if(att_to_dim[a->_id] != dim_id){
            _after_order.push_back(a);
        }
    }
 
    // attributes in the new dimension are placed last
    for(Attribute* a: _d[dim_id -1]->_as){
        
        if(a->_id != _state._toDrillDown[dim_id]){
            _after_order.push_back(a);
        }else{
            break;
        }
    }
    
    

    _after_order.push_back(_a[_state._toDrillDown[dim_id]-1]);

    after._attr_order =  _after_order;
    
    // update toDrillDown
    if(_d[dim_id -1]->_as[_d[dim_id -1]->_as.size() - 1]->_id == _state._toDrillDown[dim_id]){
        // further drill down is invalid
        after._toDrillDown[dim_id] = -1;
    } else{
        after._toDrillDown[dim_id] = _d[dim_id -1]->_as[att_to_index[_state._toDrillDown[dim_id]] + 1]->_id;
    }

    // update countDiv
    if(after._toDrillDown[dim_id] == -1){
            after.countDiv[dim_id] = _d[dim_id -1]->_as[_d[dim_id -1]->_as.size()-1]->_num_values;
    } else{     
        int a_id = after._toDrillDown[_d[dim_id -1]->_id];
        after.countDiv[dim_id] = _d[dim_id -1]->_as[att_to_index[a_id]-1]->_num_values;
    }
  



    // COUNT of attributes inside of dimension
    for(Attribute* a: _d[dim_id -1]->_as){
        if(a->_id ==  after._toDrillDown[dim_id]){
            break;
        }
        Count c{};
        c.id = a->_id;
        c.value = after.countDiv[dim_id];
        after.cs[a->_id] = c;
    }

    // countAtt

    // check cache
    bool cached = false;
    if(cache_allowed && (_cache.find(dim_id) != _cache.end())){
        if(_cache[dim_id].find(after._toDrillDown[dim_id]) != _cache[dim_id].end()){
            DimCache cache  = _cache[dim_id][after._toDrillDown[dim_id]];
            if(after._toDrillDown[dim_id] == -1){
                for(int i = signed(_d[dim_id -1]->_as.size()) - 1; i >= 0; i--){
                    after.cas[_d[dim_id -1]->_as[i]->_id] = cache.cas[_d[dim_id -1]->_as[i]->_id];
                }
            }
            else{
                for(int i = signed(att_to_index[after._toDrillDown[_d[dim_id -1]->_id]]) - 1; i >= 0; i--){
                    after.cas[_d[dim_id -1]->_as[i]->_id] = cache.cas[_d[dim_id -1]->_as[i]->_id];
                }
            }
            cached = true;
        }
    }
    if(!cached){
        // compute countAtt
        if(after._toDrillDown[dim_id] == -1){
            CountAtt last{};
            Attribute* last_a = _d[dim_id -1]->_as[_d[dim_id -1]->_as.size() - 1];
            last.id = last_a->_id;
            last.allOne = true;
            after.cas[last_a->_id] = last;
            
            for(int i = signed(_d[dim_id -1]->_as.size()) - 2; i >= 0; i--){
                Attribute* prev = _d[dim_id -1]->_as[i + 1];
                Attribute* cur = _d[dim_id -1]->_as[i];
                CountAtt ca{};
                ca.id = cur->_id;
                ca.allOne = false;
                if(after.cas[prev->_id].allOne){
                    ca.prefix_sum = prev->_endVec;
                }else{
                    // memory leak!!!
                    vector<int>* prefix_sum = new vector<int>();
                    vector<int>* pre_prefix_sum = after.cas[prev->_id].prefix_sum;
                    for(int i : *(prev->_endVec)){
                        prefix_sum->push_back(pre_prefix_sum->at(i - 1));
                    }
                    ca.prefix_sum = prefix_sum;
                }
                after.cas[cur->_id] = ca;
            }
        // check if this dimension is even involved in current order
        }
        else if(after._toDrillDown[_d[dim_id -1]->_id] != _d[dim_id -1]->_as[0]->_id){
            CountAtt last{};
            Attribute* last_a = _d[dim_id -1]->_as[att_to_index[after._toDrillDown[_d[dim_id -1]->_id]] - 1];
            last.id = last_a->_id;
            last.allOne = true;
            after.cas[last_a->_id] = last;

            for(int i = signed(att_to_index[after._toDrillDown[_d[dim_id -1]->_id]]) - 2; i >= 0; i--){
                Attribute* prev = _d[dim_id -1]->_as[i + 1];
                Attribute* cur = _d[dim_id -1]->_as[i];
                CountAtt ca{};
                ca.id = cur->_id;
                ca.allOne = false;

                if(after.cas[prev->_id].allOne){
                    ca.prefix_sum = prev->_endVec;
                }else{
                    vector<int>* prefix_sum = new vector<int>();
                    vector<int>* pre_prefix_sum = after.cas[prev->_id].prefix_sum;
                    for(int i : *(prev->_endVec)){
                        prefix_sum->push_back(pre_prefix_sum->at(i - 1));
                    }
                    ca.prefix_sum = prefix_sum;
                }
                after.cas[cur->_id] = ca;
            }
        }

        DimCache cache;
        if(after._toDrillDown[dim_id] == -1){
            for(int i = signed(_d[dim_id -1]->_as.size()) - 1; i >= 0; i--){
                cache.cas[_d[dim_id -1]->_as[i]->_id] = after.cas[_d[dim_id -1]->_as[i]->_id];
            }
        }
        else{
            for(int i = signed(att_to_index[after._toDrillDown[_d[dim_id -1]->_id]]) - 1; i >= 0; i--){
                cache.cas[_d[dim_id -1]->_as[i]->_id] = after.cas[_d[dim_id -1]->_as[i]->_id];
            }
        }
        _cache[dim_id][after._toDrillDown[dim_id]] = cache;

    } 
    


    long long leftC = 1;
    vector<long long> prefix_product;
    unordered_map<int, int> dim_index;
    int prev_dim = -1;
    for(int i = after._attr_order.size() - 1; i >= 0; i--){
        Attribute* a =after._attr_order[i];
        int cur_dim = att_to_dim[a->_id];

        if(prev_dim == -1 || prev_dim != cur_dim){
            dim_index[cur_dim] = prefix_product.size();
            prev_dim = cur_dim;
            prefix_product.push_back(leftC);
            leftC *= after.countDiv[cur_dim];
        }
        after.cas[a->_id].leftCount = prefix_product[dim_index[cur_dim]];
        after.cs[a->_id].leftCount = prefix_product[dim_index[cur_dim]];
    }

    after.ccofs.clear();

    for(int i = after._attr_order.size() - 1; i >= 0; i--){
        for(int j = i - 1; j >= 0; j--){
            Attribute* a_i = after._attr_order[i];
            Attribute* a_j = after._attr_order[j];
            
            int dim_i = att_to_dim[a_i->_id];
            int dim_j = att_to_dim[a_j->_id];

            if(dim_i != dim_j){
                CountCof ccf{};
                ccf.id1 = a_i->_id;
                ccf.id2 = a_j->_id;
                // ccf.leftCount = prefix_product[dim_index[dim_i]];
                ccf.middleCount = prefix_product[dim_index[dim_j]] / prefix_product[dim_index[dim_i]+1];
                ccf.cartesian = true;
                after.ccofs[a_i->_id][a_j->_id] = ccf;
            }else{
                CountCof ccf{};
                ccf.id1 = a_i->_id;
                ccf.id2 = a_j->_id;
                // ccf.leftCount = prefix_product[dim_index[dim_i]];
                ccf.middleCount = 1;
                ccf.cartesian = false;
                after.ccofs[a_i->_id][a_j->_id] = ccf;
            }
        }
    }

    return after;
}

void Ftree::setState(FtreeState fstate){
    _state = fstate;
}

RowIter::RowIter(const FtreeState& ts):
    _ts(ts)
{
    for(unsigned int i = 0; i < _ts._attr_order.size(); i++){
        _iters.push_back(new AttributeRowIter( *_ts._attr_order[i]));
    }
    _hasNext = true;
}

void RowIter::next()
{
    update.clear();
    for(int i = _iters.size() - 1; i >= 0; i--){
        // note that here need to use reference not copy!!
        AttributeRowIter* iter = _iters[i];
        
        iter->next(carry, value);
        update.push_back(value);

        if(i == 0 && carry){
            _hasNext = false;
        }else if(!carry){
            break;
        }
    }
    // return update;
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
        
        next();
        // unordered_map<int,int> map 

        //note check hasNext here, not in the while!
        if(!hasNext()){
            break;
        }

        for(int i = 0; i < update.size(); i++){
            r[rowSize() - 1 - i] = update[i];
        }

        // unordered_map<int, int>::iterator it;
        // for ( it = map.begin(); it != map.end(); it++ )
        // {
            
        // }
        m.push_back(r);
    }

    double* result = new double[m.size()*m[0].size()];

    for(unsigned int i = 0; i < m.size(); i++){
        for(unsigned int j = 0; j < m[0].size(); j++){
            result[i*m[0].size() + j] = m[i][j];
        
        }
    }

    Matrix* mx = new Matrix(result, m.size(), m[0].size());
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

        const vector<double> & features = _ts._attr_order[i]->getFeatures(0);
        for(double feature: features){
            r.push_back(feature);
        }
    }

    m.push_back(r);

    while(true){
        // unordered_map<int,int> map = 
        next();
        
        //note check hasNext here, not in the while!
        if(!hasNext()){
            break;
        }

        for(int i = 0; i < update.size(); i++){
            // r[rowSize() - 1 - i] = update[i];
            const vector<double> & features = _ts._attr_order[rowSize() - 1 - i]->getFeatures(update[i] - 1);
            for(unsigned int j = 0; j < features.size(); j++){
                r[j + prefix_sum_f[rowSize() - 1 - i]] = features[j];
            }
        }

        // unordered_map<int, int>::iterator it;
        // for ( it = map.begin(); it != map.end(); it++ )
        // {
        //     vector<double> features = _ts._attr_order[it->first]->getFeatures(it->second - 1);
        //     for(unsigned int i = 0; i < features.size(); i++){
        //         r[i + prefix_sum_f[it->first]] = features[i];
        //     }
        // }
        m.push_back(r);
    }

       
    double* result = new double[m.size()*m[0].size()];

    for(unsigned int i = 0; i < m.size(); i++){
        for(unsigned int j = 0; j < m[0].size(); j++){
            result[i*m[0].size() + j] = m[i][j];
        
        }
    }

    Matrix* mx = new Matrix(result, m.size(), m[0].size());

    return mx;

}

Matrix* FtreeRightMultiplication::RightMultiply(Matrix* right)
{
    Count c_first = _ts.cs.at(_ts._attr_order[0]->_id);
    
    int a = c_first.leftCount * c_first.value;

    double* right_vec = right->_m;
    // int b = right->_num_row;
    int c = right->_num_column;

    double* result = new double[a*c];
    
    // unsigned int b = _t._f.size();
    // if(b != right_vec.size()){
    //     cout<< "Invalid multiplication!\n";
	// 	exit(1);
    // }

    // current attribute ids
    vector<double> r;

    int k = 0;

    // build a map between attribute id -> feature id prefix sum
    // (first x features belong to y attribute)
    vector<int> prefix_sum_f;
    int prefix_sum = 0;

    for(unsigned int i = 0; i < _ts._attr_order.size(); i++){

        prefix_sum_f.push_back(prefix_sum);
        prefix_sum += _ts._attr_order[i]->_fs.size();

        vector<double> features = _ts._attr_order[i]->getFeatures(0);
        for(double feature: features){
            for(int j = 0; j < c; j++){
                result[j] += feature * right_vec[k*c + j];
            }
            k++;
            r.push_back(feature);
        }
    }


    k = 1;

    // for rows after
    while(true){

        // unordered_map<int,int> map = 
        next();

        //note check hasNext here, not in the while!
        if(!hasNext()){
            break;
        }

        for(int j = 0; j < c; j++){
            result[k*c + j] = result[k*c - c + j];
        }

        // first is position of change. second is the value changed to.
        // unordered_map<int, int>::iterator it;

        // for ( it = map.begin(); it != map.end(); it++ )
        // {
        //     vector<double> features = _ts._attr_order[it->first]->getFeatures(it->second - 1);

        //     for(unsigned int i = 0; i < features.size(); i++){

        //         for(int j = 0; j < c; j++){
        //             result[k*c + j] -= r[i + prefix_sum_f[it->first]] * right_vec[(i + prefix_sum_f[it->first])*c + j];
        //         }

        //         r[i + prefix_sum_f[it->first]] = features[i];

        //         for(int j = 0; j < c; j++){
        //             result[k*c + j] += r[i + prefix_sum_f[it->first]] * right_vec[(i + prefix_sum_f[it->first])*c + j];
        //         }
        //     }
        // }
        for(int att = 0; att< update.size(); att++){
            // update returns the change of attribute
            // now get the change of features
            const vector<double> & features = _ts._attr_order[rowSize() - 1 - att]->getFeatures(update[att] - 1);
            for(unsigned int i = 0; i < features.size(); i++){
                for(int j = 0; j < c; j++){
                    result[k*c + j] -= r[i + prefix_sum_f[rowSize() - 1 - att]] * right_vec[(i + prefix_sum_f[rowSize() - 1 - att])*c + j];                    
                    // cout<<"after delete: "<< result[k*c + j]<<"\n";
                    result[k*c + j] +=  features[i] * right_vec[(i + prefix_sum_f[rowSize() - 1 - att])*c + j];;
                    // cout<<"after add: "<< result[k*c + j]<<"\n";
                    
                }

                r[i + prefix_sum_f[rowSize() - 1 - att]] = features[i];
                // for(int j = 0; j < c; j++){
                //     result[k*c + j] += r[i + prefix_sum_f[rowSize() - 1 - att]] * right_vec[(i + prefix_sum_f[rowSize() - 1 - att])*c + j];
                // }
            }
        }


        k++;

    }

    Matrix* mx = new Matrix(result,a,c);

    return mx;

}

Matrix* FtreeCofactor::Cofactor(){

    int num_feature = 0;
    unordered_map<int,int> fid_to_index;
    for(Attribute * a: _ts._attr_order){
        for(Feature* f: a->_fs){
            fid_to_index[f->_id] = num_feature;
            num_feature ++;
        }
    }

    double* result = new double[num_feature*num_feature];

    
    Count c_first = _ts.cs.at(_ts._attr_order[0]->_id);
    double total = c_first.leftCount * c_first.value;

    for(unsigned int i = 0; i < _ts._attr_order.size(); i++){
        for(unsigned int j = i; j < _ts._attr_order.size(); j++){
            // same attribute
            if(i == j){
                Attribute* a = _ts._attr_order[i];
                for(unsigned int m = 0; m < a->_fs.size(); m++){
                    for(unsigned int n = m; n < a->_fs.size(); n++){
                        Feature* f1 = a->_fs[m];
                        Feature* f2 = a->_fs[n];
                        
                        CountAtt ca = _ts.cas.at(a->_id);
                        
                        double cell_result = 0;

                        if(ca.allOne){
                            for(unsigned int p = 0; p < f1->_value->size(); p++){
                                cell_result += f1->_value->at(p) * f2->_value->at(p);
                            }
                            
                        }else{
                            for(unsigned int p = 0; p < f1->_value->size(); p++){
                                int prev = (p==0? 0: ca.prefix_sum->at(p-1));
                                cell_result += f1->_value->at(p) * f2->_value->at(p) * (ca.prefix_sum->at(p) - prev);
                            }
                        }

                        cell_result *= ca.leftCount;
                        Count c = _ts.cs.at(a->_id);
                        double rightCount = total/ (c.value *c.leftCount);
                        cell_result *= rightCount;

                        result[num_feature*(fid_to_index.at(f1->_id) ) + fid_to_index.at(f2->_id)] = cell_result;
                        result[num_feature*(fid_to_index.at(f2->_id) ) + fid_to_index.at(f1->_id)] = cell_result;
                    }
                }

            }
            else{
                Attribute* a = _ts._attr_order[i];
                Attribute* b = _ts._attr_order[j];

                CountAtt ca1 = _ts.cas.at(a->_id);
                CountAtt ca2 = _ts.cas.at(b->_id);
                CountCof ccf = _ts.ccofs.at(b->_id).at(a->_id);

                for(Feature* f1: a->_fs){
                    for(Feature* f2: b->_fs){
                        if(ccf.cartesian){
                            

                            double cell_result = 0;

                            double left = 0;

                            if(ca2.allOne){
                                for(unsigned int p = 0; p < f2->_value->size(); p++){
                                    left += f2->_value->at(p);
                                }
                                
                            }else{
                                for(unsigned int p = 0; p < f2->_value->size(); p++){
                                    int prev = (p==0? 0: ca2.prefix_sum->at(p-1));
                                    left += f2->_value->at(p) * (ca2.prefix_sum->at(p) - prev);
                                }
                            }

                            double right = 0;

                            if(ca1.allOne){
                                for(unsigned int p = 0; p < f1->_value->size(); p++){
                                    right += f1->_value->at(p);
                                }
                                
                            }else{
                                for(unsigned int p = 0; p < f1->_value->size(); p++){
                                    int prev = (p==0? 0: ca1.prefix_sum->at(p-1));
                                    right += f1->_value->at(p) * (ca1.prefix_sum->at(p) - prev);
                                }
                            }

                            cell_result = left * right;

                            cell_result *= ca2.leftCount;
                            cell_result *= ccf.middleCount;
                            Count c = _ts.cs.at(a->_id);
                            double rightCount = total/ (c.value *c.leftCount);
                            cell_result *= rightCount;

                            result[fid_to_index.at(f1->_id)*num_feature + fid_to_index.at(f2->_id)] = cell_result;
                            result[fid_to_index.at(f2->_id)*num_feature + fid_to_index.at(f1->_id)] = cell_result;
                        
                        }
                        else{
                            double cell_result = 0;

                            // ca2 is the last level in the hierarchy
                            if(ca2.allOne){
                                int rightp = 0;
                                for(int leftp = 0; leftp < signed(f2->_value->size()); leftp++){
                                    if(leftp + 1 > ca1.prefix_sum->at(rightp)){
                                        rightp++;
                                    }
                                    cell_result += f1->_value->at(rightp) * f2->_value->at(leftp);
                                  
                                }

                            }else{
                                int rightp = 0;
                                for(unsigned int leftp = 0; leftp < f2->_value->size(); leftp++){
                                    if(ca2.prefix_sum->at(leftp) > ca1.prefix_sum->at(rightp)){
                                        rightp++;
                                    }

                                    int prev = (leftp==0? 0: ca2.prefix_sum->at(leftp-1));
                                    cell_result += f1->_value->at(rightp) * f2->_value->at(leftp) * (ca2.prefix_sum->at(leftp) - prev);
                                }

                            }

                            cell_result *= ca2.leftCount;
                            Count c = _ts.cs.at(a->_id);
                            double rightCount = total/ (c.value *c.leftCount);
                            cell_result *= rightCount;

                            result[num_feature*(fid_to_index.at(f1->_id) ) + fid_to_index.at(f2->_id)] = cell_result;
                            result[num_feature*(fid_to_index.at(f2->_id) ) + fid_to_index.at(f1->_id)] = cell_result;

                        }

                    }
                }
            }

        }
    }
    
    Matrix* mx = new Matrix(result,num_feature,num_feature);
    return mx;

}

Matrix* FtreeLeftMultiplication::LeftMultiply(Matrix* left){
  
    double* left_vec = left->_m;
    int a =  left->_num_row;
    int b =  left->_num_column;
    
    Count c_first = _ts.cs.at(_ts._attr_order[0]->_id);
    double total = c_first.leftCount * c_first.value;

    if(total != b){
        cout<<"invalid left multiplication!\n";
        exit(1);
    }

    int num_feature = 0;
    
    unordered_map<int,int> fid_to_index;
    for(Attribute * a: _ts._attr_order){
        for(Feature* f: a->_fs){
            fid_to_index[f->_id] = num_feature;
            num_feature ++;
        }
    }
    
    double* result = new double[a*num_feature];


    //build the prefix_sum
    for(int i = 0; i < a; i++){
        for(int j = 1; j < b; j++){
            left_vec[i*b + j] = left_vec[i*b + j-1] + left_vec[i*b + j];
        }
    }

    for(unsigned int i = 0; i < _ts._attr_order.size(); i++){
        Attribute* att = _ts._attr_order[i];
        CountAtt ca = _ts.cas.at(att->_id);
        Count c = _ts.cs.at(att->_id);
        double total_i = c.leftCount * c.value;

        for(unsigned int m = 0; m < att->_fs.size(); m++){
            Feature * f = att->_fs[m];
            for(int j = 0; j < a; j++){
                double cell_result = 0;
                int start = 0;
                for(int k = 0; k < (int) total/total_i; k++){
                    for(unsigned int p = 0; p < f->_value->size(); p++){
                        int length = 0;
                        if(ca.allOne){
                            length = ca.leftCount;
                        }else{
                            int prev = (p==0? 0: ca.prefix_sum->at(p-1));
                            length = (ca.prefix_sum->at(p) - prev) * ca.leftCount;
                        }

                        double rangeSum = 0;
                        if(start == 0){
                            rangeSum = left_vec[j*b + length - 1];
                        }else{
                            rangeSum = left_vec[j*b + start + length - 1] - left_vec[j*b +start - 1];
                        }

                        cell_result += rangeSum * (f->_value->at(p));

                        start += length;
                    }
                }
                result[j*num_feature + fid_to_index.at(f->_id)] = cell_result;
            }
        }
    }

    // //build the prefix_sum
    // for(int i = 0; i < a; i++){
    //     for(int j = b - 1 ; j > 0; j--){
    //         left_vec[i*b + j] = left_vec[i*b + j] - left_vec[i*b + j-1];
    //     }
    // }

    Matrix* mx = new Matrix(result,a,num_feature);
    return mx;
}


GroupIter::GroupIter(const FtreeState& ts):
    _ts(ts)
{
    for(unsigned int i = 0; i < _ts._attr_order.size() - 1; i++){
        _iters.push_back(new AttributeRowIter( *_ts._attr_order[i]));
    }
    _hasNext = true;
    _gsize = _ts.cs.at(_ts._attr_order[_ts._attr_order.size() - 1]->_id).value;
    mx = new Matrix();
}

void GroupIter::next()
{
    update.clear();
    for(int i = _iters.size() - 1; i >= 0; i--){
        // note that here need to use reference not copy!!
        AttributeRowIter* iter = _iters[i];
        iter->next(carry, value);
        update.push_back(value);

        if(i == 0 && carry){
            _hasNext = false;
        }else if(!carry){
            break;
        }
    }
}

bool GroupIter::hasNext()
{
    return _hasNext;
}

unsigned int GroupIter::groupSize()
{
    return _gsize;
}

unsigned int GroupIter::rowSize()
{
    return _iters.size();
}


GroupIter::~GroupIter()
{
    for(unsigned int i = 0; i < _iters.size(); i++){
        delete _iters[i];
    }
}


Matrix* FtreeCofactorIterator::nextCofactor(){
    if(!_init){
        

        num_feature = 0;
        
        for(Attribute * a: _ts._attr_order){
            for(Feature* f: a->_fs){
                fid_to_index[f->_id] = num_feature;
                num_feature ++;
            }
        }

        // build a map between attribute -> feature prefix sum
        for(unsigned int i = 0; i < _ts._attr_order.size(); i++){
            
            if(i != _ts._attr_order.size() - 1){
                const vector<double> & features = _ts._attr_order[i]->getFeatures(0);
                for(double feature: features){
                     _cur_feature.push_back(feature);
                }
            }
            else
            {
                // assume last attribute is a dimension itself
                for(Feature* f: _ts._attr_order[i]->_fs){
                    double sum = 0;
                    for(double v: *(f->_value)){
                        sum += v;
                    }
                     _cur_feature.push_back(sum/_gsize);
                }
            }
        }

        double* result = new double[num_feature*num_feature];
        
        
        // treat last dimension separately
        for(unsigned int i = 0; i < _ts._attr_order.size() ; i++){
            for(unsigned int j = i; j < _ts._attr_order.size() ; j++){
                // same attribute
                if(i == j){
                    Attribute* a = _ts._attr_order[i];
                    for(unsigned int m = 0; m < a->_fs.size(); m++){
                        for(unsigned int n = m; n < a->_fs.size(); n++){
                            Feature* f1 = a->_fs[m];
                            Feature* f2 = a->_fs[n];
                            double cell_result = _cur_feature[fid_to_index.at(f1->_id)] * _cur_feature[fid_to_index.at(f2->_id)] * _gsize;
                            result[fid_to_index.at(f1->_id)*num_feature + fid_to_index.at(f2->_id)] = cell_result;
                            result[fid_to_index.at(f2->_id)*num_feature + fid_to_index.at(f1->_id)] = cell_result;
                        }
                    }
                }
                else{
                    Attribute* a = _ts._attr_order[i];
                    Attribute* b = _ts._attr_order[j];

                    for(Feature* f1: a->_fs){
                        for(Feature* f2: b->_fs){
                            double cell_result = _cur_feature[fid_to_index.at(f1->_id)] * _cur_feature[fid_to_index.at(f2->_id)] * _gsize;
                            result[fid_to_index.at(f1->_id)*num_feature + fid_to_index.at(f2->_id)] = cell_result;
                            result[fid_to_index.at(f2->_id)*num_feature + fid_to_index.at(f1->_id)] = cell_result;
                        }
                    }

                }
            }
        }    


        // special case for last attribute features

        for(unsigned int m = 0; m < _ts._attr_order[ _ts._attr_order.size() - 1]->_fs.size(); m++){
            for(unsigned int n = m; n < _ts._attr_order[ _ts._attr_order.size() - 1]->_fs.size(); n++){
                Feature* f1 = _ts._attr_order[ _ts._attr_order.size() - 1]->_fs[m];
                Feature* f2 = _ts._attr_order[ _ts._attr_order.size() - 1]->_fs[n];
                double sum = 0;

                for(unsigned int p = 0; p <f1->_value->size(); p++){
                    sum += f1->_value->at(p)*f2->_value->at(p);
                }

                result[fid_to_index.at(f1->_id)*num_feature + fid_to_index.at(f2->_id)] = sum;
                result[fid_to_index.at(f2->_id)*num_feature + fid_to_index.at(f1->_id)] = sum;
            }
        }
    

        _cof = new Matrix(result,num_feature,num_feature);
        _init = true;

    }else{
        next();

        if(!hasNext()){
            return _cof;
        }

        
        for (int att = 0; att< update.size(); att++)
        {
            // vector<double> features = _ts._attr_order[it->first]->getFeatures(it->second - 1);

            for(Feature* f: _ts._attr_order[rowSize() - 1 - att]->_fs){
                
                for(int j = 0; j < num_feature; j++){
                    _cof->_m[fid_to_index.at(f->_id)*num_feature + j] /= _cur_feature[fid_to_index.at(f->_id)];
                    _cof->_m[j*num_feature + fid_to_index.at(f->_id)] /= _cur_feature[fid_to_index.at(f->_id)];
                }


                // cout<< "before: "<<_cur_feature[fid_to_index.at(f->_id)] <<"\n";
                _cur_feature[fid_to_index.at(f->_id)] = f->_value->at(update[att] - 1);
                // cout<< "after: "<<_cur_feature[fid_to_index.at(f->_id)] <<"\n";

                for(int j = 0; j < num_feature; j++){
                    _cof->_m[fid_to_index.at(f->_id)*num_feature + j] *= _cur_feature[fid_to_index.at(f->_id)];
                    _cof->_m[j * num_feature + fid_to_index.at(f->_id)] *= _cur_feature[fid_to_index.at(f->_id)];
                }

            }
        }
    }
    return _cof;
}

Matrix* FtreeLeftMultiplicationIterator::LeftMultiplyNext(Matrix* left){
    if(!_init){

        num_feature = 0;
        
        for(Attribute * a: _ts._attr_order){
            for(Feature* f: a->_fs){
                fid_to_index[f->_id] = num_feature;
                num_feature ++;
            }
        }
        // memory leak
        _res = new double[num_feature];

        if(left->_num_column!= _gsize){
            cout << "wrong size for left mulitplication, want " <<num_feature <<", get "<< left->_num_column<<"\n";
            exit(1);
        }

        double baseSum = 0;
        double* leftvec = left->_m;
        for(int i = 0; i < left->_num_column; i++){
            baseSum += leftvec[i];
        }

        for(unsigned int i = 0; i < _ts._attr_order.size() - 1; i++){
            for(Feature * f : _ts._attr_order[i]->_fs){
                _cur_feature.push_back(f->_value->at(0));
                _res[fid_to_index[f->_id]] = f->_value->at(0) * baseSum;
            }
        }
            
        for(Feature * f : _ts._attr_order[_ts._attr_order.size() - 1]->_fs){
            double last = 0;
            for(int i = 0; i < _gsize; i++){
                last += f->_value->at(i) * leftvec[i];
            }
            _res[fid_to_index[f->_id]] = last;
        }
        
        _init = true;
    }
    else{
        next();

        if(!hasNext()){
            mx ->update(_res,1,num_feature);
            return mx;

        }

        
        for (int att = 0; att< update.size(); att++)
        {
            for(Feature* f: _ts._attr_order[rowSize() - 1 - att]->_fs){
                _cur_feature[fid_to_index.at(f->_id)] = f->_value->at(update[att] - 1);
            }
        }

        double baseSum = 0;
        double* leftvec = left->_m;
        for(int i = 0; i < left->_num_column; i++){
            baseSum += leftvec[i];
        }

        for(unsigned int i = 0; i < _ts._attr_order.size() - 1; i++){
            for(Feature * f : _ts._attr_order[i]->_fs){
                _res[fid_to_index[f->_id]] = _cur_feature[fid_to_index.at(f->_id)] * baseSum;
            }
        }
            
        for(Feature * f : _ts._attr_order[_ts._attr_order.size() - 1]->_fs){
            double last = 0;
            for(int i = 0; i < _gsize; i++){
                last += f->_value->at(i) * leftvec[i];
            }
            _res[fid_to_index[f->_id]] = last;
        }
    }

    mx ->update(_res,1,num_feature);
    return mx;

}



Matrix* FtreeRightMultiplicationIterator::RightMultiplyNext(Matrix* right){
    if(!_init){

        num_feature = 0;
        
        for(Attribute * a: _ts._attr_order){
            for(Feature* f: a->_fs){
                fid_to_index[f->_id] = num_feature;
                num_feature ++;
            }
        }
        // memory leak
        _res = new double[_gsize];

        if(right->_num_row != num_feature){
            cout << "wrong size for right mulitplication, want " <<num_feature <<", get "<< right->_num_row<<"\n";
            exit(1);
        }

        for(unsigned int i = 0; i < _ts._attr_order.size() - 1; i++){
            vector<double> features = _ts._attr_order[i]->getFeatures(0);
            for(double feature: features){
                    _cur_feature.push_back(feature);
            }
        }

        double base = 0;
        double* rightvec = right->_m;
        for(unsigned int i = 0; i < _cur_feature.size(); i++){
            base += _cur_feature[i] * rightvec[i];
        }

        for(int i = 0; i < _gsize; i++){
            double last = 0;
            for(Feature * f : _ts._attr_order[_ts._attr_order.size() - 1]->_fs){
                last += f->_value->at(i) * rightvec[fid_to_index[f->_id]];
            }
            _res[i] = last + base;
        }

        _init = true;
    }
    else{
        next();

        if(!hasNext()){
            mx ->update(_res,_gsize,1);
            // Matrix* mx = new Matrix(_res,_gsize,1);
            return mx;
        }

        
        for (int att = 0; att< update.size(); att++)
        {
            for(Feature* f: _ts._attr_order[rowSize() - 1 - att]->_fs){
                _cur_feature[fid_to_index.at(f->_id)] = f->_value->at(update[att] - 1);
            }
        }

        double base = 0;
        double* rightvec = right->_m;
        for(unsigned int i = 0; i < _cur_feature.size(); i++){
            base += _cur_feature[i] * rightvec[i];
            
        }


        for(int i = 0; i < _gsize; i++){
            double last = 0;
            
            for(Feature * f : _ts._attr_order[_ts._attr_order.size() - 1]->_fs){
                last += f->_value->at(i) * rightvec[fid_to_index[f->_id]];
              

            }

            
            _res[i] = last + base;
        }

    }

    mx ->update(_res,_gsize,1);
    return mx;

}



