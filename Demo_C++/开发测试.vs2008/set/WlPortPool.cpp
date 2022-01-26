/**************************************************************************
 *
 *        Copyright (C) 2022  Beijing Winicssec Technologies Co.,Ltd
 *        All rights reserved
 *
 *        Filename    :	WLPortPool.cpp
 *        Description : 动态端口已占用完毕的情况下，获取动态端口的函数接口实现文件
 *
 *        Created by  mingming.shi on Jan. 24th, 2022
 *
 *************************************************************************/
#include "StdAfx.h"
#include "WLPortPool.h"


#define STU_MEMBER_NUM		2
#define CMD_RESULT_BUF_SIZE 1024

#define CMD_QUERY_DYNAMIC_TCP "netsh int ipv4 show dynamicport tcp"

std::vector<STU_PORT_POOL> g_stuPortPool;

#if 0 /* 接口声明 */
#endif
/************************************************************************/
/* 其他函数声明 2022-1-24 14:33:28                                    */
/************************************************************************/
// 获取动态端口
int executeCMD(__out char *pChRet);
int getDynamicPortFromstr(__out STU_PORT_POOL& stuDynamicPort);

// 获取当前已占用端口
int getAllTcpConnectionsPort(__out std::set<UINT>& setRet);

// 获取剩余动态端口
int getFreeDynamicPort(__in STU_PORT_POOL stuDynamicPort, __in std::set<UINT> CurSysUsedPort, __out std::vector<STU_PORT_POOL>& stuFreeDynamicPort);
int getFreeStaticPort(__in STU_PORT_POOL stuDynamicPort, __in std::set<UINT> CurSysUsedPort, __out std::vector<STU_PORT_POOL>& stuFreeStaticPort);

// 更新端口
void updatePort();

// 创建端口池
int creatPortPool();

#if 0 /* 对外接口定义 */
#endif
/*
* @fn			getPort
* @brief		返回一个可用端口和其范围
* @param[in]	
* @param[out]   STU_PORT_POOL: 返回一个可用端口，如果调用失败，则修改stuPort中的BEnable为false
* @return		
*               
* @detail      	依次返回端口池中的端口和范围
* @author		mingming.shi
* @date			2022-1-24
*/
void WNT::getPort(__out STU_PORT_POOL& stuPort)
{
	int iRet = -1;
	if ( 0 == g_stuPortPool.size() ) // 当前端口池为空
	{
		iRet = creatPortPool();
	}
	else
	{
		stuPort = *g_stuPortPool.begin();
	}

	if (NO_ERROR == iRet)
	{
		stuPort = *g_stuPortPool.begin();
	}
	else
	{
		stuPort.bEnabl = false;
	}
}

/*
* @fn			SetPortIsAvailable
* @brief		设置当前端口的可用状态
* @param[in]	STU_PORT_POOL: 需要删除的端口
* @param[out]   
* @return		
*               
* @detail      	会根据当前的启动端口和范围在端口池中查找一致的并删除,然后会立即更新端口池
* @author		mingming.shi
* @date			2022-1-24
*/
void WNT::SetPortIsAvailable(__in STU_PORT_POOL stuPort)
{
	std::vector<STU_PORT_POOL>::iterator it = g_stuPortPool.begin();
	for (; it != g_stuPortPool.end(); it++)
	{
		if (it->nStartPort == stuPort.nStartPort &&
			it->nRange == stuPort.nRange && 
			stuPort.bEnabl == false) // 当前端口不可用
		{
			g_stuPortPool.erase(it);
		}
	}

	updatePort();
}

