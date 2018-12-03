#!/bin/bash
#
# Recorre todos los archivos del directorio actual y los muestra
#
RED='\033[0;31m'
NC='\033[0m' # No Color

echo " "
echo "---------------  SYNTAX TESTS  ---------------"
echo " "
echo "  All this test should fail"
echo " "

for i in $(ls tests/sintax-semanth/should_fail -C1)
do
echo -e "${RED}Current file: " $i "${NC}"
./c-tds ./tests/sintax-semanth/should_fail/$i
echo ""
echo "Press enter to continue...."
read -n 1 key
done
echo "------------------  NO MORE TEST TO RUN  -------------------"
