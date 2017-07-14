20170625升级内容:
	接口修正:
		CSgitFtdcTraderApi::Init(bool isLogged) 
		增加参数,变为:
		CSgitFtdcTraderApi::Init(bool isLogged,bool bFastMode=false)
	功能修正:
		新增api极速模式,
		Init(bool isLogged,bool bFastMode=false)中 bFastMode=true时,为极速模式.
		极速模式运行api,回调线程会100%占用一个逻辑cpu;