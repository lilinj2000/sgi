// mdusertest.cpp :
//一个简单的例子，介绍CSgitFtdcMduserApi和CSgitFtdcMduserSpi接口的使用。
#include "stdio.h"
#include <string.h>
#include "SgitFtdcMdApi.h"
//20130110 
#include "SgitFtdcTraderApi.h"
//
#ifdef _DEBUG
#pragma comment(lib,"sgitquotapi.lib ")
#pragma comment(lib,"sgittradeapi.lib ")
#else
#pragma comment(lib,"sgitquotapi.lib ")
#pragma comment(lib,"sgittradeapi.lib ")
#endif
//
char gszUserID[] = { "admin2" };
char gszPwd[] = { "888888" };
//
static bool bOrder = true;
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
			//strcpy(reqUserLogin.BrokerID, "yyy");
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
				return;
			}
			//
			CSgitFtdcQryInstrumentField reqInstrument;
			memset(&reqInstrument,0,sizeof(CSgitFtdcQryInstrumentField));
			if (m_pUserApi->ReqQryInstrument(&reqInstrument,1) == 0)
			{
				printf("send query Instrument success.\n");
			} 
			else
			{
				printf("send query Instrument error\n");
			}
		}
		//
		///请求查询合约响应
		void OnRspQryInstrument(CSgitFtdcInstrumentField *pInstrument, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
		{
			printf("OnRspQryInstrument \n");
			//
			if (pInstrument != NULL)
			{
				printf("Instrument info: %s,%.2f\n",pInstrument->InstrumentID,pInstrument->PriceTick);
			} 
			//只发一笔
			if (bOrder == false)
			{
				CSgitFtdcInputOrderField ReqOrderAdd;
				//
				memset(&ReqOrderAdd,0,sizeof(ReqOrderAdd));
				//strncpy(ReqOrderAdd.UserID,"9002",sizeof(ReqOrderAdd.UserID) - 1);
				sprintf(ReqOrderAdd.UserID,"%s",gszUserID);
				ReqOrderAdd.RequestID = 1;
				const char *pch = "000000000001";//
				strncpy(ReqOrderAdd.OrderRef,pch,sizeof(ReqOrderAdd.OrderRef) - 1);
				ReqOrderAdd.OrderPriceType = Sgit_FTDC_OPT_LimitPrice;//限价
				sprintf(ReqOrderAdd.CombHedgeFlag,"%s","1");//投保标志-投机
				//
				ReqOrderAdd.TimeCondition = Sgit_FTDC_TC_GFD;//当日有效
				ReqOrderAdd.VolumeCondition = Sgit_FTDC_VC_AV;//成交量类型-任何数量
				ReqOrderAdd.MinVolume = 1;//最小成交量
				ReqOrderAdd.ContingentCondition = Sgit_FTDC_CC_Immediately;//触发条件-立即
				ReqOrderAdd.ForceCloseReason = Sgit_FTDC_FCC_NotForceClose;//强平原因-非强平
				//
				sprintf(ReqOrderAdd.InvestorID,"%s","30057791");//
				sprintf(ReqOrderAdd.InstrumentID,"%s","IF1303");//合约
				ReqOrderAdd.Direction = '0';//买卖 0-买  1-卖
				sprintf(ReqOrderAdd.CombOffsetFlag,"%s","0");//开平 0-开  1-平仓 2-强平 3-平今  4-平昨 5-强减
				ReqOrderAdd.LimitPrice = 2090.2;//价格
				ReqOrderAdd.VolumeTotalOriginal = 1;//数量
				//
				int iRet = m_pUserApi->ReqOrderInsert(&ReqOrderAdd,2);
				if (iRet == 0)
				{
					printf("ReqOrderInsert success:%d \n",iRet);
				}
				else
				{
					printf("ReqOrderInsert error:%d \n",iRet);
				}
				//
				bOrder = true;
			}
			//
			if (bIsLast)
			{
				CSgitFtdcQryInvestorPositionDetailField reqQryInvestorPositionDetail;
				memset(&reqQryInvestorPositionDetail,0,sizeof(CSgitFtdcQryInvestorPositionDetailField));
				//
				int iRet = m_pUserApi->ReqQryInvestorPositionDetail(&reqQryInvestorPositionDetail,3);
				if (iRet == 0)
				{
					printf("Query success\n");
				} 
				else
				{
					printf("Query error:%d\n",iRet);
				}
			}
		}
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
		void OnRtnInstrumentStatus(CSgitFtdcInstrumentStatusField *pInstrumentStatus)
		{
			printf("OnRtnInstrumentStatus: InstrumentID=%s,ExchangeID=%s\n",
				pInstrumentStatus->InstrumentID,pInstrumentStatus->ExchangeID);
		}

		///请求查询投资者持仓明细响应
	virtual void OnRspQryInvestorPositionDetail(CSgitFtdcInvestorPositionDetailField *pInvestorPositionDetail, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQryInvestorPositionDetail\n");
		if (pInvestorPositionDetail)
		{
			printf("%s-%s-%s-%c-%c-%d-%s-%.2lf\n",pInvestorPositionDetail->ExchangeID,pInvestorPositionDetail->InvestorID,
				pInvestorPositionDetail->InstrumentID,pInvestorPositionDetail->HedgeFlag,pInvestorPositionDetail->Direction,
				pInvestorPositionDetail->Volume,pInvestorPositionDetail->OpenDate,pInvestorPositionDetail->OpenPrice);
		}
	}
			
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
		//strcpy(reqUserLogin.TradingDay, m_pUserApi->GetTradingDay());
		//strcpy(reqUserLogin.BrokerID, "yyy");
		strcpy(reqUserLogin.UserID, gszUserID);
		strcpy(reqUserLogin.Password, gszPwd);
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
	//
	//20130110  Add Begin
	CSgitFtdcTraderApi *pUserTradeApi = CSgitFtdcTraderApi::CreateFtdcTraderApi("./");
	CTradeHandle th(pUserTradeApi);
	pUserTradeApi->RegisterSpi(&th);
	pUserTradeApi->SubscribePrivateTopic(Sgit_TERT_RESTART);
	pUserTradeApi->SubscribePublicTopic(Sgit_TERT_RESTART);
	//
	pUserTradeApi->RegisterFront("tcp://221.133.247.114:7776");
	//
	pUserTradeApi->Init(false);//快速模式
	pUserTradeApi->Init(false,true);//极速模式
	//
	//20130110  Add End
	//
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
	pUserApi->RegisterFront("tcp://221.133.247.114:7777");
	// 设置多播的配置 MlCast://hostIP$multiaddress:port，
	//如果不需要使用行情多播，可以不做以下设置
//	pUserApi->SetMultiCastAddr("MlCast://ANY$224.0.1.12:5555");
	// 使客户端开始与行情发布服务器建立连接
	pUserApi->Init(false);
	//

	//20130110  Add Begin
	pUserTradeApi->Join();
	//20130110  Add End
	
	// 释放useapi实例
	pUserApi->Join();
	pUserApi->Release();
	//20130110  Add Begin
	pUserTradeApi->Release();
	//20130110  Add End
	return 0;
}
