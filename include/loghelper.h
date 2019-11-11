

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

using namespace ime_pinyin;

void printLemmaArray(LemmaEntry* lemma_arr, size_t num, int limit=10);
void printSpellingTable(SpellingTable* spellingTable, int limit=10);
void printSpellingBuff(const char *spellingBuf, size_t splItemSize, size_t num, int limit=10);
void printSpellingNodes(SpellingNode** spellingNodes, size_t num, int limit=10);
void printSpellingTrie(SpellingTrie& spellingTrie);
void printSpellingTrieNodes(SpellingNode* spellingNodes, const char* pre);

// 打印spellingTrie被保存到磁盘的数据
void printSpellingTrieSavedData(SpellingTrie& spellingTrie);
// 打印dictList被保存到磁盘的数据
void printDictListSavedData(DictList* dictList);
// 打印dictTrie被保存到磁盘的数据
void printDictTrieSavedData(DictTrie* dictTrie);

void printLemmaArrayByHzIdx(LemmaEntry* lemma, size_t num, int idx);

void printMatrixRows(MatrixRow* row, size_t num, MatrixNode* head);
void printDictExtPara(DictExtPara* dep);
void printDictMatchInfo(DictMatchInfo* dmi, size_t num);
void printMatrixNode(MatrixNode* nd, size_t num, MatrixSearch *matrix_search);
void printLmaPsbItem(LmaPsbItem* lpi, size_t num);