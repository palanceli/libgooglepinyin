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
#include "../include/loghelper.h"
#include <unistd.h>

// cd libgooglepinyin/build/test/Debug
// ./pinyin_test --gtest_filter=DictBuilderTest.TC01BuildDict

using namespace ime_pinyin;

namespace googlepinyin_test
{
class DictBuilderTest : public testing::Test
{
protected:
  const char *mSzRawDictPath = "data/rawdict_utf16_65105_freq.txt";
  const char *mSzValidUtfPath = "data/valid_utf16.txt";
  const char *mSzDatFilePath = "build/data/dict_pinyin.dat";
  char const *mSzUserDict = "";

protected:
  static void SetUpTestCase()
  {
    FLAGS_logtostderr = 1; // 输出到控制台
    setlocale(LC_ALL, "");

    char buffer[1024] = {0};
    char *cwd = getwd(buffer);
    LOG(INFO) << "current dir is:" << cwd;
  }
  static void TearDownTestCase()
  {
  }
};

TEST_F(DictBuilderTest, TC01BuildDict)
{
  DictTrie *dict_trie = new DictTrie();
  dict_trie->build_dict(mSzRawDictPath, mSzValidUtfPath);
}

TEST_F(DictBuilderTest, TC02BuildDict)
{
  DictTrie *dict_trie = new DictTrie();
  DictBuilder *dict_builder = new DictBuilder();
  dict_trie->free_resource(true);
  dict_builder->build_dict(mSzRawDictPath, mSzValidUtfPath, dict_trie);
}

TEST_F(DictBuilderTest, TC03BuildDict)
{
  DictTrie *dict_trie = new DictTrie();
  DictBuilder *dict_builder = new DictBuilder();
  dict_trie->free_resource(true);

  size_t lemma_num_ = dict_builder->read_raw_dict(mSzRawDictPath, mSzValidUtfPath, 240000);

  size_t spl_item_size;
  size_t spl_num;
  const char *spl_buf;
  spl_buf = dict_builder->spl_table_->arrange(&spl_item_size, &spl_num);

  // 查看dict_builder->lemma_arr_和dict_builder->spl_table_
  printLemmaArray(dict_builder->lemma_arr_, dict_builder->lemma_num_);
  printSpellingTable(dict_builder->spl_table_);
  printSpellingBuff(spl_buf, spl_item_size, spl_num);

  SpellingTrie &spl_trie = SpellingTrie::get_instance();

  spl_trie.construct(spl_buf, spl_item_size, spl_num,
                     dict_builder->spl_table_->get_score_amplifier(),
                     dict_builder->spl_table_->get_average_score());

  printSpellingNodes(spl_trie.level1_sons_, spl_trie.kValidSplCharNum, 0);

  printSpellingTrie(spl_trie);
}

} // namespace googlepinyin_test
