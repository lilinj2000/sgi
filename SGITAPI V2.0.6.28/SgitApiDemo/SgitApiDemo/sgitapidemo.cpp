// mdusertest.cpp :
//һ���򵥵����ӣ�����CSgitFtdcMduserApi��CSgitFtdcMduserSpi�ӿڵ�ʹ�á�
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
			// �˵�ʧ�ܣ��ͻ�������д�����
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
		///�����ѯ��Լ��Ӧ
		void OnRspQryInstrument(CSgitFtdcInstrumentField *pInstrument, CSgitFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
		{
			printf("OnRspQryInstrument \n");
			//
			if (pInstrument != NULL)
			{
				printf("Instrument info: %s,%.2f\n",pInstrument->InstrumentID,pInstrument->PriceTick);
			} 
			//ֻ��һ��
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
				ReqOrderAdd.OrderPriceType = Sgit_FTDC_OPT_LimitPrice;//�޼�
				sprintf(ReqOrderAdd.CombHedgeFlag,"%s","1");//Ͷ����־-Ͷ��
				//
				ReqOrderAdd.TimeCondition = Sgit_FTDC_TC_GFD;//������Ч
				ReqOrderAdd.VolumeCondition = Sgit_FTDC_VC_AV;//�ɽ�������-�κ�����
				ReqOrderAdd.MinVolume = 1;//��С�ɽ���
				ReqOrderAdd.ContingentCondition = Sgit_FTDC_CC_Immediately;//��������-����
				ReqOrderAdd.ForceCloseReason = Sgit_FTDC_FCC_NotForceClose;//ǿƽԭ��-��ǿƽ
				//
				sprintf(ReqOrderAdd.InvestorID,"%s","30057791");//
				sprintf(ReqOrderAdd.InstrumentID,"%s","IF1303");//��Լ
				ReqOrderAdd.Direction = '0';//���� 0-��  1-��
				sprintf(ReqOrderAdd.CombOffsetFlag,"%s","0");//��ƽ 0-��  1-ƽ�� 2-ǿƽ 3-ƽ��  4-ƽ�� 5-ǿ��
				ReqOrderAdd.LimitPrice = 2090.2;//�۸�
				ReqOrderAdd.VolumeTotalOriginal = 1;//����
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
		void OnRtnInstrumentStatus(CSgitFtdcInstrumentStatusField *pInstrumentStatus)
		{
			printf("OnRtnInstrumentStatus: InstrumentID=%s,ExchangeID=%s\n",
				pInstrumentStatus->InstrumentID,pInstrumentStatus->ExchangeID);
		}

		///�����ѯͶ���ֲ߳���ϸ��Ӧ
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
	// ���캯������Ҫһ����Ч��ָ��CSgitFtdcMduserApiʵ����ָ��
	CSimpleHandler(CSgitFtdcMdApi *pUserApi) : m_pUserApi(pUserApi) {}
	~CSimpleHandler() {}
	// ���ͻ��������鷢��������������ͨ�����ӣ��ͻ�����Ҫ���е�¼
	void OnFrontConnected() {
		CSgitFtdcReqUserLoginField reqUserLogin;
		//strcpy(reqUserLogin.TradingDay, m_pUserApi->GetTradingDay());
		//strcpy(reqUserLogin.BrokerID, "yyy");
		strcpy(reqUserLogin.UserID, gszUserID);
		strcpy(reqUserLogin.Password, gszPwd);
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
	pUserTradeApi->Init(false);//����ģʽ
	pUserTradeApi->Init(false,true);//����ģʽ
	//
	//20130110  Add End
	//
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
	pUserApi->RegisterFront("tcp://221.133.247.114:7777");
	// ���öಥ������ MlCast://hostIP$multiaddress:port��
	//�������Ҫʹ������ಥ�����Բ�����������
//	pUserApi->SetMultiCastAddr("MlCast://ANY$224.0.1.12:5555");
	// ʹ�ͻ��˿�ʼ�����鷢����������������
	pUserApi->Init(false);
	//

	//20130110  Add Begin
	pUserTradeApi->Join();
	//20130110  Add End
	
	// �ͷ�useapiʵ��
	pUserApi->Join();
	pUserApi->Release();
	//20130110  Add Begin
	pUserTradeApi->Release();
	//20130110  Add End
	return 0;
}
