# Correcteur orthographique en C

Correcteur en ligne de commande avec dictionnaire français fourni, trois méthodes
 de recherche et suggestions. Le programme affiche les mots inconnus, puis un
bilan et les temps CPU de chargement, d'analyse et de libération.

## Compiler et utiliser

Prérequis : compilateur C11 (GCC ou Clang), Make et locale UTF-8.
Exécuter les commandes depuis la racine du projet :

```sh
make
./bin/correcteur tests/exemple.txt
./bin/correcteur --suggest tests/exemple.txt
./bin/correcteur -m trie -d text/dico1.txt -r rapport.txt tests/exemple.txt
printf 'Bonjour chatt' | ./bin/correcteur -s -
./bin/correcteur --help
```

| Option | Fonction |
| --- | --- |
| `-d`, `--dictionary FICHIER` | Dictionnaire personnalisé, par défaut `text/dico1.txt` |
| `-m`, `--method METHODE` | `hash` (défaut), `trie` ou `seq` |
| `-s`, `--suggest` | Jusqu'à cinq suggestions par mot inconnu |
| `-r`, `--report FICHIER` | Un mot inconnu normalisé par ligne, répétitions conservées |
| `-h`, `--help` | Aide |

Le texte est obligatoire ; `-` lit l'entrée standard. Le code de retour est `0`
si l'analyse réussit, même avec des mots inconnus, et `1` en cas d'erreur.
Un rapport ne peut pas remplacer le texte ou le dictionnaire, y compris via un
lien symbolique. Un échec de lecture préserve le rapport précédent.

## Comportement

- Texte et dictionnaire doivent être encodés en UTF-8. Les entrées invalides
  provoquent une erreur explicite.
- Les majuscules, accents français et ligatures sont normalisés : `ÉCOLE`
  devient `ecole`, `cœur` devient `coeur`. Cela correspond au dictionnaire fourni,
  qui est essentiellement sans accents. Le correcteur ne détecte donc pas les
  erreurs d'accentuation et affiche les mots sous leur forme normalisée.
- Ponctuation, chiffres, apostrophes et traits d'union séparent les mots.
  Les élisions et mots composés sont vérifiés composant par composant ; aucune
  analyse grammaticale n'est effectuée. Les accents Unicode décomposés ne sont
  pas recomposés : privilégier un texte Unicode NFC.
- Les doublons du dictionnaire sont supprimés après normalisation. Un dictionnaire
  vide est refusé. La limite est de 255 octets par mot normalisé : un mot trop long
  dans le texte est compté et ignoré avec un avertissement ; dans le dictionnaire,
  il provoque une erreur.
- Les suggestions utilisent la distance de Levenshtein, limitée à deux éditions,
  puis l'ordre alphabétique des formes normalisées. Elles n'appliquent aucune
  modification au fichier. Sur un gros texte, leur calcul peut être coûteux.

## Méthodes

Les trois méthodes utilisent le même vocabulaire et doivent retourner les mêmes
résultats. `hash` utilise une table avec chaînage des collisions ; `trie` un arbre
préfixe à enfants liés ; `seq` une recherche séquentielle, utile pour comparer les
performances mais plus lente sur le dictionnaire complet. Les temps CPU d'analyse
incluent la lecture, l'affichage et les suggestions éventuelles.

## Vérifier

Python 3 est nécessaire uniquement pour les tests :

```sh
make test
make sanitize
make clean
```

`make sanitize` utilise AddressSanitizer, UndefinedBehaviorSanitizer et la détection
des fuites dans un exécutable séparé. LeakSanitizer nécessite un environnement
sans surveillance `ptrace`.

Les tests couvrent les trois méthodes, accents, doublons, fin de fichier sans saut
de ligne, suggestions et classement, entrée standard, texte vide, mots trop longs,
UTF-8 invalide, options invalides et protection des fichiers d'entrée.

## Organisation

- `src/Correcteur.c` : interface et rapports.
- `src/dictionary.c`, `include/dictionary.h` : lecture normalisée, index et suggestions.
- `tests/` : tests automatisés et exemple.
- `text/dico1.txt` : dictionnaire fourni.
- `src/Trie.c`, `src/Arbre.c`, `src/ArbreSuffixe.c`, `src/hashTable.c`,
  `src/dictionaire_tools.c`, `src/list.c` et `test/` : anciennes expériences conservées.
  Elles ne sont pas utilisées par le nouveau correcteur. `make benchmarks` compile
  les anciens programmes de comparaison ; leur code expérimental n'a pas été fiabilisé.
- `data/`, `img/` : mesures et graphiques historiques.
