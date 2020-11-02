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

Attribute::~Attribute()
{
    delete _endVec;
}

void Attribute::setEnd(std::vector<int>* end)
{
    _endVec = end;
    
}

void Attribute::printEnd(){
    cout<< "attribute " << _id << " ends set contains: ";
    for(int i : *_endVec)
    {
        cout<< i <<" ";
    }

    cout<<"\n";
}

vector<double> Attribute::getFeatures(int index){
    vector<double> result;
    for(Feature* f: _fs){
        result.push_back(f->_value->at(index));
    }
    return result;
}


AttributeRowIter::AttributeRowIter(const Attribute& a):
    _a(a)
{
    cur = 1;  
    next_check = 0;
}

IterReply AttributeRowIter::next()
{   
    IterReply r;
    r.carry = false;

    if(cur >= _a._endVec->at(next_check)) {
        r.carry = true;
        next_check ++;
    }

    cur ++;
    
    if(cur > _a._num_values){
        cur = 1;
        next_check = 0;
    }

    r.value = cur;
    return r;

}