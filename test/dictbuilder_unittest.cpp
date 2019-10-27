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

// 因为涉及词库的相对路径，一定要在libgooglepinyin下执行
// cd libgooglepinyin
// ./build/test/pinyin_test --gtest_filter=DictBuilderTest.TC01BuildDict

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
  // 这是tools/pinyinime_dictbuilder.cpp里构建词库的核心操作
  // 就两步：1、构建；2、保存
  // 先不管构建的步骤，倒着看，保存的内容一定是构建的最终结果
  DictTrie *dict_trie = new DictTrie();
  dict_trie->build_dict(mSzRawDictPath, mSzValidUtfPath);
  bool success = dict_trie->save_dict(mSzDatFilePath);

  // 以下是save_dict的核心内容
  // if (!spl_trie.save_spl_trie(fp) || !dict_list_->save_list(fp) ||
  //     !save_dict(fp) || !ngram.save_ngram(fp)) {

  // 以下是save_spl_trie展开的内容
  // fwrite(&spelling_size_, sizeof(size_t), 1, fp)
  // fwrite(&spelling_num_, sizeof(size_t), 1, fp)
  // fwrite(&score_amplifier_, sizeof(float), 1, fp)
  // fwrite(&average_score_, sizeof(unsigned char), 1, fp)
  // fwrite(spelling_buf_, sizeof(char) * spelling_size_, spelling_num_, fp)
}

TEST_F(DictBuilderTest, TC02BuildTrie)
{
  DictTrie *dict_trie = new DictTrie();
  DictBuilder *dict_builder = new DictBuilder();
  dict_trie->free_resource(true);

  size_t lemma_num_ = dict_builder->read_raw_dict(mSzRawDictPath, mSzValidUtfPath, 240000);

  size_t spl_item_size;
  size_t spl_num;
  const char *spl_buf;
  spl_buf = dict_builder->spl_table_->arrange(&spl_item_size, &spl_num);

  SpellingTrie &spl_trie = SpellingTrie::get_instance();

  // 把spl_buf构建成一个trie树，spl_trie.level1_sons_是第一层节点的快速索引
  // spelling_idx是怎么生成的？
  // 声母 + 单韵母这些能作为首字母的字符叫做half，它们的spelling_idx=`char - 'A' + 1`
  // 其余的叫做full，它们的spelling_idx=`kFullSplIdStart + item_start_next`，即`30+在spl_buf中出现的顺序`
  // 同时生成h2f_start_和h2f_num_
  spl_trie.construct(spl_buf, spl_item_size, spl_num,
                     dict_builder->spl_table_->get_score_amplifier(),
                     dict_builder->spl_table_->get_average_score());

  printSpellingTrieSavedData(spl_trie);
}

TEST_F(DictBuilderTest, TC03BuildDictList){
  // 以下是DictList::save_list的展开内容
  // fwrite(&scis_num_, sizeof(size_t), 1, fp)
  // fwrite(start_pos_, sizeof(size_t), kMaxLemmaSize + 1, fp)
  // fwrite(start_id_, sizeof(size_t), kMaxLemmaSize + 1, fp)
  // fwrite(scis_hz_, sizeof(char16), scis_num_, fp)
  // fwrite(scis_splid_, sizeof(SpellingId), scis_num_, fp)
  // fwrite(buf_, sizeof(char16), start_pos_[kMaxLemmaSize], fp)

  DictTrie *dict_trie = new DictTrie();
  DictBuilder *dict_builder = new DictBuilder();
  dict_trie->free_resource(true);

  dict_builder->lemma_num_ = dict_builder->read_raw_dict(mSzRawDictPath, mSzValidUtfPath, 240000);

  size_t spl_item_size;
  size_t spl_num;
  const char *spl_buf;
  spl_buf = dict_builder->spl_table_->arrange(&spl_item_size, &spl_num);

  SpellingTrie &spl_trie = SpellingTrie::get_instance();

  spl_trie.construct(spl_buf, spl_item_size, spl_num,
                     dict_builder->spl_table_->get_score_amplifier(),
                     dict_builder->spl_table_->get_average_score());

  // 由于前面spelling_idx_构建完成，接下来根据lemma_num_中的拼音串生成spl_idx_arr串
  for (size_t i = 0; i < dict_builder->lemma_num_; i++) {
    for (size_t hz_pos = 0; hz_pos < (size_t)dict_builder->lemma_arr_[i].hz_str_len;
         hz_pos++) {
      uint16 spl_idxs[2];
      uint16 spl_start_pos[3];
      bool is_pre = true;
      int spl_idx_num =
        dict_builder->spl_parser_->splstr_to_idxs(dict_builder->lemma_arr_[i].pinyin_str[hz_pos],
                                    strlen(dict_builder->lemma_arr_[i].pinyin_str[hz_pos]),
                                    spl_idxs, spl_start_pos, 2, is_pre);
      assert(1 == spl_idx_num);

      if (spl_trie.is_half_id(spl_idxs[0])) {
        uint16 num = spl_trie.half_to_full(spl_idxs[0], spl_idxs);
        assert(0 != num);
      }
      dict_builder->lemma_arr_[i].spl_idx_arr[hz_pos] = spl_idxs[0];
    }
  }

  // Sort the lemma items according to the hanzi, and give each unique item a
  // id
  dict_builder->sort_lemmas_by_hz();

  size_t scis_num_ = dict_builder->build_scis();

  // Construct the dict list
  dict_trie->dict_list_ = new DictList();
  DictList* dict_list = dict_trie->dict_list_;
  bool dl_success = dict_list->init_list(dict_builder->scis_,
                                         dict_builder->scis_num_,
                                         dict_builder->lemma_arr_,
                                         dict_builder->lemma_num_);
  printDictListSavedData(dict_trie->dict_list_);
}

