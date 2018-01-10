#!/bin/bash
pushd .
cd "${0%/*}"
find ../app/. -type f -exec sed -i '/\/\* USER CODE /d' {} \;
find ../app/. -type f -print0 | xargs -0 dos2unix
find ../app/. -type f -exec sed -i 'N;/^\n$/d;P;D' {} \;
find ../app/. | xargs -I {} ./indent_with_vim.sh {}
popd
