#include "tradespi.h"
#include <Windows.h>
#include <iostream>
#include <ThostFtdcMdApi.h>
#include <ThostFtdcTraderApi.h>
#include <ThostFtdcUserApiDataType.h>
#include <ThostFtdcUserApiStruct.h>
#include "tradespi.h"
#include <chrono>
#include <thread>
using namespace std;
// USER_API参数
extern CThostFtdcTraderApi* pUserApi;

// 配置参数
extern char FRONT_ADDR[];		// 前置地址
extern char BROKER_ID[];		// 经纪公司代码
extern char INVESTOR_ID[];		// 投资者代码
extern char PASSWORD[];			// 用户密码
extern char INSTRUMENT_ID[];	// 合约代码
extern TThostFtdcPriceType	LIMIT_PRICE;	// 价格
extern TThostFtdcDirectionType	DIRECTION;	// 买卖方向

// 请求编号
extern int iRequestID;

// 会话参数
TThostFtdcFrontIDType	FRONT_ID;	//前置编号
TThostFtdcSessionIDType	SESSION_ID;	//会话编号
TThostFtdcOrderRefType	ORDER_REF;	//报单引用

void TradeSpi::OnFrontConnected()
{
    cerr << "============================================================" << endl;
    cerr << "= 交易程序初始化" << endl;
    cerr << "============================================================" << endl;

    ///发送用户登录请求
    ReqUserLogin();
}

///发送用户登录请求
void TradeSpi::ReqUserLogin()
{
    cerr << "---> 用户登录中..." << endl;

    CThostFtdcReqUserLoginField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, BROKER_ID);
    strcpy(req.UserID, INVESTOR_ID);
    strcpy(req.Password, PASSWORD);
    int iResult = pUserApi->ReqUserLogin(&req, ++iRequestID);
    if (iResult != 0) {
        cerr << "---> 发送用户登录请求失败" << endl;
    }
}

///发送用户登录请求,回调函数
void TradeSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        cerr << "---> 登录成功（FrontID:" << pRspUserLogin->FrontID << " SessionID:" << pRspUserLogin->SessionID << " MaxOrderRef:" << pRspUserLogin->MaxOrderRef << "）" << endl;

        Sleep(1000);

        // 保存会话参数
        FRONT_ID   = pRspUserLogin->FrontID;
        SESSION_ID = pRspUserLogin->SessionID;
        int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
        iNextOrderRef++;
        sprintf(ORDER_REF, "%d", iNextOrderRef);
        ///获取当前交易日
        cerr << "---> 当前交易日：" << pUserApi->GetTradingDay() << endl;

        ///投资者结算结果确认
        ReqSettlementInfoConfirm();
    }
}

///投资者结算单确认
void TradeSpi::ReqSettlementInfoConfirm()
{
    cerr << "---> 投资者结算单确认中..." << endl;

    Sleep(1000);

    CThostFtdcSettlementInfoConfirmField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, BROKER_ID);
    strcpy(req.InvestorID, INVESTOR_ID);
    int iResult = pUserApi->ReqSettlementInfoConfirm(&req, ++iRequestID);
    if (iResult != 0) {
        cerr << "---> 确认投资者结算单失败！" << endl;
    }
}

