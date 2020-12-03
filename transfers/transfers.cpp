/*****************************************************************************************************
    Copyright (c) 2020 Contributors as noted in the AUTHORS file

    This file is part of TransferFile.

    TransferFile is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    TransferFile is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	*  @file     Transfers.cpp                                                   *
	*  @brief    对文件的简述                                                    *
	*  Details.                                                                  *
	*                                                                            *
	*  @author   Peter.cui                                                       *
	*  @email    PeterChy@yeah.net                                               *
	*  @version  1.0.0.1(版本号)                                                 *
	*  @date     2020-12-03                                                      *
	*  @license  GNU General Public License (GPL)     

********************************************************************************************************/
#include <string>
#include <io.h>
#include <direct.h>

#include <iostream>
#include <fstream>
#include "zmq.h"
#include "zmq_utils.h"            //Zeromq 函数的导入
#pragma comment(lib,"libzmq.lib")

using namespace std;
int main()
{ 
    void* context=zmq_init(1);    //指定zmq 处理I/0事件的thread pool 为1
    void* z_socket=zmq_socket(context,ZMQ_REP);
    zmq_bind(z_socket,"tcp://*:8089");    // accept connections on a socket
	std::cout<<"Waiting for the connection"<<std::endl;
    while(true)    //循环工作
    {    
        //接受文件名
        zmq_msg_t recv_msg;
        zmq_msg_init(&recv_msg);
        int size =zmq_msg_recv(&recv_msg,z_socket,0);          //0表示非阻塞
		char *str = (char*)malloc(size + 1);
		memset(str,0,size+1);
		memcpy(str, zmq_msg_data(&recv_msg), size);
		zmq_msg_close(&recv_msg);
		char  * tag=nullptr;
		for (tag = str; *tag; tag++)
		{
			if (*tag == '\\')
			{
				char buf[1000]={0};
				char path[1000]={0};
				strcpy(buf, str);
				buf[strlen(str) - strlen(tag) + 1] = NULL;
				strcpy(path, buf);
				_mkdir(path);			
			}
		}
		string filepath(str);
		free (str);
		std::cout<<filepath<<std::endl;    
		
		//发送完成
		zmq_msg_t send_msg;
		zmq_msg_init_size(&send_msg,6);
		memcpy(zmq_msg_data(&send_msg),"over.",6);
		zmq_sendmsg(z_socket,&send_msg,0);
		zmq_msg_close(&send_msg);


		//接受文件
		zmq_msg_t recv_msg1;
		zmq_msg_init(&recv_msg1);
		size =zmq_msg_recv(&recv_msg1,z_socket,0);                            //0表示非阻塞		
		//write to outfile
		std::fstream wfile(filepath,std::fstream::out|std::fstream::binary);
		wfile.write(((char*)zmq_msg_data(&recv_msg1)),zmq_msg_size (&recv_msg1));
		wfile.close();
		zmq_msg_close(&recv_msg1);


        //发送完成
        zmq_msg_t send_msg1;
        zmq_msg_init_size(&send_msg1,6);
        memcpy(zmq_msg_data(&send_msg1),"over.",6);
        zmq_sendmsg(z_socket,&send_msg1,0);
        zmq_msg_close(&send_msg1);

    }
    zmq_close(z_socket);
    zmq_term(context);          
    return 0;
}
