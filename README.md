#TransferFile
##Windows
###1、环境Windows 10，vs2019，c++17。依赖libzmq（自行去官网下载编译）
###2、cd server&&cmake .&&cmake --build .
###3、cd client&&cmake .&&cmake --build .
##Linux
###1、环境Ubuntu14.08，gcc10，c++17。依赖libzmq（自行去官网下载编译）
###2、cd server
###3、g++ -std=c++17 transferc.cpp -I ../include -lzmq -liconv -o transc 
