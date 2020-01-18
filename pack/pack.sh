# ！/bin/bash

cd ../src/

#make clean
mkdir ai_platform/ai_runtime/bin/ -p
make
cd -
cd ai_platform/ai_runtime/bin/
sudo ./AIService
