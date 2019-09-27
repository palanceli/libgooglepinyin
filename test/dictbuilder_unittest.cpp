/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
#include <unistd.h>

// cd libgooglepinyin/build/test/Debug
// ./pinyin_test --gtest_filter=DictBuilderTest.TC01BuildDict

using namespace ime_pinyin;

namespace googlepinyin_test{
class DictBuilderTest : public testing::Test{
protected:
  const char *mSzRawDictPath = "../../../data/rawdict_utf16_65105_freq.txt";
  const char *mSzValidUtfPath = "../../../data/valid_utf16.txt";
  const char *mSzDatFilePath = "../../data/dict_pinyin.dat";
  char const *mSzUserDict = "";
  
protected:
  static void SetUpTestCase(){
    FLAGS_logtostderr = 1; // 输出到控制台
    setlocale(LC_ALL, "");
  }
  static void TearDownTestCase(){
    
  }
  
};

TEST_F(DictBuilderTest, TC01BuildDict){
  DictTrie* dict_trie = new DictTrie();
  dict_trie->build_dict(mSzRawDictPath, mSzValidUtfPath);
}

TEST_F(DictBuilderTest, TC02BuildDict){
  DictTrie* dict_trie = new DictTrie();
  DictBuilder* dict_builder = new DictBuilder();
  dict_trie->free_resource(true);
  dict_builder->build_dict(mSzRawDictPath, mSzValidUtfPath, dict_trie);
}

void printLemmaArray(LemmaEntry* lemma_arr, size_t num, int limit=10){
  char szTitle[] ="idx_by_py idx_by_hz hz_str_len      freq hanzi_str  \
              pinyin_str                   hanzi_scis_ids      spl_idx_arr";
  printf("%s\n", szTitle);
  
  for(size_t i=0; i<num; i++){
    if(i >= limit)
      break;
    
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
  }
}

TEST_F(DictBuilderTest, TC03BuildDict){
  DictTrie* dict_trie = new DictTrie();
  DictBuilder* dict_builder = new DictBuilder();
  dict_trie->free_resource(true);
  
  size_t lemma_num_ = dict_builder->read_raw_dict(mSzRawDictPath, mSzValidUtfPath, 240000);
  LOG(INFO)<<"lemma_num_="<<lemma_num_;
  
  // 查看dict_builder->lemma_arr_和dict_builder->spl_table_
  printLemmaArray(dict_builder->lemma_arr_, dict_builder->lemma_num_);
}

}