/*
* @fn			getFreeDynamicPortsNum
* @brief		获取剩余动态端口数量
* @param[in]    
* @param[out]	nNum：系统空闲动态端口数量
* @return		0 表示成功，其他表示失败
*               
* @detail      	
* @author		mingming.shi
* @date			2022-1-24
*/
int WNT::getFreeDynamicPortsNum(__out int& nNum)
{
	int iRet = 0;
	int nPortsNum = 0;

	STU_PORT_POOL stuDynamicPort;
	std::set<UINT> CurSysUsedPort;
	std::set<UINT>::iterator it;

	// 1 获取动态端口状态（起始地址和范围）
	iRet = getDynamicPortFromstr(stuDynamicPort);
	if ( NO_ERROR != iRet ) 
	{
		iRet = -1;
		goto _END_;
	}

	printf("动态端口启动地址：%d\n", stuDynamicPort.nStartPort);
	printf("动态端口范围：%d\n", stuDynamicPort.nRange);

	// 2 获取当前系统端口占用情况
	iRet = getAllTcpConnectionsPort(CurSysUsedPort);
	if ( NO_ERROR != iRet ) 
	{
		iRet = -2;
		goto _END_;
	}
	
	// 3 统计动态端口个数
	for (it = CurSysUsedPort.begin(); it != CurSysUsedPort.end(); it++)
	{
		if(true == stuDynamicPort.isContain(*it))
		{
			nPortsNum++;
			printf("%d:%d ",nPortsNum, *it);
			if (nPortsNum % 10 == 0)
			{
				printf("\n");
			}
			
		}
	}
	printf("被占用\n动态端口占用个数：%d\n", nPortsNum);

	nNum = stuDynamicPort.nRange - nPortsNum;
	iRet = 0;
_END_:
	if (CurSysUsedPort.size())
	{
		CurSysUsedPort.clear();
	}
	return iRet;
}

#if 0 /* 接口定义 */
#endif
/************************************************************************/
/* 接口实现                                                     */
/************************************************************************/



/*
* @fn			CreatPortPool
* @brief		创建端口资源池
* @param[in]    
* @param[out]	
* @return		0 创建成功；其他表示失败
*               
* @detail      	当系统动态端口使用情况不满足IEG时调用此接口获取静态端口资源池，保存到 g_stuPortPool 变量中
* @author		mingming.shi
* @date			2022-1-24
*/
int creatPortPool()
{
	int iRet = 0;
	std::vector<STU_PORT_POOL> stuPortPool;			// 保存接口返回的端口范围
	std::vector<STU_PORT_POOL> stuFreeDynamicPort;	// 保存未被使用端口范围
	STU_PORT_POOL	stuDynamicPort;
	std::set<UINT>	CurSysUsedPort;

	// 1 获取动态端口状态（起始地址和范围）
	iRet = getDynamicPortFromstr(stuDynamicPort);
	if(NO_ERROR != iRet)
	{
		iRet = -1;
		goto _END_;
	}

	// 2 获取端口占用情况
	iRet = getAllTcpConnectionsPort(CurSysUsedPort);
	if(NO_ERROR != iRet)
	{
		iRet = -2;
		goto _END_;
	}

	// 3 从默认地址开始查找可用端口范围
	iRet = getFreeStaticPort(stuDynamicPort, CurSysUsedPort, stuFreeDynamicPort);
	if(NO_ERROR == iRet)
	{
		g_stuPortPool = stuFreeDynamicPort;
	}

	iRet = 0;
_END_:
	return iRet;
}

/************************************************************************/
/* 函数实现  2022-1-24 14:33:41                                        */
/************************************************************************/
/*
* @fn			ExecuteCMD
* @brief		执行查询TCP动态端口命令，获取系统动态端口范围
* @param[in]    
* @param[out]	pChRet: 命令行执行后的输出字符串
* @return		0 表示成功；其他 失败
*               
* @detail      	
* @author		mingming.shi
* @date			2022-1-24
*/
static int executeCMD(__out char *pChRet)
{
	int iRet = -1;
	char buf_ps[CMD_RESULT_BUF_SIZE];
	char pChBuf[CMD_RESULT_BUF_SIZE] = { 0 };
	FILE *ptr;

	strcpy_s(pChBuf, _countof(CMD_QUERY_DYNAMIC_TCP) ,CMD_QUERY_DYNAMIC_TCP);

	if ( NULL != (ptr = _popen(pChBuf, "r")) )
	{
		while (NULL != fgets(buf_ps, sizeof(buf_ps), ptr) )
		{
			strcat_s(pChRet, _countof(buf_ps), buf_ps);
			if (strlen(pChRet) > CMD_RESULT_BUF_SIZE)
			{
				iRet = -2;
				break;
			}
		}
		_pclose(ptr);
		ptr = NULL;
		iRet = 0;  // 处理成功
	}
	else
	{
	
		iRet = -1; // 处理失败
	}

	return iRet;
}

