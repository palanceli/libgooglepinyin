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
// ./build/test/pinyin_test --gtest_filter=LoadDictTest.TC01

using namespace ime_pinyin;

namespace googlepinyin_test
{
class ConvertorTest : public testing::Test
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

TEST_F(ConvertorTest, TC01Convert)
{
  bool ret = im_open_decoder(mSzDatFilePath, mSzUserDict);  // 加载
  assert(ret);
  im_set_max_lens(32, 16);
  char szInput[256] = "dae";

  im_set_max_lens(32, 16);

  im_reset_search();
  size_t nr = im_search(szInput, strlen(szInput)); // 查询
  size_t size = 0;
  LOG(INFO) << "输入串：" << im_get_sps_str(&size) << "\n候选串："; // 返回拼音串
  char16 str[64] = {0};
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

    im_close_decoder(); // 关闭
}

} // namespace googlepinyin_test