TEST_F(DictBuilderTest, TC02SaveDict)
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

  // 将文件中的词条加载到lemma_num_，并将(拼音, 词频)加入到spl_table_，
  // 后者是一个 `hash[拼音]词频` 的数据结构，同样的拼音重复加入时，将词频累加
  dict_builder->lemma_num_ = dict_builder->read_raw_dict(mSzRawDictPath, mSzValidUtfPath, 240000);

  size_t spl_item_size;
  size_t spl_num;
  const char *spl_buf;
  // 将hash表中的内容按照字母顺序排列到`char[kMaxPinyinSize+1][] spelling_buf_`的紧凑内存里
  // 并将词频归一化到(0, 255)之间
  spl_buf = dict_builder->spl_table_->arrange(&spl_item_size, &spl_num);

  // 查看dict_builder->lemma_arr_和dict_builder->spl_table_
  printLemmaArray(dict_builder->lemma_arr_, dict_builder->lemma_num_);
  printSpellingTable(dict_builder->spl_table_);
  printSpellingBuff(spl_buf, spl_item_size, spl_num);

  SpellingTrie &spl_trie = SpellingTrie::get_instance();

  // 把spl_buf构建成一个trie树，spl_trie.level1_sons_是第一层节点的快速索引
  // spelling_idx是怎么生成的？
  // 声母 + 单韵母这些能作为首字母的字符叫做half，它们的spelling_idx=`char - 'A' + 1`
  // 其余的叫做full，它们的spelling_idx=`kFullSplIdStart + item_start_next`，即`30+在spl_buf中出现的顺序`
  // 同时生成h2f_start_和h2f_num_
  spl_trie.construct(spl_buf, spl_item_size, spl_num,
                     dict_builder->spl_table_->get_score_amplifier(),
                     dict_builder->spl_table_->get_average_score());

  printSpellingNodes(spl_trie.level1_sons_, spl_trie.kValidSplCharNum, 0);

  printSpellingTrie(spl_trie);
}


TEST_F(DictBuilderTest, TC04BuildDict)
{
  DictTrie *dict_trie = new DictTrie();
  DictBuilder *dict_builder = new DictBuilder();
  dict_trie->free_resource(true);

  dict_builder->lemma_num_ = dict_builder->read_raw_dict(mSzRawDictPath, mSzValidUtfPath, 240000);

  size_t spl_item_size;
  size_t spl_num;
  const char *spl_buf;
  spl_buf = dict_builder->spl_table_->arrange(&spl_item_size, &spl_num);

  SpellingTrie &spl_trie = SpellingTrie::get_instance();

  spl_trie.construct(spl_buf, spl_item_size, spl_num,
                     dict_builder->spl_table_->get_score_amplifier(),
                     dict_builder->spl_table_->get_average_score());

  // 由于前面spelling_idx_构建完成，接下来根据lemma_num_中的拼音串生成spl_idx_arr串
  for (size_t i = 0; i < dict_builder->lemma_num_; i++) {
    for (size_t hz_pos = 0; hz_pos < (size_t)dict_builder->lemma_arr_[i].hz_str_len;
         hz_pos++) {
      uint16 spl_idxs[2];
      uint16 spl_start_pos[3];
      bool is_pre = true;
      int spl_idx_num =
        dict_builder->spl_parser_->splstr_to_idxs(dict_builder->lemma_arr_[i].pinyin_str[hz_pos],
                                    strlen(dict_builder->lemma_arr_[i].pinyin_str[hz_pos]),
                                    spl_idxs, spl_start_pos, 2, is_pre);
      assert(1 == spl_idx_num);

      if (spl_trie.is_half_id(spl_idxs[0])) {
        uint16 num = spl_trie.half_to_full(spl_idxs[0], spl_idxs);
        assert(0 != num);
      }
      dict_builder->lemma_arr_[i].spl_idx_arr[hz_pos] = spl_idxs[0];
    }
  }

  // Sort the lemma items according to the hanzi, and give each unique item a
  // id
  dict_builder->sort_lemmas_by_hz();

  size_t scis_num_ = dict_builder->build_scis();

  printLemmaArray(dict_builder->lemma_arr_, dict_builder->lemma_num_);
}

} // namespace googlepinyin_test
