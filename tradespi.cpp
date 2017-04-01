#include<iostream>
#include "tradespi.h"
#include <ThostFtdcMdApi.h>
#include <ThostFtdcTraderApi.h>
#include <ThostFtdcUserApiDataType.h>
#include <ThostFtdcUserApiStruct.h>
#include <time.h>
#include <thread>
using namespace std;
string tradingDate;
//模拟账号
const string USER_ID = "086624";
const string PASS = "zhouye0804";
const string BROKER = "9999";
TThostFtdcDirectionType	DIRECTION = THOST_FTDC_D_Sell;	// 买卖方向,表示买
TThostFtdcPriceType	LIMIT_PRICE = 2380;				// 价格
TThostFtdcFrontIDType	FRONT_ID;	//前置编号
TThostFtdcSessionIDType	SESSION_ID;	//会话编号
TThostFtdcOrderRefType	ORDER_REF;	//报单引用
//构造函数
TradeSpi::TradeSpi(CThostFtdcTraderApi *tdapi){
    this->tdapi = tdapi;
}
void TradeSpi::OnFrontDisconnected(int nReason){
    cout<<"OnFrontDisconnected"<<endl;
    cout<<nReason<<endl;
}
//当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
void TradeSpi::OnFrontConnected(){
    cout << "Td连接成功" << endl;
    cout << "请求登陆\n";
    loginField = new CThostFtdcReqUserLoginField();
    strcpy(loginField->BrokerID, BROKER.c_str());
    strcpy(loginField->UserID, USER_ID.c_str());
    strcpy(loginField->Password, PASS.c_str());
    tdapi->ReqUserLogin(loginField, 3);
}

///登录请求响应
void TradeSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
    cout << "登录请求回调OnRspUserLogin" << endl;
    cout<<"请求编号："<<nRequestID<<endl;
    cout <<"ErrorID:"<< pRspInfo->ErrorID << " " << pRspInfo->ErrorMsg << endl;
    cout << "前置编号:" << pRspUserLogin->FrontID << endl//客户端连接到的前置机编号
        << "会话编号" << pRspUserLogin->SessionID << endl//客户端连接到前置机的连接会话编号
        << "最大报单引用:" << pRspUserLogin->MaxOrderRef << endl//每一笔报单都有一个唯一的不重复的编号
        << "上期所时间：" << pRspUserLogin->SHFETime << endl
        << "大商所时间：" << pRspUserLogin->DCETime << endl
        << "郑商所时间：" << pRspUserLogin->CZCETime << endl
        << "中金所时间：" << pRspUserLogin->FFEXTime << endl
        << "交易日：" << tdapi->GetTradingDay() << endl;
    FRONT_ID=pRspUserLogin->FrontID;
    SESSION_ID=pRspUserLogin->SessionID;
    int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
    iNextOrderRef++;
    sprintf(ORDER_REF, "%d", iNextOrderRef);
   // tradingDate = tdapi->GetTradingDay();//设置交易日期
    cout << "--------------------------------------------" << endl << endl;
    //请求查询结算信息确认
    ReqSettlementInfoConfirm();
}


void TradeSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "--->>> " << "OnRspSettlementInfoConfirm" << endl;
    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {

        //请求查询合约
       //ReqQryInstrument();
        //请求查询投资者持仓
       //ReqQryInvestorPosition();
        //查询用户资金账户
       //ReqQryTradingAccount();
        //报单录入请求
        //ReqOrderInsert();
        //请求查询报单
        // RspQryOrder();
        //请求查询成交
        //RspQryTrade();
        //请求查询行情
        RspQryDepthMarketData();
    }
}
void TradeSpi::ReqOrderInsert()
{
    CThostFtdcInputOrderField ord;
    memset(&ord, 0, sizeof(ord));
    //经纪公司代码
    strcpy(ord.BrokerID, "9999");
    //投资者代码
    strcpy(ord.InvestorID, "086624");
    // 合约代码
    strcpy(ord.InstrumentID, "rb1705");
    ///报单引用
    strcpy(ord.OrderRef, ORDER_REF);
    // 用户代码
    strcpy(ord.UserID, "086624");
    // 报单价格条件
    ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
    // 买卖方向
    ord.Direction= THOST_FTDC_D_Buy;
    // 组合开平标志
    strcpy(ord.CombOffsetFlag, "0");
    // 组合投机套保标志
    strcpy(ord.CombHedgeFlag, "1");
    // 价格
    ord.LimitPrice= 3379;
    // 数量
    ord.VolumeTotalOriginal= 10;
    // 有效期类型
    ord.TimeCondition = THOST_FTDC_TC_GFD;
    // GTD日期
    strcpy(ord.GTDDate,"");
    // 成交量类型
    ord.VolumeCondition= THOST_FTDC_VC_AV;
    // 最小成交量
    ord.MinVolume= 0;
    // 触发条件
    ord.ContingentCondition= THOST_FTDC_CC_Immediately;
    // 止损价
    ord.StopPrice= 0;
    // 强平原因
    ord.ForceCloseReason= THOST_FTDC_FCC_NotForceClose;
    // 自动挂起标志
    //ord.IsAutoSuspend =0;
    int iResult = tdapi->ReqOrderInsert(&ord, 1234);
    cerr << "--->>> 报单录入请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}
//报单录入请求响应
void TradeSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,
               CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cout<<"nRequest:"<<nRequestID<<endl;
    cout<<"OnRspOrderInsert "<<endl;
    cout<<pRspInfo->ErrorID<<" "<<pRspInfo->ErrorMsg<<endl;
}
//报单回报
void TradeSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
    if(IsMyOrder(pOrder))
    {
        cout<<"OnRtnOrder"<<endl;
        cout<<"报单状态："<<pOrder->OrderStatus<<endl;
        cout<<"报单类型："<<pOrder->OrderType<<endl;
        cout<<"状态信息："<<pOrder->StatusMsg<<endl;
        cout<<pOrder->Direction<<endl;
    }

}
//成交回报
void TradeSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
   cout<<"OnRtnTrade"<<endl;
   cout<<"报单引用:"<<pTrade->OrderRef<<endl;
   cout<<"成交编号:"<<pTrade->TradeID<<endl;
   cout<<"报单编号:"<<pTrade->OrderSysID<<endl;
   cout<<"成交编号:"<<pTrade->TradeID<<endl;
   cout<<"交易角色:"<<pTrade->TradingRole<<endl;
   cout<<"开平标志:"<<pTrade->OffsetFlag<<endl;
   cout<<"价格:"<<pTrade->Price<<endl;
   cout<<"数量:"<<pTrade->Volume<<endl;
   cout<<"价格:"<<pTrade->Price<<endl;
   cout<<"成交时期:"<<pTrade->TradeDate<<endl;
   cout<<"成交时间:"<<pTrade->TradeTime<<endl;
   cout<<"成交类型:"<<pTrade->TradeType<<endl;
   cout<<"成交价来源:"<<pTrade->PriceSource<<endl;
   cout<<"本地报单编号:"<<pTrade->OrderLocalID<<endl;
   cout<<"结算会员编号:"<<pTrade->ClearingPartID<<endl;
   cout<<"业务单元:"<<pTrade->BusinessUnit<<endl;
   cout<<"结算编号:"<<pTrade->SettlementID<<endl;
   cout<<"成交来源:"<<pTrade->TradeSource<<endl;

}
//请求查询合约
void TradeSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "--->>> " << "OnRspQryInstrument" << endl;
    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        ///请求查询合约
        //ReqQryTradingAccount();
            cout<<"合约代码:"<<pInstrument->InstrumentID<<endl;
            cout<<"交易所代码:"<<pInstrument->ExchangeID<<endl;
            cout<<"产品代码："<<pInstrument->ProductID<<endl;
            cout<<"交割年份："<<pInstrument->DeliveryYear<<endl;
            cout<<"交割月:"<<pInstrument->DeliveryMonth<<endl;
            cout<<"开始交割日："<<pInstrument->StartDelivDate<<endl;
            cout<<"结束交割日："<<pInstrument->EndDelivDate<<endl;
            cout<<"市价单最大下单量："<<pInstrument->MaxMarketOrderVolume<<endl;
            cout<<"市价单最小下单量："<<pInstrument->MinMarketOrderVolume<<endl;
            cout<<"限价单最大下单量:"<<pInstrument->MaxLimitOrderVolume<<endl;
            cout<<"限价单最小下单量:"<<pInstrument->MinLimitOrderVolume<<endl;
            cout<<"最小变动价位:"<<pInstrument->PriceTick<<endl;
            cout<<"合约数量乘数:"<<pInstrument->VolumeMultiple<<endl;
            cout<<"创建日:"<<pInstrument->CreateDate<<endl;
            cout<<"上市日:"<<pInstrument->OpenDate<<endl;
            cout<<"到期日:"<<pInstrument->ExpireDate<<endl;

    }
}








