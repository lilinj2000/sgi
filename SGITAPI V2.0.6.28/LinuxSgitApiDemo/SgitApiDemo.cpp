// mdusertest.cpp :
//һ���򵥵����ӣ�����CSgitFtdcMduserApi��CSgitFtdcMduserSpi�ӿڵ�ʹ�á�
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
			// �˵�ʧ�ܣ��ͻ�������д�����
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
		///����֪ͨ
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
		
		///�ɽ�֪ͨ
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
	// ���캯������Ҫһ����Ч��ָ��CSgitFtdcMduserApiʵ����ָ��
	CSimpleHandler(CSgitFtdcMdApi *pUserApi) : m_pUserApi(pUserApi) {}
	~CSimpleHandler() {}
	// ���ͻ��������鷢��������������ͨ�����ӣ��ͻ�����Ҫ���е�¼
	void OnFrontConnected() {
		CSgitFtdcReqUserLoginField reqUserLogin;
		strcpy(reqUserLogin.TradingDay, m_pUserApi->GetTradingDay());
		strcpy(reqUserLogin.BrokerID, "yyy");
		strcpy(reqUserLogin.UserID, "admin1");
		strcpy(reqUserLogin.Password, "888888");
		m_pUserApi->ReqUserLogin(&reqUserLogin, 0);
	}
	// ���ͻ��������鷢��������ͨ�����ӶϿ�ʱ���÷���������
	void OnFrontDisconnected() {
		// ��������������API���Զ��������ӣ��ͻ��˿ɲ�������
		printf("OnFrontDisconnected.\n");
	}
	// ���ͻ��˷�����¼����֮�󣬸÷����ᱻ���ã�֪ͨ�ͻ��˵�¼�Ƿ�ɹ�
	void OnRspUserLogin(CSgitFtdcRspUserLoginField *pRspUserLogin, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		printf("OnRspUserLogin:\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
	
		if (pRspInfo->ErrorID == 0) {
				m_pUserApi->Ready();
		}
		if (pRspInfo->ErrorID != 0) {
// �˵�ʧ�ܣ��ͻ�������д�����
			printf("Failed to login, errorcode=%d errormsg=%s requestid=%d chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
		}
	}
	// �������֪ͨ�����������������֪ͨ�ͻ���
	///�������֪ͨ
	//void OnRtnDepthMarketData(CSgitFtdcDepthMarketDataField *pDepthMarketData) {};

	void OnRtnDepthMarketData(CSgitFtdcDepthMarketDataField *pMarketData) 
	{
		// �ͻ��˰��账���ص�����
		printf("receive contract=%s Volume=%d lastprice=%f\n",pMarketData->InstrumentID,pMarketData->Volume, pMarketData->LastPrice);
	}
	// ����û�����ĳ���֪ͨ
	void OnRspError(CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		printf("OnRspError:\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
		// �ͻ�������д�����
	}
private:
	// ָ��CSgitFtdcMduserApiʵ����ָ��
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
	//pUserTradeApi->Init(true,true);//����ģʽ
	pUserTradeApi->Init(true,false);//�Ǽ���ģʽ
	//
	//20130110  Add End
	
	// ����һ��CSgitFtdcMduserApiʵ��
	CSgitFtdcMdApi *pUserApi = CSgitFtdcMdApi::CreateFtdcMdApi("./");
	// ����һ���¼������ʵ��
	printf("I am here 1\n");
	CSimpleHandler sh(pUserApi);
	// ע��һ�¼������ʵ��
	pUserApi->RegisterSpi(&sh);
	printf("I am here 2\n");
	// ע����Ҫ�������������
	/// TERT_RESTART:�ӱ������տ�ʼ�ش�
	/// TERT_RESUME:���ϴ��յ�������
	/// TERT_QUICK:�ȴ��͵�ǰ�������,�ٴ��͵�¼���г����������
	pUserApi-> SubscribeMarketTopic (Sgit_TERT_RESTART);
	// ���ý���ϵͳ����ǰ��NameServer�ĵ�ַ
	pUserApi->RegisterFront("tcp://192.168.1.50:7777");
	// �������鷢���������ĵ�ַ
	// ʹ�ͻ��˿�ʼ�����鷢����������������
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
	
	// �ͷ�useapiʵ��
	pUserApi->Join() ;
	pUserApi->Release();
	//20130110  Add Begin
	pUserTradeApi->Release();
	//20130110  Add End
	return 0;
}
