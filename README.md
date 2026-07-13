# IndexCraft C

一个用 C17 实现的轻量级文档检索引擎。它把若干文本文件构建为倒排索引，支持 AND / OR 多关键词查询，并使用 TF-IDF 风格的分数对结果排序。

这个项目的目标不是堆功能，而是做一个复试能讲清楚、代码量可控、能自然延伸到数据库与搜索系统的话题作品。

## Features

- Hash table: stores vocabulary and maps each term to its postings list.
- Inverted index: each term points to documents containing it and their term frequencies.
- Tokenizer: case-insensitive extraction of alphanumeric tokens.
- Boolean retrieval: AND returns common documents; OR returns any matching document.
- Ranking: combines normalized term frequency with inverse document frequency.
- Engineering basics: CMake, Makefile, warning flags, sample corpus, and GitHub Actions CI.

## Quick start

### CMake

    cmake -S . -B build
    cmake --build build
    ./build/indexcraft examples/data_structures.txt examples/systems.txt examples/algorithms.txt

### GCC

    gcc -std=c17 -O2 -Wall -Wextra -Wpedantic -o indexcraft src/main.c src/index.c -lm
    ./indexcraft examples/data_structures.txt examples/systems.txt examples/algorithms.txt

Then try:

    AND data structure
    OR search index
    STATS
    QUIT

## Example

    Indexed 3 documents and 64 unique terms.
    Commands: AND <terms>, OR <terms>, STATS, QUIT

    indexcraft> AND data structure
     1. score=0.2135  examples/data_structures.txt
     2. score=0.1532  examples/algorithms.txt

## Design

For a term such as search, the index is conceptually:

    search -> [(doc 0, tf=1), (doc 2, tf=1)]

During query evaluation, the engine scans the indexed documents, checks whether each query term has a posting for the document, and accumulates:

    score = sum((term_frequency / document_token_count) * IDF)
    IDF = log((N + 1) / (document_frequency + 1)) + 1

This is deliberately a small, readable baseline. A production search engine would add persistent storage, a better tokenizer, compressed postings, phrase queries, and a real ranking model.

## 复试讲解提纲（90 秒）

1. 我把“词 -> 出现过该词的文档列表”建成倒排索引；查词不再需要逐篇扫全文。
2. 词典采用哈希表，平均查找复杂度是 O(1)；每个词下面是链式 postings list，保存文档编号和词频。
3. 查询时，AND 要求所有词都命中，OR 要求任一词命中；命中后用 TF-IDF 风格分数排序。
4. 这个项目让我把链表、哈希、文件 I/O、排序、模块化 C 工程串成了一条完整链路。
5. 如果继续扩展，我会加倒排索引落盘、跳表或压缩、短语检索和 BM25 排序。

## Project structure

    src/index.h     Public data structures and APIs
    src/index.c     Hash table, inverted index, tokenizer, query/ranking logic
    src/main.c      CLI interaction
    examples/       Small corpus for a reproducible demo
    .github/        Continuous integration workflow

