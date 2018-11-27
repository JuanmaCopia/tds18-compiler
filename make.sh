#!/bin/bash
cd src/
flex ctds-lexicon.l
bison -d -v ctds-syntax.y
gcc lex.yy.c ctds-syntax.tab.c
mv a.out parser.out
cd ..
mkdir -p build
mv src/parser.out src/ctds-syntax.output src/ctds-syntax.tab.c src/ctds-syntax.tab.h src/lex.yy.c ./build

echo " "
echo "The C-TDS parser was successfully generated."
echo "To compile a C-TDS program write: "
echo " "
echo "./c-tds [FILENAME.tds]"
echo " "