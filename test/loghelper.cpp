

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

void printLemmaTitle()
{
  printf( "idx_by_py idx_by_hz hz_str_len    freq hanzi_str  \
              pinyin_str                   hanzi_scis_ids      spl_idx_arr\n");
}

void printLemmaItem(LemmaEntry *pLemma){
  if (pLemma == nullptr)
    return;
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

    printf("%8zu, %8zu, %9u, %6.0f, %9s, %24s, %32s, %24s\n",
           pLemma->idx_by_py, pLemma->idx_by_hz,
           pLemma->hz_str_len, pLemma->freq,
           strHanzi.c_str(), strPy.c_str(),
           strScis.c_str(), strSpl.c_str());
}

void printLemmaArray(LemmaEntry *lemma_arr, size_t num, int limit)
{
  LOG(INFO) << "lemma_num_=" << num;
  printLemmaTitle();

  for (size_t i = 0; i < num; i++)
  {
    ime_pinyin::LemmaEntry *pLemma = lemma_arr + i;
    printLemmaItem(pLemma);
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

void printDictListSavedData(DictList* dictList)
{
  printf("======== DictList saved data ========\n");
  printf("scis_num \n");
  printf("%8lu \n", dictList->scis_num_);
  printf("start_pos start_id\n");
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;

  for(int i=0; i<kMaxLemmaSize; i++)
  {
    printf("%9lu %8lu\n", dictList->start_pos_[i], dictList->start_id_[i]);
  }
  printf("scis_hz scis_splid\n");
  for(int i=0; i<dictList->scis_num_; i++)
  {
    char16 sz[2] = {dictList->scis_hz_[i], 0};
    std::string strHanzi = convert.to_bytes((char16_t *)sz);
    printf("%7s %4u %4u\n", strHanzi.c_str(), 
    dictList->scis_splid_[i].full_splid, dictList->scis_splid_[i].half_splid);
    if(i > 6)
    {
      printf("%7s %10s\n", "...", "...");
      break;
    }
  }
  printf("buf：");
  for(int wordlen=1; wordlen<=4; wordlen++)
  {
    size_t start_pos = dictList->start_pos_[wordlen-1];
    for(int i=0; i<3; i++)
    {
      char16 sz[8] = {0};
      int cbWords = wordlen * sizeof(char16);
      memcpy(sz, dictList->buf_ + start_pos + i * cbWords, cbWords);
      std::string strHanzi = convert.to_bytes((char16_t *)sz);
      printf("%s", strHanzi.c_str());
    }
    printf("...");
  }
  printf("\n");
  printf("======== ======== ======== ======== ========\n");
}

void printLemmaArrayByHzIdx(LemmaEntry* lemma, size_t num, int idx)
{
  for(int i=0; i<num; i++){
    if (lemma[i].idx_by_hz == idx)
    {
      printLemmaTitle();
      printLemmaItem(lemma + i);
      return;
    }
  }
  printf("lemma NOT exits! idx_by_hz=%d\n", idx);
}

void printDictTrieSavedData(DictTrie* dictTrie)
{
  printf("========    DictTrie saved data    ========\n");
  printf("lma_node_num_le0 lma_node_num_ge1 lma_idx_buf_len top_lmas_num\n");
  printf("%16lu %16lu %15lu %12lu\n", dictTrie->lma_node_num_le0_,
         dictTrie->lma_node_num_ge1_, dictTrie->lma_idx_buf_len_,
         dictTrie->top_lmas_num_);
  printf("======== ======== ======== ======== ========\n");
}


void printMartixRowTitle()
{
  printf("========    matrix row    ========\n");
  printf("[ ] mtrx_nd_pos dmi_pos mtrx_nd_num dmi_num dmi_has_full_id mtrx_nd_fixed\n");
}

void printMatrixRows(MatrixRow *rows, size_t num, MatrixNode* head)
{
  if (rows == nullptr)
    return;
  printMartixRowTitle();
  for (int i = 0; i < num; i++)
  {
    MatrixRow *row = rows + i;
    int offset = row->mtrx_nd_fixed == nullptr ? -1 : row->mtrx_nd_fixed - head;
    printf("%-3d %11u %7u %11u %7u %15u %13d\n", i, row->mtrx_nd_pos, row->dmi_pos,
           row->mtrx_nd_num, row->dmi_num, row->dmi_has_full_id, offset);
  }
  printf("\n");
}

void printDictExtPara(DictExtPara* dep)
{
  if(dep == nullptr)
    return;
  printf("========    DictExtPara    ========\n");
  printf("splids_extended ext_len step_no splid_end_split id_start id_num splids\n");
  printf("%15u %7u %7u %15d %8u %6u [", dep->splids_extended, dep->ext_len,
         dep->step_no, dep->splid_end_split, dep->id_start, dep->id_num);
        
  for(int i=0; i<kMaxSearchSteps; i++){
    if(dep->splids[i] == 0)
      break;
    if(i>0)
      printf(", ");
    printf("%u", dep->splids[i]);
  }
  printf("]\n\n");
}

void printDictMatchInfo(DictMatchInfo* dmi, size_t num)
{
  if(dmi == nullptr)
    return;
  SpellingTrie &spl_trie = SpellingTrie::get_instance();
  printf("========    DictMatchInfo    ========\n");
  printf("[ ] dict_handles dmi_fr spl_id dict_level c_phrase splid_end_split splstr_len all_full_id\n");
  for(int i=0; i<num; i++){
    const char* pinyin = spl_trie.get_spelling_str(dmi[i].spl_id);
    printf("%-3d       [%1d, %1d] %6u %6u %10u %8u %15u %10u %11u %s\n",
           i, dmi[i].dict_handles[0], dmi[i].dict_handles[1], dmi[i].dmi_fr,
           dmi[i].spl_id, dmi[i].dict_level, dmi[i].c_phrase,
           dmi[i].splid_end_split, dmi[i].splstr_len, dmi[i].all_full_id, pinyin);
  }
  printf("\n");
}

void printMatrixNode(MatrixNode* nd, size_t num, MatrixSearch *matrix_search)
{
  if(nd == nullptr)
    return;
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  printf("========    MatrixNode    ========\n");
  printf("[ ]    id score from dmi_fr step\n");
  for(int i=0; i<num; i++){
    int offset = nd[i].from == nullptr? -1 : nd[i].from - nd;
    char16 str[kMaxLemmaSize + 1];
    size_t str_len = matrix_search->get_lemma_str(nd[i].id, str, kMaxLemmaSize);
    std::string strHanzi = convert.to_bytes((char16_t*)str);
    printf("%-3d %5lu %5.0f %4d %6u %4u %s\n",
           i, nd[i].id, nd[i].score, offset, nd[i].dmi_fr, nd[i].step,
           nd[i].id > 0 ? strHanzi.c_str() : "");
  }
  printf("\n");
}

void printLmaPsbItem(LmaPsbItem* lpi, size_t num)
{
  if(lpi == nullptr)
    return;

  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  printf("========    LmaPsbItem    ========\n");
  printf("[ ]    id lma_len   psb hanzi\n");
  for(int i=0; i<num; i++){
    std::string strHanzi = convert.to_bytes((char16_t)lpi[i].hanzi);
    printf("%-3d %5u %7u %5u %s\n", i, lpi[i].id, lpi[i].lma_len, lpi[i].psb, strHanzi.c_str());
    if(i>5){
      printf("...共%lu个\n", num);
      break;
    }
  }
  printf("\n");
}