//请求查询行情
void TradeSpi::RspQryDepthMarketData()
{
    CThostFtdcQryDepthMarketDataField * req=new CThostFtdcQryDepthMarketDataField();
    strcpy(req->InstrumentID,"rb1705");
    int iResult=tdapi->ReqQryDepthMarketData(req,1024);
    cerr << "--->>> 请求查询行情: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

//请求查询行情响应
void TradeSpi::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
    cout << "OnRspQryDepthMarketData" << endl;
    cout << nRequestID << endl;
    if (pDepthMarketData != nullptr){
        cout << ">>>>>>>>>>>>>>>>>>行情数据>>>>>>>>>>>>>>>>>>" << endl;
        cout << "交易日:" << pDepthMarketData->TradingDay << endl
            << "合约代码:" << pDepthMarketData->InstrumentID << endl
            << "最新价:" << pDepthMarketData->LastPrice << endl
            << "最高价:" << pDepthMarketData->HighestPrice << endl
            << "最低价:" << pDepthMarketData->LowestPrice << endl;
        cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
    }
}
//请求查询报单
void TradeSpi::RspQryOrder()
{
    CThostFtdcQryOrderField *pOrder = new CThostFtdcQryOrderField();
    strcpy(pOrder->BrokerID, BROKER.c_str());
    strcpy(pOrder->InvestorID, USER_ID.c_str());
    strcpy(pOrder->InstrumentID, "rb1705");
//    strcpy(pOrder->InsertTimeStart, "20140101");
//    strcpy(pOrder->InsertTimeEnd, "20170331");
    int iResult=tdapi->ReqQryOrder(pOrder,1111);
    cerr << "--->>> 请求查询报单: " << ((iResult == 0) ? "成功" : "失败") << endl;
}
//请求查询报单响应
void TradeSpi::OnRspQryOrder(CThostFtdcOrderField *pOrder,
                             CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cout<<"OnRspQryOrder"<<endl;
    //cout<<pRspInfo->ErrorID<<":"<<pRspInfo->ErrorMsg;
    if(pOrder!=nullptr)
    {

        cout<<"报单状态："<<pOrder->OrderStatus<<endl;
        cout<<"报单类型："<<pOrder->OrderType<<endl;
        cout<<"状态信息："<<pOrder->StatusMsg<<endl;
    }


}
//请求查询成交
void TradeSpi::RspQryTrade()
{
    CThostFtdcQryTradeField *pTrade = new CThostFtdcQryTradeField();
    strcpy(pTrade->BrokerID, BROKER.c_str());
    strcpy(pTrade->InvestorID, USER_ID.c_str());
    strcpy(pTrade->InstrumentID, "rb1705");
//    strcpy(pTrade->TradeTimeStart, "20140101");
//    strcpy(pTrade->TradeTimeEnd, "20170331");
    int iResult = tdapi->ReqQryTrade(pTrade, 1110);
    cerr << "--->>> 请求查询成交: " << ((iResult == 0) ? "成功" : "失败") << endl;
}
//请求查询成交响应
void TradeSpi::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo,
          int nRequestID, bool bIsLast)
{
    cout<<"OnRspQryTrade"<<endl;
    //cout<<pRspInfo->ErrorID<<":"<<pRspInfo->ErrorMsg<<endl;
    if (pTrade!=nullptr){

        cout<<"报单引用:"<<pTrade->OrderRef<<endl;
        cout<<"成交编号:"<<pTrade->TradeID<<endl;
        cout<<"报单编号:"<<pTrade->OrderSysID<<endl;
        cout<<"成交编号:"<<pTrade->TradeID<<endl;
        cout<<"交易角色:"<<pTrade->TradingRole<<endl;
        cout<<"开平标志:"<<pTrade->OffsetFlag<<endl;
        cout<<"价格:"<<pTrade->Price<<endl;
        cout<<"数量:"<<pTrade->Volume<<endl;
        cout<<"价格:"<<pTrade->Price<<endl;
        cout<<"成交时期:"<<pTrade->TradeDate<<endl;
        cout<<"成交时间:"<<pTrade->TradeTime<<endl;
        cout<<"成交类型:"<<pTrade->TradeType<<endl;
        cout<<"成交价来源:"<<pTrade->PriceSource<<endl;
        cout<<"本地报单编号:"<<pTrade->OrderLocalID<<endl;
        cout<<"结算会员编号:"<<pTrade->ClearingPartID<<endl;
        cout<<"业务单元:"<<pTrade->BusinessUnit<<endl;
        cout<<"结算编号:"<<pTrade->SettlementID<<endl;
        cout<<"成交来源:"<<pTrade->TradeSource<<endl;
    }


}
//查询资金帐户
void TradeSpi::ReqQryTradingAccount()
{
    CThostFtdcQryTradingAccountField req;
        memset(&req, 0, sizeof(req));
        strcpy(req.BrokerID, BROKER.c_str());
        strcpy(req.InvestorID, USER_ID.c_str());
        int iResult = tdapi->ReqQryTradingAccount(&req, 2000);
        cerr << "--->>> 请求查询资金账户: " << ((iResult == 0) ? "成功" : "失败") << endl;
}
//查询资金帐户响应
void TradeSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
    cout << "OnRspQryTradingAccount\n";
    if (pRspInfo == nullptr || pRspInfo->ErrorID == 0){
        cout << "nRequestID: " << nRequestID << endl;
        cout << "可用资金" << pTradingAccount->Available << endl;
    }

}

