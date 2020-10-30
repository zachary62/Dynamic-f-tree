#include <vector>

class Attribute
{
public:
    Attribute(std::vector<int> f, int id);
    int _id;
    std::vector<int> _value;
};