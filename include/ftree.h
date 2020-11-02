#pragma once
#include "feature.h"
#include "attribute.h"
#include "dimension.h"
#include "matrix.h"
#include "utl.h"
#include "state.h"
#include <string>
#include <vector>
#include <unordered_map>


class Ftree
{
public:
    Ftree(std::string dir);
    ~Ftree();

    void initalize(FtreeState init);
    void printState();

    // return a copy of new state
    FtreeState attemptDrillDown(int dim_id);

    // return a copy of new state
    std::vector<FtreeState> attemptAllDrillDown();

    // return a copy of state as well as update own state
    FtreeState DrillDown(int dim_id);
    

    // feature, attribute and dimenson
    std::vector<Feature*> _f;
    std::vector<Attribute*> _a;
    std::vector<Dimension*> _d;

    // dim_id -> next attribute_id to drill down
    std::unordered_map<int,int> _toDrillDown;

    // use key to the cache value
    // for example, for dimension 2 attr 4
    // string would be "2_4"
    std::unordered_map<std::string,DimCache> _cache;

    int _num_f;
    int _num_a;
    int _num_d;



    FtreeState _state;


private:
    void buildFromFile(std::string dir);

    std::vector<double>* buildFeatureFromFile(std::string dir);
    std::vector<int>* buildRelationFromFile(std::string dir);
    
    

};

class RowIter
{
public:
    RowIter(const FtreeState& ts);
    ~RowIter();
    std::unordered_map<int,int> next();
    unsigned int rowSize();
    bool hasNext();
    
private:
    const FtreeState& _ts;
    bool _hasNext;
    std::vector<AttributeRowIter*> _iters;
};

class FtreeToAttrMatrix: public RowIter
{
public:
    FtreeToAttrMatrix(const FtreeState& ts):RowIter(ts){

    };
    Matrix* toMatrix();

};

class FtreeToFeatureMatrix: public RowIter
{
public:
    FtreeToFeatureMatrix(const FtreeState& ts):RowIter(ts),_ts(ts){

    };
    Matrix* toMatrix();
    const FtreeState& _ts;

};

class FtreeRightMultiplication: public RowIter
{
public:
    FtreeRightMultiplication(const FtreeState& ts):RowIter(ts),_ts(ts){

    };
    Matrix* RightMultiply(Matrix* right);
    const FtreeState& _ts;

};

class FtreeCofactor
{
public:
    FtreeCofactor(const FtreeState& ts):_ts(ts){};
    Matrix* Cofactor();
    const FtreeState& _ts;  
};

class FtreeLeftMultiplication
{
public:
    FtreeLeftMultiplication(const FtreeState& ts):_ts(ts){};
    Matrix* LeftMultiply(Matrix* left);
    const FtreeState& _ts;  
};