//请求查询结算确认信息
void TradeSpi::ReqSettlementInfoConfirm()
{

    CThostFtdcSettlementInfoConfirmField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, BROKER.c_str());
    strcpy(req.InvestorID,  USER_ID.c_str());
    int iResult = tdapi->ReqSettlementInfoConfirm(&req, 999);
    cerr << "--->>> 投资者结算结果确认: " << ((iResult == 0) ? "成功" : "失败") << endl;
}
//查询合约
void TradeSpi::ReqQryInstrument()
{

      CThostFtdcQryInstrumentField req;
      memset(&req, 0, sizeof(req));
      strcpy(req.InstrumentID, "rb1705");
      int iResult = tdapi->ReqQryInstrument(&req, 1001);
      cerr << "--->>> 请求查询合约: " << ((iResult == 0) ? "成功" : "失败") << endl;

}
// 请求查询投资者持仓
void TradeSpi::ReqQryInvestorPosition()
{

    CThostFtdcQryInvestorPositionField req;
        memset(&req, 0, sizeof(req));
        strcpy(req.BrokerID, BROKER.c_str());
        strcpy(req.InvestorID, USER_ID.c_str());
        strcpy(req.InstrumentID, "rb1705");
        int iResult = tdapi->ReqQryInvestorPosition(&req, 1999);
        cerr << "--->>> 请求查询投资者持仓: " << ((iResult == 0) ? "成功" : "失败") << endl;
}
//请求查询投资者持仓响应
void TradeSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
    cout << "OnRspQryInvestorPosition  ID: " << nRequestID << endl;
    //cout << "错误代码：" << pRspInfo->ErrorID << "错误信息:" << pRspInfo->ErrorMsg;
    //auto test=pInvestorPosition->BrokerID ;
    cout << "持仓多空方向:"<< pInvestorPosition->PosiDirection<< endl;
    cout << "持仓日期:"<<pInvestorPosition->PositionDate<<endl;
    cout << "上日持仓:"<<pInvestorPosition->YdPosition<<endl;
    cout << "今日持仓:"<<pInvestorPosition->Position<<endl;
    cout << "开仓量:"<<pInvestorPosition->OpenVolume<<endl;
    cout << "平仓量:"<<pInvestorPosition->CloseVolume<<endl;
    cout << "开仓金额:"<<pInvestorPosition->OpenAmount<<endl;
    cout << "平仓金额:"<<pInvestorPosition->CloseAmount<<endl;
    cout << "持仓成本:"<<pInvestorPosition->PositionCost<<endl;
    cout << "平仓盈亏:"<<pInvestorPosition->CloseProfit<<endl;
    cout << "持仓盈亏:"<<pInvestorPosition->PositionProfit<<endl;
    cout << "交易日:"<<pInvestorPosition->TradingDay<<endl;

    if (bIsLast){
        cout << "last\n";
    }
}

//判断是否收到错误相应
bool TradeSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
    // 如果ErrorID != 0, 说明收到了错误的响应
    bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
    if (bResult)
        cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
    return bResult;
}
//过滤订单
bool TradeSpi::IsMyOrder(CThostFtdcOrderField *pOrder)
{
//    cout<<pOrder->FrontID<<endl;
//    cout<<pOrder->SessionID<<endl;
    return ((pOrder->FrontID == FRONT_ID) &&
            (pOrder->SessionID == SESSION_ID) &&
            (strcmp(pOrder->OrderRef, ORDER_REF) == 0));
}

