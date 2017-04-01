#ifndef TRADESPI_H
#define TRADESPI_H
#include <ThostFtdcMdApi.h>
#include <ThostFtdcTraderApi.h>
#include <ThostFtdcUserApiDataType.h>
#include <ThostFtdcUserApiStruct.h>

class TradeSpi:public CThostFtdcTraderSpi
{
public:
    //构造函数
        TradeSpi(CThostFtdcTraderApi *tdapi);

        //当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
        void OnFrontConnected();

        void OnFrontDisconnected(int nReason) override;
        //登录请求响应
        void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);



        //请求查询合约
       void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo,
                                        int nRequestID, bool bIsLast) override;


        //投资者结算结果确认响应
        void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
            CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;


        //请求查询行情响应
        void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData,
            CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

        //请求查询投资者持仓响应
        void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
            CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;



        //查询资金帐户响应
        void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount,
            CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

        //报单录入错误的时候相应
        void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo,
                                          int nRequestID, bool bIsLast) override;
        //报单回报
        void OnRtnOrder(CThostFtdcOrderField *pOrder) override;

        //成交通知
       void OnRtnTrade(CThostFtdcTradeField *pTrade) override;

       //请求查询报单响应
       void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo,
                          int nRequestID, bool bIsLast) override;

       //请求查询成交响应
       void OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo,
                          int nRequestID, bool bIsLast) override;

private:
        //用户登录请求
        void ReqUserLogin();
        //用户查询行情
        void RspQryDepthMarketData();
        //投资者结算结果确认
        void ReqSettlementInfoConfirm();
        //请求查询合约
        void ReqQryInstrument();
        //请求查询资金账户
        void ReqQryTradingAccount();
        //请求查询投资者持仓
        void ReqQryInvestorPosition();
        //报单录入请求
        void ReqOrderInsert();
        //报单操作请求
        void ReqOrderAction(CThostFtdcOrderField *pOrder);
        //请求查询报单
        void RspQryOrder();
        //请求查询成交
        void RspQryTrade();
        // 是否收到成功的响应
        bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
        // 是否我的报单回报
        bool IsMyOrder(CThostFtdcOrderField *pOrder);
        // 是否正在交易的报单
        bool IsTradingOrder(CThostFtdcOrderField *pOrder);
        CThostFtdcTraderApi *tdapi;
        CThostFtdcReqUserLoginField *loginField;
        CThostFtdcReqAuthenticateField *authField;
};

#endif // TRADESPI_H
