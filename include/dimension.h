#include <vector>

class Dimension
{
public:
    Dimension(std::vector<int> f, int id);
    int _id;
    std::vector<int> _value;
};