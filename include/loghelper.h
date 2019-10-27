

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
