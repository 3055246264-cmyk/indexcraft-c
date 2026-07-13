#include "index.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *copy_string(const char *source) {
    size_t length = strlen(source) + 1;
    char *copy = malloc(length);
    if (copy != NULL) {
        memcpy(copy, source, length);
    }
    return copy;
}

static unsigned long hash_text(const char *text) {
    unsigned long hash = 5381;
    unsigned char ch;
    while ((ch = (unsigned char)*text++) != 0) {
        hash = ((hash << 5) + hash) ^ ch;
    }
    return hash % INDEX_BUCKETS;
}

static Term *find_term(const InvertedIndex *index, const char *text) {
    Term *term = index->buckets[hash_text(text)];
    while (term != NULL) {
        if (strcmp(term->text, text) == 0) {
            return term;
        }
        term = term->next;
    }
    return NULL;
}

static Term *get_or_create_term(InvertedIndex *index, const char *text) {
    unsigned long bucket = hash_text(text);
    Term *term = find_term(index, text);
    if (term != NULL) {
        return term;
    }

    term = calloc(1, sizeof(*term));
    if (term == NULL) {
        return NULL;
    }
    term->text = copy_string(text);
    if (term->text == NULL) {
        free(term);
        return NULL;
    }
    term->next = index->buckets[bucket];
    index->buckets[bucket] = term;
    index->unique_terms++;
    return term;
}

static int grow_documents(InvertedIndex *index) {
    if (index->document_count < index->document_capacity) {
        return 1;
    }
    size_t new_capacity = index->document_capacity == 0 ? 8 : index->document_capacity * 2;
    Document *resized = realloc(index->documents, new_capacity * sizeof(*resized));
    if (resized == NULL) {
        return 0;
    }
    index->documents = resized;
    index->document_capacity = new_capacity;
    return 1;
}

static int add_token(InvertedIndex *index, const char *token, size_t doc_id) {
    Term *term = get_or_create_term(index, token);
    if (term == NULL) {
        return 0;
    }
    Posting *posting = term->postings;
    while (posting != NULL && posting->doc_id != doc_id) {
        posting = posting->next;
    }
    if (posting == NULL) {
        posting = calloc(1, sizeof(*posting));
        if (posting == NULL) {
            return 0;
        }
        posting->doc_id = doc_id;
        posting->term_frequency = 1;
        posting->next = term->postings;
        term->postings = posting;
        term->document_frequency++;
    } else {
        posting->term_frequency++;
    }
    return 1;
}

void index_init(InvertedIndex *index) {
    memset(index, 0, sizeof(*index));
}

void index_free(InvertedIndex *index) {
    for (size_t i = 0; i < INDEX_BUCKETS; ++i) {
        Term *term = index->buckets[i];
        while (term != NULL) {
            Term *next_term = term->next;
            Posting *posting = term->postings;
            while (posting != NULL) {
                Posting *next_posting = posting->next;
                free(posting);
                posting = next_posting;
            }
            free(term->text);
            free(term);
            term = next_term;
        }
    }
    for (size_t i = 0; i < index->document_count; ++i) {
        free(index->documents[i].path);
    }
    free(index->documents);
    memset(index, 0, sizeof(*index));
}

int index_add_file(InvertedIndex *index, const char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL || !grow_documents(index)) {
        if (file != NULL) {
            fclose(file);
        }
        return 0;
    }

    size_t doc_id = index->document_count;
    index->documents[doc_id].path = copy_string(path);
    index->documents[doc_id].token_count = 0;
    if (index->documents[doc_id].path == NULL) {
        fclose(file);
        return 0;
    }

    char token[128];
    size_t length = 0;
    int character;
    int ok = 1;
    while ((character = fgetc(file)) != EOF) {
        if (isalnum((unsigned char)character)) {
            if (length + 1 < sizeof(token)) {
                token[length++] = (char)tolower((unsigned char)character);
            }
        } else if (length > 0) {
            token[length] = '\0';
            ok = add_token(index, token, doc_id);
            index->documents[doc_id].token_count++;
            length = 0;
            if (!ok) {
                break;
            }
        }
    }
    if (ok && length > 0) {
        token[length] = '\0';
        ok = add_token(index, token, doc_id);
        index->documents[doc_id].token_count++;
    }
    fclose(file);

    if (!ok) {
        free(index->documents[doc_id].path);
        return 0;
    }
    index->document_count++;
    return 1;
}

static const Posting *posting_for_doc(const Term *term, size_t doc_id) {
    const Posting *posting = term == NULL ? NULL : term->postings;
    while (posting != NULL) {
        if (posting->doc_id == doc_id) {
            return posting;
        }
        posting = posting->next;
    }
    return NULL;
}

static int compare_hits(const void *left, const void *right) {
    const SearchHit *a = left;
    const SearchHit *b = right;
    if (a->score < b->score) return 1;
    if (a->score > b->score) return -1;
    return (a->doc_id > b->doc_id) - (a->doc_id < b->doc_id);
}

size_t index_search(
    const InvertedIndex *index,
    QueryMode mode,
    const char *const terms[],
    size_t term_count,
    SearchHit *hits,
    size_t hit_capacity
) {
    if (term_count == 0 || hit_capacity == 0) {
        return 0;
    }

    const Term *resolved[MAX_QUERY_TERMS];
    if (term_count > MAX_QUERY_TERMS) {
        term_count = MAX_QUERY_TERMS;
    }
    for (size_t i = 0; i < term_count; ++i) {
        resolved[i] = find_term(index, terms[i]);
    }

    size_t hit_count = 0;
    for (size_t doc_id = 0; doc_id < index->document_count && hit_count < hit_capacity; ++doc_id) {
        size_t matches = 0;
        double score = 0.0;
        for (size_t i = 0; i < term_count; ++i) {
            const Posting *posting = posting_for_doc(resolved[i], doc_id);
            if (posting != NULL) {
                matches++;
                double tf = (double)posting->term_frequency / index->documents[doc_id].token_count;
                double idf = log((double)(index->document_count + 1) /
                                 (double)(resolved[i]->document_frequency + 1)) + 1.0;
                score += tf * idf;
            }
        }
        int include = mode == QUERY_AND ? matches == term_count : matches > 0;
        if (include) {
            hits[hit_count++] = (SearchHit){doc_id, score};
        }
    }
    qsort(hits, hit_count, sizeof(*hits), compare_hits);
    return hit_count;
}

