

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

void printLemmaArray(LemmaEntry *lemma_arr, size_t num, int limit)
{
  LOG(INFO) << "lemma_num_=" << num;
  char szTitle[] = "idx_by_py idx_by_hz hz_str_len      freq hanzi_str  \
              pinyin_str                   hanzi_scis_ids      spl_idx_arr";
  printf("%s\n", szTitle);

  for (size_t i = 0; i < num; i++)
  {
    ime_pinyin::LemmaEntry *pLemma = lemma_arr + i;
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::string strHanzi = convert.to_bytes((char16_t *)pLemma->hanzi_str);

    std::string strPy = "";
    for (size_t j = 0; j < ime_pinyin::kMaxLemmaSize; j++)
    {
      if (strlen(pLemma->pinyin_str[j]) == 0)
        break;
      strPy += " ";
      strPy += pLemma->pinyin_str[j];
    }

    std::string strScis = "";
    for (size_t j = 0; j < ime_pinyin::kMaxLemmaSize; j++)
    {
      if (pLemma->hanzi_scis_ids[j] == 0)
      {
        strScis += "0 ";
      }
      else
      {
        char sz[8] = {0};
        sprintf(sz, "%5u ", pLemma->hanzi_scis_ids[j]);
        strScis += sz;
      }
    }

    std::string strSpl = "";
    for (size_t j = 0; j < ime_pinyin::kMaxLemmaSize + 1; j++)
    {
      if (pLemma->spl_idx_arr[j] == 0)
      {
        strSpl += "0 ";
      }
      else
      {
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

    if (limit > 0 && i >= limit)
      break;
  }
  printf("\n");
}

void printSpellingTable(SpellingTable *spellingTable, int limit)
{
  LOG(INFO) << "raw spelling num=" << spellingTable->spelling_num_;
  printf("idx    str    freq\n");
  int cPrint = 0;
  for (size_t i = 0; i < spellingTable->spelling_max_num_; i++)
  {
    RawSpelling *rawspelling = spellingTable->raw_spellings_ + i;
    if (rawspelling->str[0] == '\0')
    {
      continue;
    }
    printf("%3lu, %7s, %.2f\n", i, rawspelling->str, rawspelling->freq);

    if (limit > 0 && ++cPrint > limit)
    {
      break;
    }
  }
  printf("\n");
}

void printSpellingBuff(const char *spellingBuf, size_t splItemSize, size_t num, int limit)
{
  LOG(INFO) << "spelling buff num=" << num;
  printf("idx       str freq\n");
  for (size_t i = 0; i < num; i++)
  {
    char szLine[1024] = {0};
    int iPos = 0;
    int cMsg = 0;

    const char *pSpl = spellingBuf + i * splItemSize;
    cMsg = sprintf(szLine + iPos, "%3lu, %7s, %u", i, pSpl, (unsigned char)pSpl[7]);
    iPos += cMsg;
    printf("%s\n", szLine);
    if (limit > 0 && i > limit)
    {
      break;
    }
  }
  printf("\n");
}

void printSpellingNodes(SpellingNode **spellingNodes, size_t num, int limit)
{
  LOG(INFO) << "spelling nodes num=" << num;
  printf("idx spelling_idx this_char num_of_son score\n");
  for (size_t i = 0; i < num; i++)
  {
    SpellingNode *node = spellingNodes[i];
    if (node == nullptr)
      continue;

    printf("%2lu, %11d, %8c, %9d, %d\n", i, node->spelling_idx,
           node->char_this_node, node->num_of_son, node->score);
    if (limit > 0 && i > limit)
    {
      break;
    }
  }
  printf("\n");
}

void printSpellingTrieNodes(SpellingNode *spellingNodes, const char *pre)
{
  for (int i = 0; i < spellingNodes->num_of_son; i++)
  {
    SpellingNode *node = spellingNodes->first_son + i;
    char szPinyin[8] = {0};
    sprintf(szPinyin, "%s%c", pre, node->char_this_node);
    printf("%11d, %8s, %9d, %d\n", node->spelling_idx,
           szPinyin, node->num_of_son, node->score);
  }
  for (int i = 0; i < spellingNodes->num_of_son; i++)
  {
    SpellingNode *node = spellingNodes->first_son + i;
    char szPinyin[8] = {0};
    sprintf(szPinyin, "%s%c", pre, node->char_this_node);
    printSpellingTrieNodes(node, szPinyin);
  }
}

void printSpellingTrie(SpellingTrie &spellingTrie)
{
  printf("spelling_idx this_char num_of_son score\n");
  printSpellingTrieNodes(spellingTrie.root_, "");
}

void printSpellingTrieSavedData(SpellingTrie &spellingTrie)
{
  printf("======== SpellingTrie saved data ========\n");
  printf("spelling_size spelling_num score_amplifier average_score\n");
  printf("%13lu %12lu %15.2f %13d\n", spellingTrie.spelling_size_,
         spellingTrie.spelling_num_, spellingTrie.score_amplifier_,
         spellingTrie.average_score_);
  printSpellingBuff(spellingTrie.spelling_buf_, spellingTrie.spelling_size_,
                    spellingTrie.spelling_num_);
  printf("======== ======== ======== ======== ========\n");
}