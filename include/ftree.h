#pragma once
#include "feature.h"
#include "attribute.h"
#include "dimension.h"
#include "matrix.h"
#include "utl.h"
#include <string>
#include <vector>
#include <unordered_map>


class Ftree
{
public:
    Ftree(std::string dir);
    ~Ftree();
    // feature, attribute and dimenson
    std::vector<Feature*> _f;
    std::vector<Attribute*> _a;
    std::vector<Dimension*> _d;
    int _num_f;
    int _num_a;
    int _num_d;


private:
    void buildFromFile(std::string dir);

    std::vector<double> buildFeatureFromFile(std::string dir);
    std::unordered_set<int> buildRelationFromFile(std::string dir);
    

};

class RowIter
{
public:
    RowIter(const Ftree& t);
    ~RowIter();
    std::unordered_map<int,int> next();
    unsigned int rowSize();
    bool hasNext();
    
private:
    const Ftree& _t;
    bool _hasNext;
    std::vector<AttributeRowIter*> _iters;
};

class FtreeToAttrMatrix: public RowIter
{
public:
    FtreeToAttrMatrix(const Ftree& t):RowIter(t){

    };
    Matrix* toMatrix();

};