///投资者结算单确认，回调函数
void TradeSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        cerr << "---> 投资者结算单确认成功" << endl;
        ///请求查询报单
       // RspQryOrder();
        ///请求查询成交
        // RspQryTrade();
        ///查询合约
        //ReqQryInstrument();

        ///查询资金账户
         ReqQryTradingAccount();
    }
}//请求查询报单
void TradeSpi::RspQryOrder()
{
    CThostFtdcQryOrderField *pOrder = new CThostFtdcQryOrderField();
    strcpy(pOrder->BrokerID, BROKER_ID);
    strcpy(pOrder->InvestorID, INVESTOR_ID);
    strcpy(pOrder->InstrumentID, "rb1705");
//    strcpy(pOrder->InsertTimeStart, "20140101");
//    strcpy(pOrder->InsertTimeEnd, "20170331");
    int iResult=pUserApi->ReqQryOrder(pOrder,1111);
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

///请求查询成交
void TradeSpi::RspQryTrade()
{
    CThostFtdcQryTradeField *pTrade = new CThostFtdcQryTradeField();
    strcpy(pTrade->BrokerID, BROKER_ID);
    strcpy(pTrade->InvestorID, INVESTOR_ID);
    strcpy(pTrade->InstrumentID, "rb1705");
//    strcpy(pTrade->TradeTimeStart, "20140101");
//    strcpy(pTrade->TradeTimeEnd, "20170331");
    int iResult = pUserApi->ReqQryTrade(pTrade, 1110);
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
///请求查询合约
void TradeSpi::ReqQryInstrument()
{
    cerr << "---> 查询合约中... " << endl;

    Sleep(1000);

    CThostFtdcQryInstrumentField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.InstrumentID, INSTRUMENT_ID);
    int iResult = pUserApi->ReqQryInstrument(&req, ++iRequestID);
    if (iResult != 0) {
        cerr << "---> 查询合约失败！" << endl;
    }
}

///请求查询合约，回调函数
void TradeSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        ///查询资金账户
        ReqQryTradingAccount();
    }
}

///查询资金账户
void TradeSpi::ReqQryTradingAccount()
{
    cerr << "---> 查询资金账户中... " << endl;

    Sleep(1000);

    CThostFtdcQryTradingAccountField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, BROKER_ID);
    strcpy(req.InvestorID, INVESTOR_ID);
    int iResult = pUserApi->ReqQryTradingAccount(&req, ++iRequestID);
    if (iResult != 0) {
        cerr << "---> 查询资金账户失败！" << endl;
    }
}

///查询资金账户，回调函数
void TradeSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "============================================================" << endl;
    cerr << "= 经纪公司代码：" << pTradingAccount->BrokerID << endl;
    cerr << "= 资金账号：" << pTradingAccount->AccountID << endl;
    cout << "= 期初资金：" << (int) pTradingAccount->PreBalance << endl;
    cerr << "= 动态权益：" << (int) pTradingAccount->Balance << endl;
    cerr << "= 可用资金：" << (int) pTradingAccount->Available << endl;
    cerr << "= 持仓盈亏：" << pTradingAccount->PositionProfit << endl;
    cerr << "= 平仓盈亏：" << pTradingAccount->CloseProfit << endl;
    cerr << "= 手续费  ：" << pTradingAccount->Commission << endl;
    cerr << "============================================================" << endl;

    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        ///查询持仓
        ReqQryInvestorPosition();
    }
}

///查询持仓（汇总）
void TradeSpi::ReqQryInvestorPosition()
{

    cerr << "---> 查询用户持仓中..." << endl;

    Sleep(1000);

    CThostFtdcQryInvestorPositionField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, BROKER_ID);
    strcpy(req.InvestorID, INVESTOR_ID);
    strcpy(req.InstrumentID, INSTRUMENT_ID);	// 可以查询指定的合约，不填就是所有合约
    int iResult = pUserApi->ReqQryInvestorPosition(&req, ++iRequestID);
    if (iResult != 0) {
        cerr << "---> 查询用户持仓失败！" << iResult << endl;
    }
}

