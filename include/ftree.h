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

    // return a copy of new state
    FtreeState attemptDrillDown(int dim_id, bool cache_allowed);

    // return a copy of state as well as update own state
    void setState(FtreeState fstate);
    

    // feature, attribute and dimenson
    std::vector<Feature*> _f;
    std::vector<Attribute*> _a;
    std::vector<Dimension*> _d;


    std::unordered_map<int,int> att_to_dim;
    // location of att in dim._as
    std::unordered_map<int,int> att_to_index;

    // use dim_id, todrillDown att_id to the cache value
    std::unordered_map<int,std::unordered_map<int,DimCache>> _cache;

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
    void next();
    unsigned int rowSize();
    bool hasNext();
    std::vector<int> update;
    
private:
    const FtreeState& _ts;
    bool _hasNext;
    std::vector<AttributeRowIter*> _iters;
    int value;
    bool carry;
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

// skip the last attribute
class GroupIter
{
public:
    GroupIter(const FtreeState& ts);
    ~GroupIter();
    std::unordered_map<int,int> next();
    unsigned int groupSize();
    bool hasNext();
    int _gsize;
    
private:
    const FtreeState& _ts;
    bool _hasNext;
    std::vector<AttributeRowIter*> _iters;
    int value;
    bool carry;
};


// for XiT Xi
// The matrix it returns should be read only, since it is updated each time
class FtreeCofactorIterator: public GroupIter
{
public:
    FtreeCofactorIterator(const FtreeState& ts):GroupIter(ts),_ts(ts),_init(false){};
    Matrix* nextCofactor();
    const FtreeState& _ts; 
    Matrix* _cof;
    bool _init;
    int num_feature;
    std::vector<double> _cur_feature;
    std::unordered_map<int,int> fid_to_index;
};

class FtreeRightMultiplicationIterator: public GroupIter
{
public:
    FtreeRightMultiplicationIterator(const FtreeState& ts):GroupIter(ts),_ts(ts),_init(false){};
    Matrix* RightMultiplyNext(Matrix* right);
    const FtreeState& _ts;
    bool _init;
    double* _res;
    int num_feature;
    std::vector<double> _cur_feature;
    std::unordered_map<int,int> fid_to_index;
};


class FtreeLeftMultiplicationIterator: public GroupIter
{
public:
    FtreeLeftMultiplicationIterator(const FtreeState& ts):GroupIter(ts),_ts(ts),_init(false){};
    Matrix* LeftMultiplyNext(Matrix* left);
    const FtreeState& _ts;
    bool _init;
    double* _res;
    int num_feature;
    std::vector<double> _cur_feature;
    std::unordered_map<int,int> fid_to_index;
};
