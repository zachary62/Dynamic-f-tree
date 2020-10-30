#include "dimension.h"
#include <vector>

using namespace std;

Dimension::Dimension(std::vector<int> f, int id)
{
    _value = f;
    _id = id;
}