///查询持仓（汇总）,回调函数
void TradeSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cout<<"OnRspQryInvestorPosition"<<endl;
    if (pInvestorPosition)
    {
        cerr << "---------------------------------------------------------------------" << endl;
        cerr << "- 合约：" << pInvestorPosition->InstrumentID << " 成本：" << pInvestorPosition->PreSettlementPrice << " 方向：" << pInvestorPosition->PosiDirection << " 昨仓" << pInvestorPosition->YdPosition << " 今仓" << pInvestorPosition->TodayPosition << " 盈亏：" << pInvestorPosition->PositionProfit << endl;
        cerr << "- 最新价：" << pInvestorPosition->SettlementPrice << " 手续费：" << pInvestorPosition->Commission << endl;
        cerr << pInvestorPosition->InstrumentID << "," << pInvestorPosition->BrokerID << "," << pInvestorPosition->InvestorID << "," << pInvestorPosition->PosiDirection << "," << pInvestorPosition->HedgeFlag << "," << pInvestorPosition->PositionDate << "," << pInvestorPosition->YdPosition << "," << pInvestorPosition->Position << "," << pInvestorPosition->LongFrozen << "," << pInvestorPosition->ShortFrozen << "," << pInvestorPosition->LongFrozenAmount << "," << pInvestorPosition->ShortFrozenAmount << "," << pInvestorPosition->OpenVolume << ",|," << pInvestorPosition->CloseVolume << "," << pInvestorPosition->OpenAmount << "," << pInvestorPosition->CloseAmount << "," << pInvestorPosition->PositionCost << "," << pInvestorPosition->PreMargin << "," << pInvestorPosition->UseMargin << "," << pInvestorPosition->FrozenMargin << "," << pInvestorPosition->FrozenCash << "," << pInvestorPosition->FrozenCommission << "," << pInvestorPosition->CashIn << "," << pInvestorPosition->Commission << "," << pInvestorPosition->CloseProfit << "," << pInvestorPosition->PositionProfit << "," << pInvestorPosition->PreSettlementPrice << "," << pInvestorPosition->SettlementPrice << "," << pInvestorPosition->TradingDay << "," << pInvestorPosition->SettlementID << "," << (int) pInvestorPosition->OpenCost << "," << pInvestorPosition->ExchangeMargin << "," << pInvestorPosition->CombPosition << "," << pInvestorPosition->CombLongFrozen << "," << pInvestorPosition->CombShortFrozen << "," << pInvestorPosition->CloseProfitByDate << "," << pInvestorPosition->CloseProfitByTrade << "," << pInvestorPosition->TodayPosition << "," << pInvestorPosition->MarginRateByMoney << "," << pInvestorPosition->MarginRateByVolume << endl;
        if (bIsLast)
            cerr << "---------------------- 查询持仓结束-----------------------------------" << endl;
    }

    if(bIsLast && !IsErrorRspInfo(pRspInfo))
    {
       cout<<"bao dan"<<endl;
        ///报单录入请求
       ReqOrderInsert();
    }
}

/// 报单(开仓)
void TradeSpi::ReqOrderInsert()
{
    CThostFtdcInputOrderField req;
    memset(&req, 0, sizeof(req));
    ///经纪公司代码
    strcpy(req.BrokerID, BROKER_ID);
    ///投资者代码
    strcpy(req.InvestorID, INVESTOR_ID);
    ///合约代码
    strcpy(req.InstrumentID, INSTRUMENT_ID);
    ///报单引用
    strcpy(req.OrderRef, ORDER_REF);
    ///用户代码
//	TThostFtdcUserIDType	UserID;
    ///报单价格条件: 限价
    req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
    ///买卖方向:买(THOST_FTDC_D_Buy),卖(THOST_FTDC_D_Sell)
    req.Direction = THOST_FTDC_D_Buy;

    ///组合开平标志: 开仓
    req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
    ///组合投机套保标志
    req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
    ///价格
    req.LimitPrice = LIMIT_PRICE;
    ///数量: 1
    req.VolumeTotalOriginal = 10;
    ///有效期类型: 当日有效
    req.TimeCondition = THOST_FTDC_TC_GFD;
    ///GTD日期
//	TThostFtdcDateType	GTDDate;
    ///成交量类型: 任何数量
    req.VolumeCondition = THOST_FTDC_VC_AV;
    ///最小成交量: 1
    req.MinVolume = 1;
    ///触发条件: 立即
    req.ContingentCondition = THOST_FTDC_CC_Immediately;
    ///止损价
//	TThostFtdcPriceType	StopPrice;
    ///强平原因: 非强平
    req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    ///自动挂起标志: 否
    req.IsAutoSuspend = 0;
    ///业务单元
//	TThostFtdcBusinessUnitType	BusinessUnit;
    ///请求编号
//	TThostFtdcRequestIDType	RequestID;
    ///用户强评标志: 否
    req.UserForceClose = 0;

    int iResult = pUserApi->ReqOrderInsert(&req, ++iRequestID);
    if (iResult != 0) {
        cerr << "---> 报单录入请求失败！" << endl;
    }
}

