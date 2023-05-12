// GetTcpStatistics.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
//#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "iphlpapi.lib")
TCP_RESERVE_PORT_RANGE tcpDynamicPort;
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x)) 
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

/* Note: could also use malloc() and free() */

int main()
{
	PMIB_TCPSTATS pTCPStats;
	DWORD dwRetVal = 0;

	pTCPStats = (MIB_TCPSTATS*)MALLOC(sizeof(MIB_TCPSTATS));
	if (pTCPStats == NULL) {
		printf("Error allocating memory\n");
		return 1;
	}

	if ((dwRetVal = GetTcpStatistics(pTCPStats)) == NO_ERROR) {
		printf("\tActive Opens: %ld\n", pTCPStats->dwActiveOpens);
		printf("\tPassive Opens: %ld\n", pTCPStats->dwPassiveOpens);
		printf("\tSegments Recv: %ld\n", pTCPStats->dwInSegs);
		printf("\tSegments Xmit: %ld\n", pTCPStats->dwOutSegs);
		printf("\tTotal # Conxs: %ld\n", pTCPStats->dwNumConns);
	}
	else {
		printf("GetTcpStatistics failed with error: %ld\n", dwRetVal);

		LPVOID lpMsgBuf;
		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwRetVal,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR)&lpMsgBuf,
			0,
			NULL)) {
			printf("\tError: %s", lpMsgBuf);
		}
		LocalFree(lpMsgBuf);
	}

	if (pTCPStats)
		FREE(pTCPStats);
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