/*
* @fn			getDynamicPortFromstr
* @brief		从cmd命令的输出中获取动态端口的启动地址和范围
* @param[in]    
* @param[out]	stuDynamicPort: 动态端口的起始地址和范围
* @return		0 表示成功，其他表示失败
*               
* @detail      	该动态端口为TCP端口
* @author		mingming.shi
* @date			2022-1-24
*/
int getDynamicPortFromstr(__out STU_PORT_POOL& stuDynamicPort)
{
	int iRet = 0;
	std::string str = "";
	std::vector<unsigned int> vecNumFromStr;

	// 1 执行CMD获取其输出，格式如下
	/*
		协议 tcp 动态端口范围
		---------------------------------
		启动端口        : 49152
		端口数          : 16384
	*/
	char chRet[CMD_RESULT_BUF_SIZE] = { 0 };
	if ( 0 != executeCMD(chRet) )
	{
		iRet = -1;
		goto _END_;
	}

	str = chRet;

	if(str.empty())
	{
		iRet = -2;
		goto _END_;
	}

	// 2 获取CMD输出中的数字

	size_t nLen = str.length();

	for (size_t i = 0; i < nLen; i++) {
		int CurNum = 0;
		bool flag = false;
		while ( !(str[i] >= '0' && str[i] <= '9') && i < nLen ) 
		{
			i++;
		}

		while ( (str[i] >= '0' && str[i] <= '9') && i < nLen ) 
		{
			flag = true;
			CurNum = CurNum * 10 + (str[i] - '0');
			i++;
		}

		if (flag)
		{
			vecNumFromStr.push_back(CurNum);
		}
	}
	
	if (STU_MEMBER_NUM != vecNumFromStr.size())
	{
		iRet = -3;
		goto _END_;
	}
	stuDynamicPort.nStartPort	= vecNumFromStr[0];
	stuDynamicPort.nRange		= vecNumFromStr[1];
	
	iRet = 0;
_END_:
	return iRet; //集合大小就是不同整数数量
}

/*
* @fn			getAllTcpConnectionsPort
* @brief		获取已连接端口数
* @param[in]    
* @param[out]	ret：已连接端口的set容器
* @return		0 表示执行成功，其他表示失败
*               
* @detail      	
* @author		mingming.shi
* @date			2022-1-24
*/
int getAllTcpConnectionsPort(__out std::set<UINT>& setRet)
{
	int iRet                = 0;
	ULONG size              = 0;
	PMIB_TCPTABLE pTcpTable = NULL;
	DWORD dwSize            = 0;
	DWORD dwRetVal			= 0;

	GetTcpTable(NULL, &size, TRUE);

	if ( ERROR_INSUFFICIENT_BUFFER == (dwRetVal = GetTcpTable(pTcpTable, &dwSize, TRUE)) ) 
	{
		free(pTcpTable);
		pTcpTable = (MIB_TCPTABLE *)malloc(dwSize);
		if (pTcpTable == NULL) 
		{	
			iRet = -1;
			goto _END_;
		}
	}

	if ( NO_ERROR == (dwRetVal = GetTcpTable(pTcpTable, &dwSize, TRUE)) ) 
	{
		for (int i = 0; i < (int)pTcpTable->dwNumEntries; i++) 
		{
			setRet.insert(pTcpTable->table[i].dwLocalPort);
		}
	}
	else 
	{
		free(pTcpTable);
		iRet = -2;
		goto _END_;
	}

_END_:
	if (pTcpTable != NULL) 
	{
		free(pTcpTable);
		pTcpTable = NULL;
	}

	return iRet;
}

/*
* @fn			getFreeDynamicPort
* @brief		获取未被占用的动态端口段
* @param[in]    stuDynamicPort：系统当前开放的动态端口范围
				CurSysUsedPort：系统当前已使用端口
* @param[out]	stuFreeStaticPort: 从默认端口开始获取的端口范围

* @return		
*               
* @detail      	
* @author		mingming.shi
* @date			2022-1-24
*/
int getFreeDynamicPort(__in STU_PORT_POOL stuDynamicPort, __in std::set<UINT> CurSysUsedPort, __out std::vector<STU_PORT_POOL>& stuFreeDynamicPort)
{
	int iRet = 0;
	UINT nStart = stuDynamicPort.nStartPort;
	UINT nEnd	= stuDynamicPort.nStartPort + stuDynamicPort.nRange;

	for (UINT nPort = nStart; nPort != nEnd; ++nPort)
	{
		STU_PORT_POOL stuTempPort;
		if ( CurSysUsedPort.end() !=  CurSysUsedPort.find(nPort) ) // 找到了未使用端口
		{
			int nRange = 0;

			// 遍历当前启动端口的端口范围
			while ( CurSysUsedPort.end() !=  CurSysUsedPort.find( nPort + nRange) )
			{
				nRange++;
			}

			stuTempPort.nStartPort	= nPort;
			stuTempPort.nRange		= nRange;
		}

		stuFreeDynamicPort.push_back(stuTempPort);
		if (stuFreeDynamicPort.size() > MAX_PORT_NUM)
		{
			iRet = -1;
			goto _END_;
		}
	}
	
_END_:
	return iRet;
}

