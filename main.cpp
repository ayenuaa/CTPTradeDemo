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

int main(int argc, char *argv[])
{

        char front_address[]="tcp://180.168.146.187:10030";
        auto tdapi = CThostFtdcTraderApi::CreateFtdcTraderApi();
        TradeSpi *tdspi = new TradeSpi(tdapi);
        //注册事件处理对象
        tdapi->RegisterSpi(tdspi);
        //订阅共有流和私有流
        tdapi->SubscribePublicTopic(THOST_TERT_RESTART);
        tdapi->SubscribePrivateTopic(THOST_TERT_RESTART);
        //注册前置机

        tdapi->RegisterFront(front_address);	//模拟
        std::cout << "1\n";
        //和前置机连接
        tdapi->Init();
        std::cout << "2\n";
        tdapi->Join();
        std::cout << "3\n";
    return 0;
}
