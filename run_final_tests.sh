#!/bin/bash
#
# Recorre todos los archivos del directorio actual y los muestra
#
RED='\033[0;31m'
NC='\033[0m' # No Color

for i in $(ls tests/final-tests -C1)
do
echo -e "${RED}Current file: " $i "${NC}"
echo " "
./c-tds ./tests/final-tests/$i
echo " "
echo "Press enter to continue...."
read -n 1 key
done
echo "------------------  NO MORE TEST TO RUN  -------------------"

