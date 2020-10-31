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

void Attribute::setEnd(std::unordered_set<int> end)
{
    _end = end;
    printEnd();
    
}

void Attribute::printEnd(){
    cout<< "attribute " << _id << " set contains: ";
    for(auto it = _end.begin(); it != _end.end(); it++)
    {
        cout<< *it <<" ";
    }

    cout<<"\n";
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

    

    if(_a._end.find(cur) != _a._end.end()) {
        r.carry = true;
    }

    cur ++;
    
    if(cur > _a._num_values){
        cur = 1;
    }
    r.value = cur;
    return r;
}