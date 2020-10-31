#include "dimension.h"
#include "feature.h"
#include "attribute.h"
#include <vector>

using namespace std;

Dimension::Dimension(vector<Attribute*> as, int id)
{
    _as = as;
    _id = id;
}
