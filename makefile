COM=gcc

all: dic seq trie arbre hash suf

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
	rm -f dic seq trie arbre hash

