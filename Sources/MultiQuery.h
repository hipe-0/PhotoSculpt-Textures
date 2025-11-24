#ifndef MULTIQUERY_H_
#define MULTIQUERY_H_

class MultiQuery
{
public:  
  MultiQuery();

  static const int TEXT_LEN     = 20 ;
  static const int NB_SIZES_MAX = 10 ;
  static const int NB_MAP_TYPES = 9 ;

  int  nbSizes;

  bool isStl, isObj, isPly, isSculpt, isSaveModel, isSaveMap, isSize2n;

  char        listSize     [NB_SIZES_MAX][TEXT_LEN];
  char        listSize2n   [NB_SIZES_MAX][TEXT_LEN];
  static char listMapType  [NB_MAP_TYPES][TEXT_LEN];

  bool isListSizeModel [NB_SIZES_MAX];
  bool isListSizeMap   [NB_SIZES_MAX];
  bool isListMapType   [NB_MAP_TYPES];
};

#endif