

#include <stdlib.h>
#include <stdio.h>
#include "../include/pinyinime.h"
#include <cassert>
#include <memory.h>

#ifdef WIN32
#include <Windows.h>
#include <tchar.h>
#endif

#include <locale.h>
#include <iostream>
#include <codecvt>

#include <gtest/gtest.h>
#include "glog/logging.h"

#define private public
#define protected public

#include "../include/dicttrie.h"
#include "../include/matrixsearch.h"
#include "../include/dictbuilder.h"
#include "../include/loghelper.h"
#include <unistd.h>

void printLemmaArray(LemmaEntry* lemma_arr, size_t num, int limit){
  char szTitle[] ="idx_by_py idx_by_hz hz_str_len      freq hanzi_str  \
              pinyin_str                   hanzi_scis_ids      spl_idx_arr";
  printf("%s\n", szTitle);
  
  for(size_t i=0; i<num; i++){
    ime_pinyin::LemmaEntry *pLemma = lemma_arr + i;
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::string strHanzi =  convert.to_bytes((char16_t*)pLemma->hanzi_str);
    
    std::string strPy = "";
    for(size_t j=0; j<ime_pinyin::kMaxLemmaSize; j++){
      if(strlen(pLemma->pinyin_str[j]) == 0)
        break;
      strPy += " ";
      strPy += pLemma->pinyin_str[j];
    }
    
    std::string strScis = "";
    for(size_t j=0; j<ime_pinyin::kMaxLemmaSize; j++){
      if(pLemma->hanzi_scis_ids[j] == 0){
        strScis += "0 ";
      }else{
        char sz[8] = {0};
        sprintf(sz, "%5u ", pLemma->hanzi_scis_ids[j]);
        strScis += sz;
      }
    }
    
    std::string strSpl = "";
    for(size_t j=0; j<ime_pinyin::kMaxLemmaSize + 1; j++){
      if(pLemma->spl_idx_arr[j] == 0){
        strSpl += "0 ";
      }else{
        char sz[8] = {0};
        sprintf(sz, "%4u ", pLemma->spl_idx_arr[j]);
        strSpl += sz;
      }
    }
    
    printf("%8zu, %8zu, %9u, %8.0f, %9s, %24s, %32s, %24s\n",
           pLemma->idx_by_py, pLemma->idx_by_hz,
           pLemma->hz_str_len, pLemma->freq,
           strHanzi.c_str(), strPy.c_str(),
           strScis.c_str(), strSpl.c_str());
    
    if(limit>0 && i >= limit)
      break;
  }
  printf("total num=%zu\n", num);
}


void printSpellingTable(SpellingTable* spellingTable, int limit){
  printf("idx    str    freq    total num:%zu\n", spellingTable->spelling_num_);
  int cPrint = 0;
  for(size_t i=0; i<spellingTable->spelling_max_num_; i++){
    RawSpelling *rawspelling =spellingTable->raw_spellings_ + i;
    if(rawspelling->str[0] == '\0'){
      continue;
    }
    printf("%3lu, %7s, %.2f\n", i, rawspelling->str, rawspelling->freq);
    
    if(limit>0 && ++cPrint>limit){
      return;
    }
  }
}
