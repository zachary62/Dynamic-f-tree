#pragma once
#include "feature.h"
#include "utl.h"
#include <vector>
#include <unordered_set>

class Attribute
{
public:
    Attribute(int num_values, int id,  std::vector<Feature*> fs);
    ~Attribute();
    void printEnd();
    void setEnd(std::vector<int>* end);
    std::vector<double>& getFeatures(int index);
    std::vector<double> result;
    int _id;
    int _num_values;
    std::vector<Feature*> _fs;
    std::vector<int>* _endVec;
};

class AttributeRowIter
{
public:
    AttributeRowIter(const Attribute& a);
    void next(bool& carry, int& value);
    
private:
    const Attribute& _a;
    int cur;
    int next_check;

};