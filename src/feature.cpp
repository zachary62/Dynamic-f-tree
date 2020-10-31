#include "feature.h"
#include <vector>
#include <iostream>

using namespace std;

Feature::Feature(std::vector<double> f, int id)
{
    _value = f;
    _id = id;
}

void Feature::printFeature(){
    cout<< "feature " << _id << " contains: ";
    for(unsigned int i = 0; i < _value.size(); i++)
    {
        cout<< _value[i] <<" ";
    }

    cout<<"\n";
}

