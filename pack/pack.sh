# ！/bin/bash

cd ../service/app/

make clean
make
cd -
cd ai_platform/ai_runtime/bin/
sudo ./AIService
