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
#include <cassert>
#include <memory.h>

#ifdef WIN32
#include <Windows.h>
#include <tchar.h>
#endif

#include <locale.h>
#include <iostream>
#include <codecvt>
#include "glog/logging.h"
#include "glog/raw_logging.h"
#include <gflags/gflags.h>

#define private public
#define protected public
#include "../include/pinyinime.h"
#include "../include/dicttrie.h"
#include "../include/matrixsearch.h"
#include "../include/dictbuilder.h"
#include "../include/loghelper.h"

using namespace ime_pinyin;

DEFINE_int32(spl_id, -1, "spelling id");
DEFINE_int32(lma_id, -1, "lemma id");

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "");
  google::ParseCommandLineFlags(&argc, &argv, true);

  char buffer[1024] = {0};
  char *cwd = getwd(buffer);
  LOG(INFO)<<"current dir is:"<<cwd;
  
  char const *szSysDict = "build/data/dict_pinyin.dat";
  char const *szUserDict = "";
  
  MatrixSearch *matrix_search = new MatrixSearch();
  bool ret = matrix_search->init(szSysDict, szUserDict);
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;

  if (FLAGS_spl_id >= 0)
  {
    SpellingTrie &spl_trie = SpellingTrie::get_instance();
    const char* pinyin = spl_trie.get_spelling_str(FLAGS_spl_id);
    LOG(INFO) << " pinyin:"<<pinyin;
  }else if (FLAGS_lma_id >= 0){
    char16 str[16] = {0};
    matrix_search->dict_trie_->dict_list_->get_lemma_str(FLAGS_lma_id, str, 16);
    std::string strCand = convert.to_bytes((char16_t *)str);
    LOG(INFO) << "hanzi:" << strCand << "\n";
  }
  
  return 0;
}
