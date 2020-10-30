#include <vector>
class Feature
{
public:
    Feature(std::vector<double> f, int id);
    int _id;
    std::vector<double> _value;
};