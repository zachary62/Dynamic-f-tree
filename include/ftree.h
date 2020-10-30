#include "feature.h"
#include "attribute.h"
#include "dimension.h"
#include <string>
#include <vector>

class Fftree
{
public:
    Fftree(std::string dir);
    std::vector<Feature> _f;
    std::vector<Attribute> _a;
    std::vector<Dimension> _d;
};