///综合交易平台交易核心返回的包含错误信息的报单响应
void TradeSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "---> 报单出错了（OnRspOrderInsert）" << endl;
    IsErrorRspInfo(pRspInfo);
}

///委托单（撤单）
void TradeSpi::ReqOrderAction(CThostFtdcOrderField *pOrder)
{
    CThostFtdcInputOrderActionField req;
    memset(&req, 0, sizeof(req));
    ///经纪公司代码
    strcpy(req.BrokerID, pOrder->BrokerID);
    ///投资者代码
    strcpy(req.InvestorID, pOrder->InvestorID);
    ///报单操作引用
    //	TThostFtdcOrderActionRefType	OrderActionRef;
    ///报单引用
    strcpy(req.OrderRef, pOrder->OrderRef);
    ///请求编号
    //	TThostFtdcRequestIDType	RequestID;
    ///前置编号
    req.FrontID = pOrder->FrontID;
    ///会话编号
    req.SessionID = pOrder->SessionID;
    ///交易所代码
    //	TThostFtdcExchangeIDType	ExchangeID;
    ///报单编号
    //	TThostFtdcOrderSysIDType	OrderSysID;
    ///操作标志
    req.ActionFlag = THOST_FTDC_AF_Delete;
    ///价格
    //	TThostFtdcPriceType	LimitPrice;
    ///数量变化
    //	TThostFtdcVolumeType	VolumeChange;
    ///用户代码
    //	TThostFtdcUserIDType	UserID;
    ///合约代码
    strcpy(req.InstrumentID, pOrder->InstrumentID);

    int iResult = pUserApi->ReqOrderAction(&req, ++iRequestID);
    if (iResult != 0) {
        cerr << "---> 撤消委托单操作请求失败！" << endl;
    }
}

void TradeSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "--->>> " << "OnRspOrderAction" << endl;
    IsErrorRspInfo(pRspInfo);
}

///报单通知(委托单)
void TradeSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
    cout<<"----------OnRtnOrder Start---------------------------"<<endl;
    cerr << "---> 报单通知  报单编号：" << pOrder->OrderSysID <<"," << pOrder->OrderStatus << endl;

    /// 委托单（未成交的）
    if (pOrder->OrderStatus == THOST_FTDC_OST_NoTradeQueueing)
    {
        cout << "---> 委托单（未成交的）" << pOrder->OrderSubmitStatus << "," << pOrder->CancelTime << endl;
        cout<<  "---> 状态信息："<<pOrder->StatusMsg<<endl;
        cerr << "---> 报单编号：" << pOrder->OrderSysID << endl;
        cerr << "---> 合约：" << pOrder->InstrumentID << endl;
        cerr << "---> 价格：" << pOrder->LimitPrice << endl;
        cerr << "---> 数量：" << pOrder->VolumeTotalOriginal << endl;

        ///撤单
        ReqOrderAction(pOrder);
    }
    /// 委托单（全部成交）
    else if (pOrder->OrderStatus == THOST_FTDC_OST_AllTraded)
    {
        cout << "---> 报单通知（全部成交）" << endl;
        cout<<  "---> 状态信息："<<pOrder->StatusMsg<<endl;
        cerr << "---> 报单编号：" << pOrder->OrderSysID << endl;
        cerr << "---> 合约：" << pOrder->InstrumentID << endl;
        cerr << "---> 价格：" << pOrder->LimitPrice << endl;
        cerr << "---> 数量：" << pOrder->VolumeTotalOriginal << endl;
    }
    // 委托单（撤单成功）
    else if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled)
    {
        cout << "---> 报单通知（撤单成功）" << endl;
        cout<<  "---> 状态信息："<<pOrder->StatusMsg<<endl;
        cerr << "---> 报单编号：" << pOrder->OrderSysID << endl;
        cerr << "---> 合约：" << pOrder->InstrumentID << endl;
        cerr << "---> 价格：" << pOrder->LimitPrice << endl;
        cerr << "---> 数量：" << pOrder->VolumeTotalOriginal << endl;
    }
    else
    {
        cout<<"---> 状态信息:"<<pOrder->StatusMsg<<endl;
    }
     cout<<"----------OnRtnOrder End-----------------------------"<<endl;
}

