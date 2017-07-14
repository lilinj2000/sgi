// mdusertest.cpp :
//一个简单的例子，介绍CSgitFtdcMduserApi和CSgitFtdcMduserSpi接口的使用。
#include "stdio.h"
#include <string.h>
#include <unistd.h>
#include "SgitFtdcMdApi.h"
//20130110 
#include "SgitFtdcTraderApi.h"
//
char gszUserID[] = { "9001" };
char gszPwd[] = { "888888" };
//
class CTradeHandle : public CSgitFtdcTraderSpi
{
public:
		CTradeHandle(CSgitFtdcTraderApi *pUserApi) : m_pUserApi(pUserApi) {}
		~CTradeHandle() {}
		//
		void OnFrontConnected()
		{
			CSgitFtdcReqUserLoginField reqUserLogin;
			strcpy(reqUserLogin.TradingDay, m_pUserApi->GetTradingDay());
			strcpy(reqUserLogin.BrokerID, "yyy");
			strcpy(reqUserLogin.UserID, gszUserID);
			strcpy(reqUserLogin.Password, gszPwd);
			//
			int iRet = m_pUserApi->ReqUserLogin(&reqUserLogin,0);
			printf("login: %d\n",iRet);
		}
		//
		void OnFrontDisconnected(char *pErrMsg)
		{
			printf("OnFrontDisconnected: %s\n",pErrMsg);
		}
		//
		void OnRspUserLogin(CSgitFtdcRspUserLoginField *pRspUserLogin, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
		{
			printf("OnRspUserLogin:\n");
			printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
			printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
			
			if (pRspInfo->ErrorID == 0) 
			{
					m_pUserApi->Ready();
			}
			if (pRspInfo->ErrorID != 0) 
			{
			// 端登失败，客户端需进行错误处理
				printf("Failed to login, errorcode=%d errormsg=%s requestid=%d chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
			}
		}
		//
		void OnRtnInstrumentStatus(CSgitFtdcInstrumentStatusField *pInstrumentStatus)
		{
			printf("OnRtnInstrumentStatus: InstrumentID=%s,ExchangeID=%s\n",
				pInstrumentStatus->InstrumentID,pInstrumentStatus->ExchangeID);
		}
		
		//
		void onRspMBLQuot(CSgitMBLQuotData *pMBLQuotData,CSgitFtdcRspInfoField *pRspMsg,int nRequestID,bool bIsLast)
		{
			printf("onRspMBLQuot nRequestID=%d,bIsLast=%s\n",nRequestID,(bIsLast? "true" : "false"));
			if(pRspMsg != NULL)
			{
				printf("onRspMBLQuot ErrorID=%d,ErrorMsg=%s\n",pRspMsg->ErrorID,pRspMsg->ErrorMsg);
			}
			if(pMBLQuotData != NULL)
			{
				printf("onRspMBLQuot ContractID=%s,BsFlag=%c,Price=%.2lf,Qty=%d\n",
					pMBLQuotData->ContractID,pMBLQuotData->BsFlag,pMBLQuotData->Price,pMBLQuotData->Qty);
			}
			return ;
		}
		//
		///报单通知
		void OnRtnOrder(CSgitFtdcOrderField *pOrder,CSgitFtdcRspInfoField *pRspInfo)
		{
			if (pRspInfo->ErrorID == 0)
			{
				printf("OnRtnOrder success\n");
			} 
			else
			{
				printf("OnRtnOrder error:%d - %s\n",pRspInfo->ErrorID,pRspInfo->ErrorMsg);
			}
		}
		
		///成交通知
		void OnRtnTrade(CSgitFtdcTradeField *pTrade)
		{
			printf("OnRtnTrade \n");
		}
		//

private:
	CSgitFtdcTraderApi *m_pUserApi;
};
//20130110
class CSimpleHandler : public CSgitFtdcMdSpi
{
public:
	// 构造函数，需要一个有效的指向CSgitFtdcMduserApi实例的指针
	CSimpleHandler(CSgitFtdcMdApi *pUserApi) : m_pUserApi(pUserApi) {}
	~CSimpleHandler() {}
	// 当客户端与行情发布服务器建立起通信连接，客户端需要进行登录
	void OnFrontConnected() {
		CSgitFtdcReqUserLoginField reqUserLogin;
		strcpy(reqUserLogin.TradingDay, m_pUserApi->GetTradingDay());
		strcpy(reqUserLogin.BrokerID, "yyy");
		strcpy(reqUserLogin.UserID, "admin1");
		strcpy(reqUserLogin.Password, "888888");
		m_pUserApi->ReqUserLogin(&reqUserLogin, 0);
	}
	// 当客户端与行情发布服务器通信连接断开时，该方法被调用
	void OnFrontDisconnected() {
		// 当发生这个情况后，API会自动重新连接，客户端可不做处理
		printf("OnFrontDisconnected.\n");
	}
	// 当客户端发出登录请求之后，该方法会被调用，通知客户端登录是否成功
	void OnRspUserLogin(CSgitFtdcRspUserLoginField *pRspUserLogin, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		printf("OnRspUserLogin:\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
	
		if (pRspInfo->ErrorID == 0) {
				m_pUserApi->Ready();
		}
		if (pRspInfo->ErrorID != 0) {
// 端登失败，客户端需进行错误处理
			printf("Failed to login, errorcode=%d errormsg=%s requestid=%d chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
		}
	}
	// 深度行情通知，行情服务器会主动通知客户端
	///深度行情通知
	//void OnRtnDepthMarketData(CSgitFtdcDepthMarketDataField *pDepthMarketData) {};

	void OnRtnDepthMarketData(CSgitFtdcDepthMarketDataField *pMarketData) 
	{
		// 客户端按需处理返回的数据
		printf("receive contract=%s Volume=%d lastprice=%f\n",pMarketData->InstrumentID,pMarketData->Volume, pMarketData->LastPrice);
	}
	// 针对用户请求的出错通知
	void OnRspError(CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		printf("OnRspError:\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
		// 客户端需进行错误处理
	}
private:
	// 指向CSgitFtdcMduserApi实例的指针
	CSgitFtdcMdApi *m_pUserApi;
};
int main()
{
	//20130110  Add Begin
	CSgitFtdcTraderApi *pUserTradeApi = CSgitFtdcTraderApi::CreateFtdcTraderApi("./");
	CTradeHandle th(pUserTradeApi);
	pUserTradeApi->RegisterSpi(&th);
	pUserTradeApi->SubscribePrivateTopic(Sgit_TERT_RESTART);
	pUserTradeApi->SubscribePublicTopic(Sgit_TERT_RESTART);
	//
	pUserTradeApi->RegisterFront("tcp://192.168.1.50:7776");
	//
	//pUserTradeApi->Init(true,true);//极速模式
	pUserTradeApi->Init(true,false);//非极速模式
	//
	//20130110  Add End
	
	// 产生一个CSgitFtdcMduserApi实例
	CSgitFtdcMdApi *pUserApi = CSgitFtdcMdApi::CreateFtdcMdApi("./");
	// 产生一个事件处理的实例
	printf("I am here 1\n");
	CSimpleHandler sh(pUserApi);
	// 注册一事件处理的实例
	pUserApi->RegisterSpi(&sh);
	printf("I am here 2\n");
	// 注册需要的深度行情主题
	/// TERT_RESTART:从本交易日开始重传
	/// TERT_RESUME:从上次收到的续传
	/// TERT_QUICK:先传送当前行情快照,再传送登录后市场行情的内容
	pUserApi-> SubscribeMarketTopic (Sgit_TERT_RESTART);
	// 设置交易系统行情前置NameServer的地址
	pUserApi->RegisterFront("tcp://192.168.1.50:7777");
	// 设置行情发布服务器的地址
	// 使客户端开始与行情发布服务器建立连接
	pUserApi->Init(false);
	
	//20130527 YZ Add Begin
	sleep(3);
	CSgitMBLQuotReq req;
	memset(&req,0,sizeof(CSgitMBLQuotReq));
	sprintf(req.StartContractID,"%s","au1306");
	sprintf(req.EndContractID,"%s","au1307");
	req.BsFlag = Sgit_FTDC_D_Buy;
	//
	int iRet = pUserTradeApi->ReqMBLQuot(5,&req);
	if(iRet != 0)
	{
		printf("ReqMBLQuot error: %d\n",iRet);
	}
	else
	{
		printf("ReqMBLQuot: %s\n",req.StartContractID);
	}
	//20130527 YZ Add End
	
	//20130110  Add Begin
	pUserTradeApi->Join();
	//20130110  Add End
	
	// 释放useapi实例
	pUserApi->Join() ;
	pUserApi->Release();
	//20130110  Add Begin
	pUserTradeApi->Release();
	//20130110  Add End
	return 0;
}
