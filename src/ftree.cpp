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
        unordered_set<int> first_e;
        first_e.insert(first->_num_values);
        first->setEnd(first_e);

        for(unsigned int i = 1; i < a_vec.size(); i++){
            Attribute* a = a_vec[i];
            unordered_set<int> e = buildRelationFromFile(dir + "r" + to_string(a->_id));
            
            if(e.find(a->_num_values) == e.end()) {
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

unordered_set<int> Ftree::buildRelationFromFile(string dir){

    ifstream input(dir);
    string line;
    stringstream ssLine;

    if (!input)
	{
		cout<< dir << "schema" <<" does not exist. \n";
		exit(1);
	}

    unordered_set<int> s;
    int value;

    while (getline(input, line))
	{   
        ssLine << line;
        ssLine >> value;

        s.insert(value);

        ssLine.clear();
    }
    return s;
    
}

RowIter::RowIter(const Ftree& t):
    _t(t)
{
    for(unsigned int i = 0; i < _t._a.size(); i++){
        _iters.push_back(new AttributeRowIter( *_t._a[i]));
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
