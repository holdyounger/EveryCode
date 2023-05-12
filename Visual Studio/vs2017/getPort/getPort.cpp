// getPort.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
// Need to link with Iphlpapi.lib and Ws2_32.lib
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <algorithm>

#include <set>
#include <vector>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
using namespace std;

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

#define _WINSOCK_DEPRECATED_NO_WARNINGS

/* Note: could also use malloc() and free() */
vector<DWORD> g_Ports;

int main()
{

	int iRet = 0;
	int nNum = 0; // TCP连接的数目

	PMIB_TCPTABLE_OWNER_PID pTcpTable(NULL);
	DWORD dwSize(0);
	GetExtendedTcpTable(pTcpTable, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);
	pTcpTable = (MIB_TCPTABLE_OWNER_PID *)new char[dwSize];//重新分配缓冲区


	if (NO_ERROR != GetExtendedTcpTable(pTcpTable, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0))
	{
		delete pTcpTable;
		pTcpTable = NULL;
		{
			iRet = -1;
			goto _END_;
		}
	}

	// TCP连接的数目
	nNum = (int)pTcpTable->dwNumEntries;

	for (int i = 0; i < nNum; i++)
	{
		g_Ports.push_back(htons(pTcpTable->table[i].dwLocalPort));
	}

_END_:
	if (pTcpTable != NULL)
	{
		free(pTcpTable);
		pTcpTable = NULL;
	}

	std::sort(g_Ports.begin(), g_Ports.end());

	if (pTcpTable != NULL) {
		FREE(pTcpTable);
		pTcpTable = NULL;
	}

	int ii = 0;
	bool flag = false;
	for (auto it : g_Ports)
	{
		if (it >= 49152 && flag == false)
		{
			flag = true;
			cout << "\n------------" << endl;
		}
		cout << it << " ";
		if (ii++ % 10 == 0)
		{
			cout << endl;
		}
	}

	cout << "num of all ports" << g_Ports.size() << endl;
	return 0;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
