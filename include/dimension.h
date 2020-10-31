#pragma once
#include "feature.h"
#include "attribute.h"
#include <vector>

class Dimension
{
public:
    Dimension(std::vector<Attribute*> as, int id);
    int _id;
    // attributes in this dimension
    std::vector<Attribute*> _as;
};