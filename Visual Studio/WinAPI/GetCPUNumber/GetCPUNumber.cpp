// GetCPUNumber.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>
using namespace std;
unsigned core_count()  
{  
	unsigned count = 1; // 至少一个  
	SYSTEM_INFO si;  
	GetSystemInfo(&si);  
	count = si.dwNumberOfProcessors;  
	return count;  
}  

typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
 
int get_cpu_info(uint16_t *user, uint16_t *nice, uint16_t *idle)
{
    FILE *fd;
    char buff[256];
            char name[32];
    uint32_t system;
 
    fd = fopen("/proc/stat", "r");
    fgets(buff, sizeof(buff), fd);
 
    sscanf(buff, "%s %u %u %u %u", name, user,
           nice, &system, idle);
    fclose(fd);
 
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	unsigned sz = core_count();
	cout << "The Number of CPU is " << sz << '.' << endl;
	system("pause");
	return 0;
}

