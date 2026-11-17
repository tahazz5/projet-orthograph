COM=gcc

CC ?= cc
CPPFLAGS ?= -Iinclude
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Wpedantic

.PHONY: all clean test sanitize benchmarks dic seq trie arbre hash suf
all: bin/correcteur

bin/correcteur: src/Correcteur.c src/dictionary.c include/dictionary.h
	@mkdir -p bin
	$(CC) $(CPPFLAGS) $(CFLAGS) src/Correcteur.c src/dictionary.c $(LDFLAGS) -o $@

test: bin/correcteur
	python3 tests/test_correcteur.py

bin/correcteur-sanitize: src/Correcteur.c src/dictionary.c include/dictionary.h
	@mkdir -p bin
	$(CC) $(CPPFLAGS) -std=c11 -g -O1 -Wall -Wextra -Wpedantic -fsanitize=address,undefined -fno-omit-frame-pointer src/Correcteur.c src/dictionary.c $(LDFLAGS) -fsanitize=address,undefined -o $@

sanitize: bin/correcteur-sanitize
	ASAN_OPTIONS=detect_leaks=1 CORRECTEUR_BINARY=bin/correcteur-sanitize python3 tests/test_correcteur.py

benchmarks: dic seq trie arbre hash suf

dic: ./test/test_rech.c ./src/dictionaire_tools.c  ./include/dictionaire_tools.h
	$(COM) ./test/test_rech.c ./src/dictionaire_tools.c -o ./bin/dic

seq: test/test_seq.c ./src/dictionaire_tools.c  ./include/dictionaire_tools.h
	$(COM) test/test_seq.c ./src/dictionaire_tools.c -o ./bin/seq


trie: test/compare_temps_TRIE.c src/Trie.c include/Trie.h
	$(COM) test/compare_temps_TRIE.c src/Trie.c -o ./bin/trie

arbre: test/compare_temps_ARBRE.c src/Arbre.c include/Arbre.h
	$(COM) test/compare_temps_ARBRE.c src/Arbre.c -o ./bin/arbre

hash: test/compare_temps_HASH.c src/hashTable.c include/hashTable.h
	$(COM) test/compare_temps_HASH.c  src/hashTable.c -o ./bin/hash

suf: test/compare_temps_ARBRE_SUF.c  src/ArbreSuffixe.c src/Arbre.c include/ArbreSuffixe.h include/Arbre.h
	$(COM) test/compare_temps_ARBRE_SUF.c src/ArbreSuffixe.c src/Arbre.c -o ./bin/suf


clean:
	rm -f bin/correcteur bin/correcteur-sanitize bin/dic bin/seq bin/trie bin/arbre bin/hash bin/suf