/*
* @fn			getFreeStaticPort
* @brief		从默认端口开始获取未被系统使用的静态端口和范围
* @param[in]    stuDynamicPort：系统当前开放的动态端口范围
				CurSysUsedPort：系统当前已使用端口
* @param[out]	stuFreeStaticPort: 返回
* @return		0 表示成功，其他表示失败
*               
* @detail      	
* @author		mingming.shi
* @date			2022-1-24
*/
int getFreeStaticPort(__in STU_PORT_POOL stuDynamicPort, __in std::set<UINT> CurSysUsedPort, __out std::vector<STU_PORT_POOL>& stuFreeStaticPort)
{
	int iRet = -1;
	UINT nStart = DEFAULT_START_PORT;
	UINT nEnd	= DEFAULT_STOP_PORT;

	for (UINT nPort = nStart; nPort != nEnd; ++nPort)
	{
		STU_PORT_POOL stuTempPort;

		if ( CurSysUsedPort.end() == CurSysUsedPort.find(nPort) && false == stuDynamicPort.isContain(nPort) ) // 找到了未使用端口，并且不再动态端口范围内
		{
			int nRange = 0;

			// 遍历当前启动端口的端口范围
			do
			{
				nRange++;
			}while ( 
				CurSysUsedPort.end() ==  CurSysUsedPort.find(nPort + nRange) && // 系统未使用
				false == stuDynamicPort.isContain(nPort + nRange) && // 不在动态端口范围
				nRange <= DEFAULT_PORT_RANGE); // 范围小于默认端口范围

			stuTempPort.nStartPort	= nPort;
			stuTempPort.nRange		= nRange;
			stuTempPort.bEnabl		= true;

			// 更新启动端口并检查合法性
			nPort += nRange; 
			nPort = nPort >= DEFAULT_STOP_PORT ? DEFAULT_START_PORT : nPort;
		}

		stuFreeStaticPort.push_back(stuTempPort);
		if (stuFreeStaticPort.size() > MAX_PORT_NUM) // 只是数组满了而已，仍然返回成功
		{
			iRet = 0; 
			goto _END_;
		}
	}

	iRet = 0;
_END_:
	return iRet;
}

void updatePort()
{
	int iRet = -1;
	UINT nStart = DEFAULT_START_PORT;
	UINT nEnd	= DEFAULT_STOP_PORT;
	STU_PORT_POOL	stuDynamicPort;
	std::set<UINT>	CurSysUsedPort;

	// 1 获取动态端口状态（起始地址和范围）
	getDynamicPortFromstr(stuDynamicPort);

	// 2 获取端口占用情况
	getAllTcpConnectionsPort(CurSysUsedPort);

	// 3 更新启动端口，从端口池最后一个端口开始
	STU_PORT_POOL stuTempPort = g_stuPortPool[g_stuPortPool.size() - 1];
	nStart = stuTempPort.nStartPort + stuTempPort.nRange;

	for (UINT nPort = nStart; nPort <= DEFAULT_STOP_PORT; ++nPort)
	{
		STU_PORT_POOL stuTempPort;

		if ( CurSysUsedPort.end() != CurSysUsedPort.find(nPort) && false == stuDynamicPort.isContain(nPort) ) // 找到了未使用端口，并且不再动态端口范围内
		{
			int nRange = 0;

			// 遍历当前启动端口的端口范围
			while ( CurSysUsedPort.end() !=  CurSysUsedPort.find(nPort + nRange) && false == stuDynamicPort.isContain(nPort + nRange) )
			{
				nRange++;
				if (nRange >= DEFAULT_PORT_RANGE)
				{
					break;
				}
			}

		
			stuTempPort.nStartPort	= nPort;
			stuTempPort.nRange		= nRange;
			stuTempPort.bEnabl		= true;

			// 更新启动端口并检查合法性
			nPort += nRange; 
			nPort = nPort >= DEFAULT_STOP_PORT ? DEFAULT_START_PORT : nPort;
		}

		if (true == stuTempPort.isLegal())
		{
			g_stuPortPool.push_back(stuTempPort);
		}
		
		if (g_stuPortPool.size() > MAX_PORT_NUM)
		{
			break;
		}
	}
}
