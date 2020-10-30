#include "feature.h"
#include <vector>

using namespace std;

Feature::Feature(std::vector<double> f, int id)
{
    _value = f;
    _id = id;
}
