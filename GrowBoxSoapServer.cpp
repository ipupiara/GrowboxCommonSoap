// GrowBoxSoapServer.cpp: implementation of the GrowBoxSoapServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GrowBoxSoapServer.h"

#include "soapH.h"
#include "states.nsmap"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HANDLE GrowBoxSoapServer::accessMutex;
GrowBoxSoapServer*  GrowBoxSoapServer::GBSoapServer = NULL;


GrowBoxSoapServer::GrowBoxSoapServer()
{
	TemperatureValue = 0.0f;
	HygroCntValue  = 0;
	HumidityValue = 0.0f;
	StateValue = "";

}

GrowBoxSoapServer::~GrowBoxSoapServer()
{

}


long WINAPI GrowBoxSoapServer::serverMethod(void* pParam)
{
	printf("Hello World!\n");

 struct soap soap; 
   int m, s; // master and slave sockets 
   soap_init(&soap); 
   m = soap_bind(&soap, "127.0.0.1", 18083, 100); 
   if (m < 0) {
		TRACE0("Soap bind error\n");
      soap_print_fault(&soap, stderr); 
   }
   else
   { 
      TRACE1("SOAP Socket connection successful: master socket = %d\n", m); 
      for (int i = 1; ; i++) 
      {
         s = soap_accept(&soap); 
         if (s < 0) 
         { 
            soap_print_fault(&soap, stderr); 
            break; 
         } 
/*         fprintf(stderr, "%d: accepted connection from IP=%d.%d.%d.%d socket=%d", i, 
            (soap.ip >> 24)&0xFF, (soap.ip >> 16)&0xFF, (soap.ip >> 8)&0xFF, soap.ip&0xFF, s); 
*/

         if (soap_serve(&soap) != SOAP_OK)  
            soap_print_fault(&soap, stderr); 
 //        TRACE0( "request served\n"); 
         soap_destroy(&soap);  
         soap_end(&soap); 
      } 
   } 
   soap_done(&soap);

// TODO: terminate via closeing socket and wait to end of thread !



	TRACE0("\nSOAP ServerTHREAD::server Thread Returning\n");
	return 0;
}



void GrowBoxSoapServer::startServerThread()
{
	TRACE0("\nstart GrowBoxSoapServer server\n");

	serverThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)serverMethod,this,0,NULL);
}


GrowBoxSoapServer::Mutex::Mutex(HANDLE* lpMutex)
{
	mutex = * lpMutex;
	WaitForSingleObject(mutex,INFINITE);
}

GrowBoxSoapServer::Mutex::~Mutex()
{
	SetEvent(mutex);
}
	

void GrowBoxSoapServer::accessProtected()
{
	Mutex m (&accessMutex);
}

int GrowBoxSoapServer::setValues(DWORD &cnt, float &tmp, float &hum, CString &state)
{
	Mutex m (&accessMutex);

	HygroCntValue = cnt;
	TemperatureValue = tmp;
	HumidityValue = hum;
	StateValue = state;

	return 0;
}

void GrowBoxSoapServer::startServer()
{
	GrowBoxSoapServer::GBSoapServer = this;
   startServerThread();
}

void GrowBoxSoapServer::stopServer()
{

}

void GrowBoxSoapServer::waitForServerStopped()
{

}


#define BuffSize 1024
char BufferForServerThread [BuffSize];

SOAP_FMAC5 int SOAP_FMAC6 ns__getGrowBoxInfo(struct soap*, GrowBoxInfo *Result)
{
	GrowBoxSoapServer::Mutex m (&GrowBoxSoapServer::accessMutex);

	Result->HygroCnt=GrowBoxSoapServer::GBSoapServer->HygroCntValue;
	Result->Temperature = GrowBoxSoapServer::GBSoapServer->TemperatureValue;
	Result->Humidity= GrowBoxSoapServer::GBSoapServer->HumidityValue;

	memset(BufferForServerThread,0,sizeof(BufferForServerThread));
	strncpy(BufferForServerThread,(LPCSTR) (GrowBoxSoapServer::GBSoapServer->StateValue), BuffSize -1);

	Result->State = BufferForServerThread;

   return SOAP_OK; 
}



void GrowBoxSoapServer::publishValues(DWORD cnt, float &tmp, float &hum, CString &stat)
{
	Mutex m (&accessMutex);

	HumidityValue = hum;
	TemperatureValue  = tmp;
	HygroCntValue = cnt;
	StateValue = stat;

}
