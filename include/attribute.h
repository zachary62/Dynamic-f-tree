#pragma once
#include "feature.h"
#include "utl.h"
#include <vector>
#include <unordered_set>

class Attribute
{
public:
    Attribute(int num_values, int id,  std::vector<Feature*> fs);
    void printEnd();
    void setEnd(std::vector<int> end);
    std::vector<double> getFeatures(int index);
    
    int _id;
    int _num_values;
    std::vector<Feature*> _fs;
    std::vector<int> _endVec;
    std::unordered_set<int> _endSet;
};

class AttributeRowIter
{
public:
    AttributeRowIter(const Attribute& a);
    IterReply next();
    
private:
    const Attribute& _a;
    int cur;

};