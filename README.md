# IndexCraft C

轻量级 C17 文档检索引擎：将多个文本文件构建为倒排索引，支持 AND / OR 多关键词查询，并以 TF-IDF 风格评分排序结果。

这个项目面向计算机复试展示：功能完整但规模可控，可以清楚说明哈希表、链表、文件 I/O、查询处理与排序如何协作。

## Features

- 哈希表词典：平均 O(1) 查找词项。
- 倒排索引：每个词记录出现过它的文档编号和词频。
- 链式 postings list：适合解释链表与动态数据组织。
- 布尔检索：AND 返回共同命中的文档，OR 返回任一命中的文档。
- 相关度排序：使用归一化 TF 与 IDF 的组合评分。
- 工程化配置：CMake、Makefile、示例语料与 GitHub Actions CI。

## Quick start

### CMake

    cmake -S . -B build
    cmake --build build
    ./build/indexcraft examples/data_structures.txt examples/systems.txt examples/algorithms.txt

### GCC

    gcc -std=c17 -O2 -Wall -Wextra -Wpedantic -o indexcraft src/main.c src/index.c -lm
    ./indexcraft examples/data_structures.txt examples/systems.txt examples/algorithms.txt

运行后可输入：

    AND data structure
    OR search index
    STATS
    QUIT

## Design

倒排索引的核心映射为：

    term -> [(document_id, term_frequency), ...]

查询时，程序判断每个文档是否包含各个词项：AND 要求全部命中，OR 要求至少一个命中。结果得分为：

    score = sum((term_frequency / document_token_count) * IDF)
    IDF = log((N + 1) / (document_frequency + 1)) + 1

## 复试 90 秒讲解

1. 我实现的是一个轻量级文本检索系统，核心数据结构是倒排索引。
2. 词典采用哈希表，词项查找平均是 O(1)；每个词下面通过链表保存文档编号和词频。
3. 查询支持 AND 和 OR：前者筛选所有关键词同时出现的文档，后者筛选任意关键词出现的文档。
4. 我没有只返回命中结果，而是用 TF-IDF 风格的分数对文档排序，因此结果有相关度概念。
5. 继续扩展时，我会加入索引持久化、短语检索、压缩 postings 与 BM25 排序。

## Project structure

    src/index.h     Public data structures and APIs
    src/index.c     Hash table, inverted index, tokenizer, query and ranking logic
    src/main.c      CLI interaction
    examples/       Small corpus for a reproducible demo
    .github/        Continuous integration workflow