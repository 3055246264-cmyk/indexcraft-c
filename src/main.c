#include "index.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define INPUT_SIZE 512

static void print_usage(const char *program) {
    printf("Usage: %s <document1.txt> <document2.txt> ...\n", program);
    printf("Then enter queries, for example: AND data structure\n");
}

static void lowercase(char *text) {
    for (; *text != '\0'; ++text) {
        *text = (char)tolower((unsigned char)*text);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    InvertedIndex index;
    index_init(&index);
    for (int i = 1; i < argc; ++i) {
        if (!index_add_file(&index, argv[i])) {
            fprintf(stderr, "Could not index: %s\n", argv[i]);
            index_free(&index);
            return 1;
        }
    }

    printf("Indexed %llu documents and %llu unique terms.\n",
           (unsigned long long)index.document_count,
           (unsigned long long)index.unique_terms);
    printf("Commands: AND <terms>, OR <terms>, STATS, QUIT\n");

    char line[INPUT_SIZE];
    while (1) {
        fputs("\nindexcraft> ", stdout);
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        char *parts[MAX_QUERY_TERMS + 1];
        size_t count = 0;
        char *token = strtok(line, " \t\r\n");
        while (token != NULL && count < MAX_QUERY_TERMS + 1) {
            lowercase(token);
            parts[count++] = token;
            token = strtok(NULL, " \t\r\n");
        }
        if (count == 0) {
            continue;
        }
        if (strcmp(parts[0], "quit") == 0 || strcmp(parts[0], "exit") == 0) {
            break;
        }
        if (strcmp(parts[0], "stats") == 0) {
            printf("documents=%llu, unique_terms=%llu\n",
                   (unsigned long long)index.document_count,
                   (unsigned long long)index.unique_terms);
            continue;
        }
        if ((strcmp(parts[0], "and") != 0 && strcmp(parts[0], "or") != 0) || count < 2) {
            puts("Use AND <terms>, OR <terms>, STATS, or QUIT.");
            continue;
        }

        QueryMode mode = strcmp(parts[0], "and") == 0 ? QUERY_AND : QUERY_OR;
        SearchHit hits[128];
        size_t hit_count = index_search(
            &index, mode, (const char *const *)&parts[1], count - 1, hits, 128
        );
        if (hit_count == 0) {
            puts("No matching documents.");
            continue;
        }
        for (size_t i = 0; i < hit_count; ++i) {
            printf("%2llu. score=%.4f  %s\n",
                   (unsigned long long)(i + 1),
                   hits[i].score,
                   index.documents[hits[i].doc_id].path);
        }
    }

    index_free(&index);
    puts("Bye.");
    return 0;
}

