# IndexCraft C

涓€涓敤 C17 瀹炵幇鐨勮交閲忕骇鏂囨。妫€绱㈠紩鎿庛€傚畠鎶婅嫢骞叉枃鏈枃浠舵瀯寤轰负鍊掓帓绱㈠紩锛屾敮鎸?AND / OR 澶氬叧閿瘝鏌ヨ锛屽苟浣跨敤 TF-IDF 椋庢牸鐨勫垎鏁板缁撴灉鎺掑簭銆?
杩欎釜椤圭洰鐨勭洰鏍囦笉鏄爢鍔熻兘锛岃€屾槸鍋氫竴涓璇曡兘璁叉竻妤氥€佷唬鐮侀噺鍙帶銆佽兘鑷劧寤朵几鍒版暟鎹簱涓庢悳绱㈢郴缁熺殑璇濋浣滃搧銆?
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

    Indexed 3 documents and 71 unique terms.
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

## 澶嶈瘯璁茶В鎻愮翰锛?0 绉掞級

1. 鎴戞妸鈥滆瘝 -> 鍑虹幇杩囪璇嶇殑鏂囨。鍒楄〃鈥濆缓鎴愬€掓帓绱㈠紩锛涙煡璇嶄笉鍐嶉渶瑕侀€愮瘒鎵叏鏂囥€?2. 璇嶅吀閲囩敤鍝堝笇琛紝骞冲潎鏌ユ壘澶嶆潅搴︽槸 O(1)锛涙瘡涓瘝涓嬮潰鏄摼寮?postings list锛屼繚瀛樻枃妗ｇ紪鍙峰拰璇嶉銆?3. 鏌ヨ鏃讹紝AND 瑕佹眰鎵€鏈夎瘝閮藉懡涓紝OR 瑕佹眰浠讳竴璇嶅懡涓紱鍛戒腑鍚庣敤 TF-IDF 椋庢牸鍒嗘暟鎺掑簭銆?4. 杩欎釜椤圭洰璁╂垜鎶婇摼琛ㄣ€佸搱甯屻€佹枃浠?I/O銆佹帓搴忋€佹ā鍧楀寲 C 宸ョ▼涓叉垚浜嗕竴鏉″畬鏁撮摼璺€?5. 濡傛灉缁х画鎵╁睍锛屾垜浼氬姞鍊掓帓绱㈠紩钀界洏銆佽烦琛ㄦ垨鍘嬬缉銆佺煭璇绱㈠拰 BM25 鎺掑簭銆?
## Project structure

    src/index.h     Public data structures and APIs
    src/index.c     Hash table, inverted index, tokenizer, query/ranking logic
    src/main.c      CLI interaction
    examples/       Small corpus for a reproducible demo
    .github/        Continuous integration workflow

