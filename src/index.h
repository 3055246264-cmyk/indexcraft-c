#ifndef INDEXCRAFT_INDEX_H
#define INDEXCRAFT_INDEX_H

#include <stddef.h>

#define INDEX_BUCKETS 257
#define MAX_QUERY_TERMS 32

typedef struct Posting {
    size_t doc_id;
    size_t term_frequency;
    struct Posting *next;
} Posting;

typedef struct Term {
    char *text;
    size_t document_frequency;
    Posting *postings;
    struct Term *next;
} Term;

typedef struct Document {
    char *path;
    size_t token_count;
} Document;

typedef struct InvertedIndex {
    Term *buckets[INDEX_BUCKETS];
    Document *documents;
    size_t document_count;
    size_t document_capacity;
    size_t unique_terms;
} InvertedIndex;

typedef enum QueryMode {
    QUERY_AND,
    QUERY_OR
} QueryMode;

typedef struct SearchHit {
    size_t doc_id;
    double score;
} SearchHit;

void index_init(InvertedIndex *index);
void index_free(InvertedIndex *index);
int index_add_file(InvertedIndex *index, const char *path);
size_t index_search(
    const InvertedIndex *index,
    QueryMode mode,
    const char *const terms[],
    size_t term_count,
    SearchHit *hits,
    size_t hit_capacity
);

#endif

