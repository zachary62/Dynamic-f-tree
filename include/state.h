#pragma once
#include "attribute.h"
#include <unordered_map>
#include <vector>

struct Count
{
  int id;
  int leftCount;
  int value;
};

struct CountAtt
{
  int id;
  int leftCount;
  bool allOne;
  std::vector<int> prefix_sum;
};

struct CountCof
{
  int id1;
  int id2;
  int leftCount;
  int middleCount;
  bool cartesian;
};

struct FtreeState
{
  std::vector<Attribute*> _attr_order;

  std::unordered_map<int, Count> cs;
  std::unordered_map<int, CountAtt> cas;
  std::unordered_map<int,std::unordered_map<int,CountCof>> ccofs;
  std::unordered_map<int,int> countDiv;


};

struct DimCache
{ 
  std::vector<CountAtt> cas;
  std::vector<std::vector<CountCof>> ccofs;
};