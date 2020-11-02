#pragma once
#include <vector>

class Feature
{
public:
    Feature(std::vector<double>* f, int id);
    ~Feature();
    void printFeature();
    int _id;
    std::vector<double>* _value;
};