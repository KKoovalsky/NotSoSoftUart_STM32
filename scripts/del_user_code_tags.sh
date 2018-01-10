#!/bin/bash
pushd .
cd "${0%/*}"
find ../app/. -type f -exec sed -i '/\/\* USER CODE /d' {} \;
popd
