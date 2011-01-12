// GrowBoxSoapServer.h: interface for the GrowBoxSoapServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GROWBOXSOAPSERVER_H__4E85D5CD_28F2_40F5_9736_80151B6D0042__INCLUDED_)
#define AFX_GROWBOXSOAPSERVER_H__4E85D5CD_28F2_40F5_9736_80151B6D0042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class GrowBoxSoapServer : public CObject 
{
public:
	void waitForServerStopped();
	void stopServer();
	void startServer();
	int setValues(DWORD& cnt, float& tmp, float& hum, CString& state);
	GrowBoxSoapServer();
	virtual ~GrowBoxSoapServer();

	static GrowBoxSoapServer* GBSoapServer;

	class Mutex{
	public:
		Mutex(HANDLE* lpMutex);
		~Mutex();
		HANDLE mutex;
	};
	static HANDLE accessMutex;	

private:
	void accessProtected();
	HANDLE serverThread;  
	static long WINAPI GrowBoxSoapServer::serverMethod(void* pParam); 

	void startServerThread();



public:
	void publishValues(DWORD cnt, float& tmp, float& hum, CString& stat);
	CString StateValue;
	DWORD   HygroCntValue;
	float	TemperatureValue;
	float   HumidityValue;
};

#endif // !defined(AFX_GROWBOXSOAPSERVER_H__4E85D5CD_28F2_40F5_9736_80151B6D0042__INCLUDED_)
