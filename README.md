# TransferFile<br>
## Windows<br>
### 1、环境Windows 10，Visual Studio 2019，c++17。依赖libzmq（可自行去官网下载编译）<br>
### 2、cd server&&cmake .&&cmake --build .<br>
### 3、cd client&&cmake .&&cmake --build .<br>
## Linux<br>
### 1、环境Ubuntu 18.04.3 LTS，gcc 10.2.0 ，c++17。依赖libzmq（可自行去官网下载编译）<br>
### 2、cd server<br>
### 3、g++ -std=c++17 transferc.cpp -I ../include -lzmq -liconv -o transc <br>
