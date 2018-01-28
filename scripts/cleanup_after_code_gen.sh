#!/bin/bash
pushd .
cd "${0%/*}"

CODE_DIR="../code_gen_temp"

mkdir -p ${CODE_DIR}
rm -rf ${CODE_DIR}/*
cp -a ../code_gen/{Inc,Src} ${CODE_DIR}/.

find ${CODE_DIR} -type f -exec sed -i '/\/\* USER CODE /d' {} \;
find ${CODE_DIR} -type f -exec dos2unix -k -s -o {} \;
find ${CODE_DIR} -type f -exec sed -i 'N;/^\n$/d;P;D' {} \;
find ${CODE_DIR} -type f -exec ./indent_with_vim.sh {} \;

popd
