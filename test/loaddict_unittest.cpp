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
class LoadDictTest : public testing::Test
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

TEST_F(LoadDictTest, TC01BuildDict)
{
  bool ret = im_open_decoder(mSzDatFilePath, mSzUserDict);  // 加载
  LOG(INFO)<<"im_open_decoder:"<<ret;
}

} // namespace googlepinyin_test
