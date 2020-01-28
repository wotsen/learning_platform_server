# !/bin/bash

# cd 路径，和setup.py里的路径要一致

cd bin

./systemcmd_proxyd &
sleep 1
sudo ./AIService &
