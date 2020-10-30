#include "attribute.h"
#include <vector>

using namespace std;

Attribute::Attribute(std::vector<int> f, int id)
{
    _value = f;
    _id = id;
}
