#include <iostream>
#include <ThostFtdcMdApi.h>
#include <ThostFtdcTraderApi.h>
#include <ThostFtdcUserApiDataType.h>
#include <ThostFtdcUserApiStruct.h>
#include "tradespi.h"
#include <chrono>
#include <thread>
#pragma comment(lib,"thostmduserapi.lib")
#pragma comment(lib,"thosttraderapi.lib")
using namespace std;


// MdApi对象
CThostFtdcMdApi *pMdApi;

// UserApi对象
CThostFtdcTraderApi *pUserApi;

char  FRONT_ADDR_1A[] = "tcp://180.168.146.187:10000";		// 正常环境模拟实盘交易地址
char  FRONT_ADDR_1B[] = "tcp://180.168.146.187:10010";		// 正常环境实盘行情地址

char  FRONT_ADDR_2A[] = "tcp://180.168.146.187:10030";		// 7*24小时模拟交易地址
char  FRONT_ADDR_2B[] = "tcp://180.168.146.187:10031";		// 7*24小时模拟行情地址

///模拟盘
TThostFtdcBrokerIDType	 BROKER_ID   = "9999";				// 经纪公司代码
TThostFtdcInvestorIDType INVESTOR_ID = "086624";					// 注意这里输入你自己的投资者代码
TThostFtdcPasswordType   PASSWORD    = "zhouye0804";					// 注意这里输入你自己的用户密码

TThostFtdcInstrumentIDType INSTRUMENT_ID = "rb1705";		// 交易合约代码
TThostFtdcDirectionType	   DIRECTION = THOST_FTDC_D_Sell;	// 交易买卖方向
TThostFtdcOffsetFlagType   MARKETState;						// 开
TThostFtdcPriceType	       LIMIT_PRICE = 3407;				// 交易价格

TThostFtdcDateExprType	   TradingDay;


// 请求编号
extern int iRequestID;

char ppInstrumentID[] = {"rb1705"};			// 行情订阅列表，注意，这个合约ID会过时的，注意与时俱进修改
int iInstrumentID = 1;							// 行情订阅数量

// 请求编号
int iRequestID = 0;

void main(void)
{
    // 交易初始化 pUserApi
    pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();			// 创建UserApi
    TradeSpi* pUserSpi = new TradeSpi();
    pUserApi->RegisterSpi((CThostFtdcTraderSpi*)pUserSpi);			// 注册事件类
    pUserApi->SubscribePublicTopic(THOST_TERT_RESUME);				// 注册公有流
    pUserApi->SubscribePrivateTopic(THOST_TERT_RESUME);			// 注册私有流
   // pUserApi->RegisterFront(FRONT_ADDR_1A);							// connect
    pUserApi->RegisterFront(FRONT_ADDR_2A);							// connect
    pUserApi->Init();

   

    pUserApi->Join();
}

