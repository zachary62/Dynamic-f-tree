#include "attribute.h"
#include "feature.h"
#include <vector>
#include <iostream>

using namespace std;

Attribute::Attribute(int num_values, int id,  std::vector<Feature*> fs)
{
    _num_values = num_values;
    _id = id;
    _fs = fs;
}

void Attribute::setEnd(std::vector<int> end)
{
    _endVec = end;
    _endSet = unordered_set<int>(end.begin(),end.end());
    
}

void Attribute::printEnd(){
    cout<< "attribute " << _id << " set contains: ";
    for(int i : _endVec)
    {
        cout<< i <<" ";
    }

    cout<<"\n";
}

vector<double> Attribute::getFeatures(int index){
    vector<double> result;
    for(Feature* f: _fs){
        result.push_back(f->_value[index]);
    }
    return result;
}


AttributeRowIter::AttributeRowIter(const Attribute& a):
    _a(a)
{
    cur = 1;  
}

IterReply AttributeRowIter::next()
{   
    IterReply r;
    r.carry = false;

    if(_a._endSet.find(cur) != _a._endSet.end()) {
        r.carry = true;
    }

    cur ++;
    
    if(cur > _a._num_values){
        cur = 1;
    }

    r.value = cur;
    return r;

}