///成交通知
void TradeSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
    cerr << "---> 成交通知(OnRtnTrade) " << endl;
    cerr << "------------------------------------------------------------" << endl;
    cout << "- 合约：" << pTrade->InstrumentID << " 方向："<< pTrade->Direction << " 开平：" << pTrade->OffsetFlag << endl;
    cerr << "- 价格：" << pTrade->Price << endl;
    cerr << "- 数量：" << pTrade->Volume << endl;
    cerr << "- 成交编号：" << pTrade->TradeID << endl;
    cerr << "------------------------------------------------------------" << endl;

    //cerr << pTrade->BrokerID << "," << pTrade->InvestorID << "," << pTrade->InstrumentID << "," << pTrade->OrderRef << "," << pTrade->UserID << "," << pTrade->ExchangeID << "," << pTrade->TradeID << "," << pTrade->Direction << "," << pTrade->OrderSysID << "," << pTrade->ParticipantID << "," << pTrade->ClientID << "," << pTrade->TradingRole << "," << pTrade->ExchangeInstID << "," << pTrade->OffsetFlag << "," << pTrade->HedgeFlag << "," << pTrade->Price << "," << pTrade->Volume << "," << pTrade->TradeDate << "," << pTrade->TradeTime << "," << pTrade->TradeType << "," << pTrade->PriceSource << "," << pTrade->TraderID << "," << pTrade->OrderLocalID << "," << pTrade->ClearingPartID << "," << pTrade->BusinessUnit << "," << pTrade->SequenceNo << "," << pTrade->TradingDay << "," << pTrade->SettlementID << "," << pTrade->BrokerOrderSeq << "," << pTrade->TradeSource << "," << endl;
}

void TradeSpi:: OnFrontDisconnected(int nReason)
{
    cerr << "--->>> " << "OnFrontDisconnected" << endl;
    cerr << "--->>> Reason = " << nReason << endl;
}

void TradeSpi::OnHeartBeatWarning(int nTimeLapse)
{
    cerr << "--->>> " << "OnHeartBeatWarning" << endl;
    cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void TradeSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "--->>> " << "OnRspError" << endl;
    IsErrorRspInfo(pRspInfo);
}

// 是否收到成功的响应
bool TradeSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
    // 如果ErrorID != 0, 说明收到了错误的响应
    bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
    if (bResult)
        cerr << "---> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
    return bResult;
}

// 是否我的报单回报
bool TradeSpi::IsMyOrder(CThostFtdcOrderField *pOrder)
{
    return ((pOrder->FrontID == FRONT_ID) &&
            (pOrder->SessionID == SESSION_ID) &&
            (strcmp(pOrder->OrderRef, ORDER_REF) == 0));
}

// 是否正在交易的报单
bool TradeSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
    return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
            (pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
            (pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}

