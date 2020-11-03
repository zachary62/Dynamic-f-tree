#pragma once
#include "attribute.h"
#include <unordered_map>
#include <vector>

struct Count
{
  int id;
  long long leftCount;
  int value;
};

struct CountAtt
{
  int id;
  long long leftCount;
  bool allOne;
  // memory leak!!
  std::vector<int>* prefix_sum;
};

struct CountCof
{
  int id1;
  int id2;
  long long middleCount;
  bool cartesian;
};

struct FtreeState
{
  std::vector<Attribute*> _attr_order;

  // attribute id to state
  std::unordered_map<int, Count> cs;
  std::unordered_map<int, CountAtt> cas;
  std::unordered_map<int, std::unordered_map<int,CountCof>> ccofs;
  
  // dimension id to state
  std::unordered_map<int, int> countDiv;

  // dim_id -> next attribute_id to drill down
  std::unordered_map<int, int> _toDrillDown;

};

struct DimCache
{ 
  std::unordered_map<int,CountAtt> cas;
};