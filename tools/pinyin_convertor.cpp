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
#include "glog/logging.h"
#include "glog/raw_logging.h"

using namespace ime_pinyin;

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "");
  char buffer[1024] = {0};
  char *cwd = getwd(buffer);
  LOG(INFO)<<"current dir is:"<<cwd;
  
  char const *szSysDict = "../../data/dict_pinyin.dat";
  char const *szUserDict = "";
  if (argc >= 3) {
    szSysDict = argv[1];
    szUserDict = argv[2];
  }
  
  bool ret = im_open_decoder(szSysDict, szUserDict);  // 加载
  assert(ret);
  im_set_max_lens(32, 16);
  char szLine[256];
  
  while (true) {
    printf("\n >");
#ifdef WIN32
    gets_s(szLine, 256);
#else
    fgets(szLine, 256, stdin);
#endif
    // 剪掉结尾的回车符号
    for(auto i=0; i<strlen(szLine); i++){
      if (szLine[i] == '\n')
        szLine[i] = '\0';
    }
    if (strlen(szLine) == 0)
      break;
    
    im_reset_search();
    size_t nr = im_search(szLine, strlen(szLine)); // 查询
    size_t size = 0;
    printf("输入串：%s\n候选串：", im_get_sps_str(&size));  // 返回拼音串
    char16 str[64] = { 0 };
    for (auto i = 0; i < nr; i++)
    {
      im_get_candidate(i, str, 32);         // 获取查询候选
      if(i > 5)
        break;
#ifdef WIN32
      const wchar_t* szCand = (const wchar_t*)str;
      wprintf(L"%s\n", szCand);
#else
      std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
      std::string strCand = convert.to_bytes((char16_t*)str);
      std::cout << i << "." << strCand << " ";
#endif
    }
    if(nr>5)
      printf("...");
    printf(" 共%lu个候选\n", nr);
  }
  
  im_close_decoder();                 // 关闭
  
  return 0;
}
