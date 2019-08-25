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
#include "../include/dicttrie.h"
#include "../include/matrixsearch.h"
#include <unistd.h>

using namespace ime_pinyin;

namespace googlepinyin_test{
  class PinyinImeTest : public testing::Test{
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
    
    void doSearch(const char* szInput);
  };
  
  TEST_F(PinyinImeTest, TCImOpenDecoder){
    char buffer[1024] = {0};
    char *cwd = getwd(buffer);
    LOG(INFO)<<"current dir is:"<<cwd;
    
    memset(buffer, 0, 1024);
    realpath(mSzDatFilePath, buffer);
    LOG(INFO)<<"mSzDatFilePath is:"<<buffer;
    
    bool ret = im_open_decoder(mSzDatFilePath, mSzUserDict);  // 加载
    ASSERT_EQ(ret, true) << "Failed to load dict!";
  }
  
  void PinyinImeTest::doSearch(const char *szInput){
    
    bool ret = im_open_decoder(mSzDatFilePath, mSzUserDict);  // 加载
    ASSERT_EQ(ret, true) << "Failed to load dict!";
    
    im_set_max_lens(32, 16);
    
    im_reset_search();
    size_t nr = im_search(szInput, strlen(szInput)); // 查询
    size_t size = 0;
    LOG(INFO)<<"输入串："<<im_get_sps_str(&size)<<"\n候选串：";  // 返回拼音串
    char16 str[64] = { 0 };
    for (auto i = 0; i < nr; i++)
    {
      im_get_candidate(i, str, 32);         // 获取查询候选
      if(i > 5)
        break;
      
      std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
      std::string strCand = convert.to_bytes((char16_t*)str);
      LOG(INFO)<< i << "." << strCand << "";
    }
    if(nr>5)
      LOG(INFO)<<"...";
    LOG(INFO)<<" 共"<< nr << "个候选\n";
    
    im_close_decoder();                 // 关闭
  }
  
  TEST_F(PinyinImeTest, TCImSearch01){
    // 只查系统词
    doSearch("dae");
  }
  
  TEST_F(PinyinImeTest, TCBuildDict01){
    
    LOG(INFO)<<"========";
    DictTrie* dict_trie = new DictTrie();
    dict_trie->test_build_dict1("../../../data/rawdict_utf16_65105_freq.txt",
                                      "../../../data/valid_utf16.txt");
  }
}
