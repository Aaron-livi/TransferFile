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

	*  @file     Transferc.cpp                                                   *
	*  @brief    ���ļ��ļ���                                                    *
	*  Details.                                                                  *
	*                                                                            *
	*  @author   Peter.cui                                                       *
	*  @email    PeterChy@yeah.net                                               *
	*  @version  1.0.0.1(�汾��)                                                 *
	*  @date     2020-12-03                                                      *
	*  @license  GNU General Public License (GPL)     

********************************************************************************************************/

#include <string>
#include <iostream> 
#include <fstream> 
#include<vector>
#include<io.h>

#include "zmq.h"
#include "zmq_utils.h"            //Zeromq �����ĵ���
#pragma comment(lib,"libzmq.lib")

using namespace std;

void getFiles(string path, vector<string>& files)
{
	//�ļ����  
	long long hFile = 0;//����ط���Ҫ�ر�ע�⣬win10�û�������long long ���ͣ�win7������long����
	//�ļ���Ϣ  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮  
			//�������,�����б�  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
int main (int argc, char *argv [])
{
	char *path=nullptr;
	path = argv [1];
	vector<string> files;
	if (path==nullptr)
	{
		std::cout<<"Please enter the path you want to transfer to the server"<<std::endl;
		path=new char[255];
		cin >> path;	
		getFiles(path, files);
		delete[] path;
	}
	else
	{
		getFiles(path, files);
	}
	void* context=zmq_init(1);    //ָ��zmq ����I/0�¼���thread pool Ϊ1
	void* z_socket=zmq_socket(context,ZMQ_REQ);
	zmq_connect(z_socket,"tcp://localhost:8089");    // accept connections on a socket

	int size = files.size();
	for (int i = 0; i < size; i++)
	{
		cout << files[i].c_str() << endl;
		//����
		zmq_msg_t send_msg;
		zmq_msg_init_size(&send_msg,files[i].length());
		memcpy(zmq_msg_data(&send_msg),files[i].c_str(),files[i].length());
		zmq_msg_send(&send_msg,z_socket,0);
		zmq_msg_close(&send_msg);

		//���ܲ���
		zmq_msg_t recv_msg;
		zmq_msg_init(&recv_msg);
		zmq_msg_recv(&recv_msg,z_socket,0);
		printf("Server:\t");
		std::cout<<(char*)zmq_msg_data(&recv_msg)<<std::endl;
		zmq_msg_close(&recv_msg);

		int length;
		char *buffer=nullptr;

		std::fstream rfile;
		rfile.open(files[i].c_str(),std::fstream::in|std::fstream::binary);
		if(!rfile)
		{
			std::cerr<<"Open txt failed!"<<std::endl;
			return -1;
		}
		//get length of file:
		rfile.seekg(0,std::ios::end);
		length = rfile.tellg();
		rfile.seekg(0,std::ios::beg);
		std::cout<<"length:"<<length<<std::endl;
		//allocate memory:
		buffer = new char[length];
		//read data as a block:
		rfile.read(buffer,length);
		rfile.close();

		//����
		zmq_msg_t send_msg1;
		zmq_msg_init_size(&send_msg1,length);
		memcpy(zmq_msg_data(&send_msg1),buffer,length);
		zmq_msg_send(&send_msg1,z_socket,0);
		zmq_msg_close(&send_msg1);

		delete[] buffer;

		//���ܲ���
		zmq_msg_t recv_msg1;
		zmq_msg_init(&recv_msg1);
		zmq_msg_recv(&recv_msg1,z_socket,0); 
		printf("�յ�Server�˻ش�:\t");
		std::cout<<(char*)zmq_msg_data(&recv_msg1)<<std::endl;
		zmq_msg_close(&recv_msg1);	

	}
	zmq_close(z_socket);
	zmq_term(context);

	return 0;
}