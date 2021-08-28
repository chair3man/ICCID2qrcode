#include<iostream>
//#include "SerialPort.h"
#include <tchar.h>
using namespace std;
#include <Windows.h>  
#include <process.h>  
#include <devguid.h>
#include <setupapi.h>
#include "qrencode.h"
#include <time.h>
#pragma comment(lib, "Setupapi.lib")
#define UNICODE
int recv_cout = 0;
int at_recv_count = 0;
char recv_at[128] = { 0 , };
char ICCID[48] = { 0 , };
char IMEI[48] = { 0 , };
char *ret = NULL;
#define INCHES_PER_METER (100.0/2.54)

static int casesensitive = 1;
static int eightbit = 0;
static int version = 0;
static int ssize = 3;
static int margin = -1;
static int dpi = 72;
static int structured = 0;
static int rle = 0;
static int micro = 0;
static QRecLevel level = QR_ECLEVEL_L;
static QRencodeMode hint = QR_MODE_8;
static unsigned int fg_color[4] = { 0, 0, 0, 255 };
static unsigned int bg_color[4] = { 255, 255, 255, 255 };

static int verbose = 1;

enum imageType {
	PNG_TYPE,
	EPS_TYPE,
	SVG_TYPE,
	ANSI_TYPE,
	ANSI256_TYPE,
	ASCII_TYPE,
	ASCIIi_TYPE,
	UTF8_TYPE,
	ANSIUTF8_TYPE
};

static enum imageType image_type = PNG_TYPE;

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */
//#include <iostream>  
/** 串口通信类
*
*  本类实现了对串口的基本操作
*  例如监听发到指定串口的数据、发送指定数据到串口
*/
class CSerialPort
{
public:
	CSerialPort(void);
	~CSerialPort(void);

public:

	/** 初始化串口函数
	*
	*  @param:  UINT portNo 串口编号,默认值为1,即COM1,注意,尽量不要大于9
	*  @param:  UINT baud   波特率,默认为9600
	*  @param:  char parity 是否进行奇偶校验,'Y'表示需要奇偶校验,'N'表示不需要奇偶校验
	*  @param:  UINT databits 数据位的个数,默认值为8个数据位
	*  @param:  UINT stopsbits 停止位使用格式,默认值为1
	*  @param:  DWORD dwCommEvents 默认为EV_RXCHAR,即只要收发任意一个字符,则产生一个事件
	*  @return: bool  初始化是否成功
	*  @note:   在使用其他本类提供的函数前,请先调用本函数进行串口的初始化
	*　　　　　   /n本函数提供了一些常用的串口参数设置,若需要自行设置详细的DCB参数,可使用重载函数
	*           /n本串口类析构时会自动关闭串口,无需额外执行关闭串口
	*  @see:
	*/
	bool InitPort(UINT  portNo = 1, UINT  baud = CBR_115200, char  parity = 'N', UINT  databits = 8, UINT  stopsbits = 1, DWORD dwCommEvents = EV_RXCHAR);

	/** 串口初始化函数
	*
	*  本函数提供直接根据DCB参数设置串口参数
	*  @param:  UINT portNo
	*  @param:  const LPDCB & plDCB
	*  @return: bool  初始化是否成功
	*  @note:   本函数提供用户自定义地串口初始化参数
	*  @see:
	*/
	bool InitPort(UINT  portNo, const LPDCB& plDCB);

	/** 开启监听线程
	*
	*  本监听线程完成对串口数据的监听,并将接收到的数据打印到屏幕输出
	*  @return: bool  操作是否成功
	*  @note:   当线程已经处于开启状态时,返回flase
	*  @see:
	*/
	bool OpenListenThread();

	/** 关闭监听线程
	*
	*
	*  @return: bool  操作是否成功
	*  @note:   调用本函数后,监听串口的线程将会被关闭
	*  @see:
	*/
	bool CloseListenTread();

	/** 向串口写数据
	*
	*  将缓冲区中的数据写入到串口
	*  @param:  unsigned char * pData 指向需要写入串口的数据缓冲区
	*  @param:  unsigned int length 需要写入的数据长度
	*  @return: bool  操作是否成功
	*  @note:   length不要大于pData所指向缓冲区的大小
	*  @see:
	*/
	bool WriteData(unsigned char* pData, unsigned int length);

	/** 获取串口缓冲区中的字节数
	*
	*
	*  @return: UINT  操作是否成功
	*  @note:   当串口缓冲区中无数据时,返回0
	*  @see:
	*/
	UINT GetBytesInCOM();

	/** 读取串口接收缓冲区中一个字节的数据
	*
	*
	*  @param:  char & cRecved 存放读取数据的字符变量
	*  @return: bool  读取是否成功
	*  @note:
	*  @see:
	*/
	bool ReadChar(char &cRecved);
	void ClosePort();
	HANDLE  m_hComm;
private:

	/** 打开串口
	*
	*
	*  @param:  UINT portNo 串口设备号
	*  @return: bool  打开是否成功
	*  @note:
	*  @see:
	*/
	bool openPort(UINT  portNo);

	/** 关闭串口
	*
	*
	*  @return: void  操作是否成功
	*  @note:
	*  @see:
	*/
	//void ClosePort();

	/** 串口监听线程
	*
	*  监听来自串口的数据和信息
	*  @param:  void * pParam 线程参数
	*  @return: UINT WINAPI 线程返回值
	*  @note:
	*  @see:
	*/
	static UINT WINAPI ListenThread(void* pParam);

private:

	/** 串口句柄 */
	//HANDLE  m_hComm;

	/** 线程退出标志变量 */
	static bool s_bExit;

	/** 线程句柄 */
	volatile HANDLE    m_hListenThread;

	/** 同步互斥,临界区保护 */
	CRITICAL_SECTION   m_csCommunicationSync;       //!< 互斥操作串口  

};



/** 线程退出标志 */
bool CSerialPort::s_bExit = false;
/** 当串口无数据时,sleep至下次查询间隔的时间,单位:秒 */
const UINT SLEEP_TIME_INTERVAL = 1;

CSerialPort::CSerialPort(void)
	: m_hListenThread(INVALID_HANDLE_VALUE)
{
	m_hComm = INVALID_HANDLE_VALUE;
	m_hListenThread = INVALID_HANDLE_VALUE;

	InitializeCriticalSection(&m_csCommunicationSync);

}

CSerialPort::~CSerialPort(void)
{
	CloseListenTread();
	ClosePort();
	DeleteCriticalSection(&m_csCommunicationSync);
}

bool CSerialPort::InitPort(UINT portNo /*= 1*/, UINT baud /*= CBR_9600*/, char parity /*= 'N'*/,
	UINT databits /*= 8*/, UINT stopsbits /*= 1*/, DWORD dwCommEvents /*= EV_RXCHAR*/)
{

	/** 临时变量,将制定参数转化为字符串形式,以构造DCB结构 */
	char szDCBparam[50];
	sprintf_s(szDCBparam, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopsbits);

	/** 打开指定串口,该函数内部已经有临界区保护,上面请不要加保护 */
	if (!openPort(portNo))
	{
		std::cout << "openPort fail 11!" << std::endl;
		return false;
	}

	/** 进入临界段 */
	EnterCriticalSection(&m_csCommunicationSync);

	/** 是否有错误发生 */
	BOOL bIsSuccess = TRUE;

	/** 在此可以设置输入输出的缓冲区大小,如果不设置,则系统会设置默认值.
	*  自己设置缓冲区大小时,要注意设置稍大一些,避免缓冲区溢出
	*/
	/*if (bIsSuccess )
	{
	bIsSuccess = SetupComm(m_hComm,10,10);
	}*/

	/** 设置串口的超时时间,均设为0,表示不使用超时限制 */
	COMMTIMEOUTS  CommTimeouts;
	CommTimeouts.ReadIntervalTimeout = 0;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.ReadTotalTimeoutConstant = 0;
	CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	CommTimeouts.WriteTotalTimeoutConstant = 0;
	if (bIsSuccess)
	{
		bIsSuccess = SetCommTimeouts(m_hComm, &CommTimeouts);
	}

	DCB  dcb;
	if (bIsSuccess)
	{
		// 将ANSI字符串转换为UNICODE字符串  
		DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, NULL, 0);
		wchar_t *pwText = new wchar_t[dwNum];
		if (!MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, pwText, dwNum))
		{
			bIsSuccess = TRUE;
		}

		/** 获取当前串口配置参数,并且构造串口DCB参数 */
		bIsSuccess = GetCommState(m_hComm, &dcb) && BuildCommDCB(pwText, &dcb);
		/** 开启RTS flow控制 */
		dcb.fRtsControl = RTS_CONTROL_ENABLE;

		/** 释放内存空间 */
		delete[] pwText;
	}

	if (bIsSuccess)
	{
		/** 使用DCB参数配置串口状态 */
		bIsSuccess = SetCommState(m_hComm, &dcb);
	}

	/**  清空串口缓冲区 */
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	/** 离开临界段 */
	LeaveCriticalSection(&m_csCommunicationSync);

	return bIsSuccess == TRUE;
}

bool CSerialPort::InitPort(UINT portNo, const LPDCB& plDCB)
{
	/** 打开指定串口,该函数内部已经有临界区保护,上面请不要加保护 */
	if (!openPort(portNo))
	{
		//printf("penPor failed");
		return false;
	}

	/** 进入临界段 */
	EnterCriticalSection(&m_csCommunicationSync);

	/** 配置串口参数 */
	if (!SetCommState(m_hComm, plDCB))
	{
		return false;
	}

	/**  清空串口缓冲区 */
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	/** 离开临界段 */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}

void CSerialPort::ClosePort()
{
	/** 如果有串口被打开，关闭它 */
	if (m_hComm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
	}
}

bool CSerialPort::openPort(UINT portNo)
{
	/** 进入临界段 */
	EnterCriticalSection(&m_csCommunicationSync);

	/** 把串口的编号转换为设备名 */
	char szPort[50];
	if(portNo <10)
	sprintf_s(szPort, "COM%d", portNo);
	else
		sprintf_s(szPort, "\\\\.\\COM%d", portNo);
	/** 打开指定的串口 */
	m_hComm = CreateFileA(szPort,  /** 设备名,COM1,COM2等 */
		GENERIC_READ | GENERIC_WRITE, /** 访问模式,可同时读写 */
		0,                            /** 共享模式,0表示不共享 */
		NULL,                         /** 安全性设置,一般使用NULL */
		OPEN_EXISTING,                /** 该参数表示设备必须存在,否则创建失败 */
		0,
		0);

	/** 如果打开失败，释放资源并返回 */
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		//DWORD err = GetLastError();
		//printf("err:%d\n", err);
		LeaveCriticalSection(&m_csCommunicationSync);
		//fprintf(stdout, "CreateFileA failed %d\n", m_hComm);

		return false;
	}

	/** 退出临界区 */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}

bool CSerialPort::OpenListenThread()
{
	/** 检测线程是否已经开启了 */
	if (m_hListenThread != INVALID_HANDLE_VALUE)
	{
		/** 线程已经开启 */
		return false;
	}

	s_bExit = false;
	/** 线程ID */
	UINT threadId;
	/** 开启串口数据监听线程 */
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);
	if (!m_hListenThread)
	{
		return false;
	}
	/** 设置线程的优先级,高于普通线程 */
	if (!SetThreadPriority(m_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))
	{
		return false;
	}

	return true;
}

bool CSerialPort::CloseListenTread()
{
	if (m_hListenThread != INVALID_HANDLE_VALUE)
	{
		/** 通知线程退出 */
		s_bExit = true;

		/** 等待线程退出 */
		Sleep(10);

		/** 置线程句柄无效 */
		CloseHandle(m_hListenThread);
		m_hListenThread = INVALID_HANDLE_VALUE;
	}
	return true;
}

UINT CSerialPort::GetBytesInCOM()
{
	DWORD dwError = 0;  /** 错误码 */
	COMSTAT  comstat;   /** COMSTAT结构体,记录通信设备的状态信息 */
	memset(&comstat, 0, sizeof(COMSTAT));

	UINT BytesInQue = 0;
	/** 在调用ReadFile和WriteFile之前,通过本函数清除以前遗留的错误标志 */
	if (ClearCommError(m_hComm, &dwError, &comstat))
	{
		BytesInQue = comstat.cbInQue; /** 获取在输入缓冲区中的字节数 */
	}

	return BytesInQue;
}

UINT WINAPI CSerialPort::ListenThread(void* pParam)
{
	/** 得到本类的指针 */
	CSerialPort *pSerialPort = reinterpret_cast<CSerialPort*>(pParam);
	char cRecved = 0x00;
	int i = 0;
	// 线程循环,轮询方式读取串口数据  
	while (!pSerialPort->s_bExit)
	{
		UINT BytesInQue = pSerialPort->GetBytesInCOM();
		/** 如果串口输入缓冲区中无数据,则休息一会再查询 */
		if (BytesInQue == 0)
		{
			Sleep(SLEEP_TIME_INTERVAL);
			
			continue;
		}
		else
			fprintf(stderr, "BytesInQue: %d\n", BytesInQue);

		/** 读取输入缓冲区中的数据并输出显示 */
		cRecved = 0x00;
		//i = 0;
		
		//memset(ICCID, 0, sizeof(ICCID));
		//memset(IMEI, 0, sizeof(IMEI));
		do
		{
			cRecved = 0x00;
			if (pSerialPort->ReadChar(cRecved) == true)
			{
				recv_at[i] = cRecved;

				i++;
				//std::cout << cRecved;
				continue;
			}
			
		} while (--BytesInQue);
		/** 清空串口缓冲区 */
		//PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		//recv_at[i] = '\0';
		fprintf(stderr, "i %d ;recv_at: %s\n", i,recv_at);
		//std::cout << "sadf\r" << std::endl;
		//for (int i = 0; i <20; i++)
		//std::cout << recv_at[i] << std::endl;
		//for(int i=0;i <strlen(recv_at);i++)
		//	printf_s("%c", recv_at[i]);
		//std::cout << i << std::endl;
		//i = 0;
		//std::cout << i << std::endl;
		if (at_recv_count == 1)
		{
			if (i < 9)
				continue;
			if (i == 9)
			{
				recv_at[i] = '\0';
				memset(recv_at, 0, 128);
				i = 0;
			}
		}
		else if (at_recv_count == 2)
		{
			if (i < 49)
				continue;
			if (i == 49)
			{
				recv_at[i] = '\0';

				ret = strstr(recv_at, "MICCID:");
				if (ret != NULL)
				{

					strncpy_s(ICCID, ret + 8, 21);
					ret = NULL;
					ICCID[22] = '\0';

				}

				/** 清空串口缓冲区 */
				//PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
				memset(recv_at, 0, 128);
				i = 0;
			}
		}
		else if (at_recv_count == 3)
		{
			if (i < 42)
				continue;
			if (i == 42 )
			{
				recv_at[i] = '\0';
			}
		
		
			ret = strstr(recv_at, "CGSN:");
			if (ret != NULL)
			{
				strncpy_s(IMEI, ret + 6, 16);
				ret = NULL;
				IMEI[17] = '\0';
			}
			memset(recv_at, 0, 128);
			i = 0;
		}

	}

	return 0;
}

bool CSerialPort::ReadChar(char &cRecved)
{
	BOOL  bResult = TRUE;
	DWORD BytesRead = 0;
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/** 临界区保护 */
	EnterCriticalSection(&m_csCommunicationSync);

	/** 从缓冲区读取一个字节的数据 */
	bResult = ReadFile(m_hComm, &cRecved, 1, &BytesRead, NULL);
	if ((!bResult))
	{
		/** 获取错误码,可以根据该错误码查出错误原因 */
		DWORD dwError = GetLastError();
		fprintf(stderr, "GetLastError: %d\n", dwError);
		/** 清空串口缓冲区 */
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return false;
	}

	/** 离开临界区 */
	LeaveCriticalSection(&m_csCommunicationSync);

	return (BytesRead == 1);

}

bool CSerialPort::WriteData(unsigned char* pData, unsigned int length)
{
	BOOL   bResult = TRUE;
	DWORD  BytesToSend = 0;
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/** 临界区保护 */
	EnterCriticalSection(&m_csCommunicationSync);

	/** 向缓冲区写入指定量的数据 */
	bResult = WriteFile(m_hComm, pData, length, &BytesToSend, NULL);
	if (!bResult)
	{
		DWORD dwError = GetLastError();
		/** 清空串口缓冲区 */
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return false;
	}

	/** 离开临界区 */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}
BOOL BTCGetDigPort(PCHAR pPort, PCHAR pName)
{
	HDEVINFO  DeviceInfoSet;
	SP_DEVINFO_DATA DeviceInfoData;
	DWORD  index;
	BOOL   bReult = FALSE;
	if (!pPort || !pName)
	{
		return FALSE;
	}
	// Buffer must over MAX_PATH(127 Byte)

	DeviceInfoSet = SetupDiGetClassDevs((LPGUID)&GUID_DEVCLASS_PORTS, // All Classes
		0,
		0,
		DIGCF_PRESENT);//DIGCF_ALLCLASSES); // All devices
	if (DeviceInfoSet == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	for (index = 0; SetupDiEnumDeviceInfo(DeviceInfoSet, index, &DeviceInfoData); index++)
	{
		char *  buffer = NULL;
		DWORD buffersize = 0;

		while (!SetupDiGetDeviceRegistryProperty(IN  DeviceInfoSet,
			IN  &DeviceInfoData,
			IN SPDRP_FRIENDLYNAME,
			OUT NULL,
			OUT(PBYTE)buffer,
			IN buffersize,
			OUT &buffersize))
		{
			if (GetLastError() == ERROR_INVALID_DATA)
			{

				break;
			}
			else if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				if (buffer)
				{
					LocalFree(buffer);
				}
				buffer = (char*)LocalAlloc(LPTR, buffersize);
			}
			else
			{

				goto cleanup_DeviceInfo;
			}
		}
		if (!buffer)
		{
			continue;
		}
		std::cout << "BTCGetDigPort success1 !" << std::endl;
		std::cout << buffer << std::endl;
		std::cout << "BTCGetDigPort success2 !" << std::endl;
		if (strstr((char *)buffer,(char *) pName))
		{
		//DBG_PRINT("Name: %s", buffer, 0);
			DWORD i = strlen(buffer);
			DWORD dwStart, dwEnd;
			PCHAR pStart;
			while (buffer[i] != '(')
			{
				if (buffer[i] == ')')
				{
					dwEnd = i;
				}
				i--;
			}
			dwStart = i + 1;
			pStart = buffer + dwStart;
			memcpy(pPort, pStart, dwEnd - dwStart);
			pPort[dwEnd - dwStart] = '/0';
			//DBG_PRINT("pPort: %s", pPort, 0);
			bReult = TRUE;
			if (buffer)
			{
				LocalFree(buffer);
				buffer = NULL;
			}
			goto cleanup_DeviceInfo;
		}/* End find our diag port */
		if (buffer)
		{
			LocalFree(buffer);
			buffer = NULL;
		}
	}
cleanup_DeviceInfo:
	SetupDiDestroyDeviceInfoList(DeviceInfoSet);
	return bReult;
}
void SetColor(unsigned short forecolor = 4, unsigned short backgroudcolor = 0)
{
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE); //获取缓冲区句柄
	SetConsoleTextAttribute(hCon, forecolor | backgroudcolor); //设置文本及背景色
}
static FILE *openFile(const char *outfile)
{
	FILE *fp;
	errno_t err;
	if (outfile == NULL || (outfile[0] == '-' && outfile[1] == '\0')) {
		fp = stdout;
	}
	else {
		err = fopen_s(&fp,outfile, "wb");
		if (err == NULL) {
			fprintf(stderr, "Failed to create file: %s\n", outfile);
			//perror(NULL);
			exit(EXIT_FAILURE);
		}
	}

	return fp;
}
#if 1
static void writeANSI_margin(FILE* fp, int realwidth,
	char* buffer, int buffer_s,
	char* white, int white_s)
{
	int y;
	int i;
	//strcpy_s(buffer, buffer_s, white );
	//memset(buffer + white_s, ' ', realwidth * 2);
	//strcpy_s(buffer + white_s + realwidth * 2, buffer_s - white_s - realwidth * 2, "\033[0m\n"); // reset to default colors
	//memset(buffer, ' ', realwidth * 2);
	//buffer[realwidth * 2 + 1] = '\0';
	SetColor(0, 0xff);
	for (y = 0; y<margin; y++) {
		for(i =0;i< realwidth * 2;i++)
		fputc(' ', fp);

		fputc('\n', fp);
	}

	SetColor(0, 7);
}

static int writeANSI(QRcode *qrcode, const char *outfile)
{
	FILE *fp;
	unsigned char *row, *p;
	int x, y;
	int realwidth;
	int last;

	char *white, *black, *buffer;
	int white_s, black_s, buffer_s;

	if (image_type == ANSI256_TYPE) {
		/* codes for 256 color compatible terminals */
		white = "\033[48;5;231m";
		white_s = 11;
		black = "\033[48;5;16m";
		black_s = 10;
	}
	else {
		white = "\033[47m";
		white_s = 5;
		black = "\033[40m";
		black_s = 5;
	}

	ssize = 1;

	fprintf(stderr, "outfile %s.\n", outfile);
	fp = openFile(outfile);

	realwidth = (qrcode->width + margin * 2) * ssize;
	buffer_s = (realwidth * white_s) * 2;
	buffer = (char *)malloc(buffer_s);
	if (buffer == NULL) {
		fprintf(stderr, "Failed to allocate memory.\n");
		exit(EXIT_FAILURE);
	}

	/* top margin */
	writeANSI_margin(fp, realwidth, buffer, buffer_s, white, white_s);
	SetColor(0, 0xff);
	/* data */
	p = qrcode->data;
	for (y = 0; y<qrcode->width; y++) {
		row = (p + (y*qrcode->width));

		//bzero(buffer, buffer_s);
		memset(buffer,0, buffer_s);
		//strcpy_s(buffer, buffer_s,white );
		SetColor(0, 0xff);
		for (x = 0; x<margin; x++) {
			//strcat_s(buffer, buffer_s, "  ");
			//fputs("  ", fp);
			fputs("  ", stdout);
		}
		//SetColor(0, 7);
		last = 0;

		for (x = 0; x<qrcode->width; x++) {
			if (*(row + x) & 0x1) {
				if (last != 1) {
					SetColor(0, 0);
					//strcat_s(buffer, buffer_s, black);
					last = 1;
				}
			}
			else {
				if (last != 0) {
					SetColor(0, 0xff);
					//strcat_s(buffer, buffer_s, white );
					last = 0;
				}
			}
			//strcat_s(buffer, buffer_s,"  ");
			fputs("  ", fp);
			//SetColor(0, 7);
		}

		if (last != 0) {
			SetColor(0, 0xff);
			//strcat_s(buffer, buffer_s,white);
		}
		for (x = 0; x<margin; x++) {
			//strcat_s(buffer, buffer_s,"  ");
			fputs("  ", fp);
		}
		//strcat_s(buffer, buffer_s,"\033[0m\n");
		fputc('\n', fp);
		//SetColor(0, 7);

	}
	//SetColor(0, 7);
	/* bottom margin */
	writeANSI_margin(fp, realwidth, buffer, buffer_s, white, white_s);
	//fprintf(stderr, "111111111111111.\n");
	//fclose(fp);
	
	  margin = -1;
	free(buffer);
	//fprintf(stderr, "\r\n");
	return 0;
}
//#else
static void writeUTF8_margin(FILE* fp, int realwidth,
	const char* white, const char *reset,
	int use_ansi)
{
	int x, y;

	for (y = 0; y < margin / 2; y++) {
		//fputs(white, fp);

		std::cout << white;
		for (x = 0; x < realwidth; x++)
			//fputs("\342\226\210", fp);
			std::cout << "\342\226\210";


		//fputs(reset, fp);
		std::cout << reset;
		fputc('\n', fp);

	}
}

static int writeUTF8(QRcode *qrcode, const char *outfile, int use_ansi)
{
	FILE *fp;
	int x, y;
	int realwidth;
	const char *white, *reset;

	if (use_ansi) {
		white = "\033[40;37;1m";
		reset = "\033[0m";
	}
	else {
		white = "";
		reset = "";
	}

	fp = openFile(outfile);

	realwidth = (qrcode->width + margin * 2);

	/* top margin */
	writeUTF8_margin(fp, realwidth, white, reset, use_ansi);

	/* data */
	for (y = 0; y < qrcode->width; y += 2) {
		unsigned char *row1, *row2;
		row1 = qrcode->data + y*qrcode->width;
		row2 = row1 + qrcode->width;

		fputs(white, fp);

		for (x = 0; x < margin; x++)
		//	fputs("\342\226\210", fp);
			std::cout << "\342\226\210";
		for (x = 0; x < qrcode->width; x++) {
			if (row1[x] & 1) {
				if (y < qrcode->width - 1 && row2[x] & 1) {
					fputc(' ', fp);
				}
				else {
					//;// fputs("\342\226\204", fp);
					//std::cout << WHITE << "\342\226\204";

				}
			}
			else {
				if (y < qrcode->width - 1 && row2[x] & 1) {
					//fputs("\342\226\200", fp);
					std::cout << "\342\226\200";
				}
				else {
				//	fputs("\342\226\210", fp);
					std::cout << "\342\226\210";
				}
			}
		}

		for (x = 0; x < margin; x++)
			//fputs("\342\226\210", fp);
			std::cout << "\342\226\210";
		//fputs(reset, fp);
		//fputc('\n', fp);
		std::cout << reset;

	}
	std::cout << '\n';
	/* bottom margin */
	writeUTF8_margin(fp, realwidth, white, reset, use_ansi);

	fclose(fp);

	return 0;
}
#endif
static void writeASCII_margin(FILE* fp, int realwidth, char* buffer, int buffer_s, int invert)
{
	int y, h;

	h = margin;

	memset(buffer, (invert ? '#' : ' '), realwidth);
	buffer[realwidth] = '\n';
	buffer[realwidth + 1] = '\0';
	for (y = 0; y<h; y++) {
		fputs(buffer, fp);
	}
}

static int writeASCII(QRcode *qrcode, const char *outfile, int invert)
{
	FILE *fp;
	unsigned char *row;
	int x, y;
	int realwidth;
	char *buffer, *p;
	int buffer_s;
	char black = '#';
	char white = ' ';

	if (invert) {
		black = ' ';
		white = '#';
	}

	ssize = 1;

	fp = openFile(outfile);

	realwidth = (qrcode->width + margin * 2) * 2;
	buffer_s = realwidth + 2;
	buffer = (char *)malloc(buffer_s);
	if (buffer == NULL) {
		fprintf(stderr, "Failed to allocate memory.\n");
		exit(EXIT_FAILURE);
	}

	/* top margin */
	writeASCII_margin(fp, realwidth, buffer, buffer_s, invert);

	/* data */
	for (y = 0; y<qrcode->width; y++) {
		row = qrcode->data + (y*qrcode->width);
		p = buffer;

		memset(p, white, margin * 2);
		p += margin * 2;

		for (x = 0; x<qrcode->width; x++) {
			if (row[x] & 0x1) {
				*p++ = black;
				*p++ = black;
			}
			else {
				*p++ = white;
				*p++ = white;
			}
		}

		memset(p, white, margin * 2);
		p += margin * 2;
		*p++ = '\n';
		*p++ = '\0';
		fputs(buffer, fp);
	}

	/* bottom margin */
	writeASCII_margin(fp, realwidth, buffer, buffer_s, invert);

	fclose(fp);
	free(buffer);

	return 0;
}

static QRcode *encode(const unsigned char *intext, int length)
{
	QRcode *code=NULL;

	if (micro) {
		if (eightbit) {
			;// code = QRcode_encodeDataMQR(length, intext, version, level);
		}
		else {
			;// code = QRcode_encodeStringMQR((char *)intext, version, level, hint, casesensitive);
		}
	}
	else {
		if (eightbit) {
			;// code = QRcode_encodeData(length, intext, version, level);
		}
		else {
			code = QRcode_encodeString((char *)intext, version, level, hint, casesensitive);
		}
	}

	return code;
}

static void qrencode(const unsigned char *intext, int length, const char *outfile)
{
	QRcode *qrcode;

	qrcode = encode(intext, length);
	if (qrcode == NULL) {
		perror("Failed to encode the input data");
		exit(EXIT_FAILURE);
	}

	if (verbose) {
		fprintf(stderr, "File: %s, Version: %d\n", (outfile != NULL) ? outfile : "(stdout)", qrcode->version);
	}
	image_type = ANSI_TYPE;
	margin = 1;
	level = QR_ECLEVEL_L;
	hint = QR_MODE_8;
	switch (image_type) {
	case PNG_TYPE:
		;//writePNG(qrcode, outfile);
		break;
	case EPS_TYPE:
		;// writeEPS(qrcode, outfile);
		break;
	case SVG_TYPE:
		;//	writeSVG(qrcode, outfile);
		break;
	case ANSI_TYPE:
	case ANSI256_TYPE:
		 writeANSI(qrcode, outfile);
		break;
	case ASCIIi_TYPE:
		 writeASCII(qrcode, outfile, 1);
		break;
	case ASCII_TYPE:
		writeASCII(qrcode, outfile, 0);
		break;
	case UTF8_TYPE:
		 writeUTF8(qrcode, outfile, 0);
		break;
	case ANSIUTF8_TYPE:
		;// writeUTF8(qrcode, outfile, 1);
		break;
	default:
		fprintf(stderr, "Unknown image type.\n");
		exit(EXIT_FAILURE);
	}
	//std::cout << "QRcode_free" << std::endl;
	QRcode_free(qrcode);
	return;
}



int _tmain(int argc, _TCHAR* argv[])
{
	CSerialPort mySerialPort;
	 char str[128] = {0,};
	 char pport[128] = { 0, };
	 char pname[128] =  "MTK123123";
	unsigned char tpname[128] = "MTK123123102301283123";
	 //printf_s("InitPort %s\r\n", pname);
	// fprintf(stdout,"11231\n");
	 //SetColor(0, 0xff);
	// std::cout<<"olored hello world for windows!\n";
	 //SetColor(0x76, 0x76);
	// SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	 //SetColor(0,0);
	 //fputs("66666666",stdout);
	//std::cout << "InitPort(22)" << std::endl;
	//	 SetColor(0, 7);
	//std::cout << pname << std::endl;
	//BTCGetDigPort(pport, pname);

	int temp;
	std::cout << "请输入MTK COM序号 ：" << std::endl;

	std::cin >> temp;

	while (1)
	{
		//fprintf(stderr, "temp =%d.\n", temp);
		if (!mySerialPort.InitPort(temp))
		{
			std::cout << "串口打开失败,请检查硬件连接！" << std::endl;
			system("pause");
			exit(EXIT_FAILURE);
		}
		else
		{
			std::cout << "打开成功!" << std::endl;
		}

		if (!mySerialPort.OpenListenThread())
		{
			std::cout << "串口接收线程打开失败 !" << std::endl;
			system("pause");
			exit(EXIT_FAILURE);
		}
		else
		{
			std::cout << "OpenListenThread success !" << std::endl;
		}
		Sleep(2000);
		memset(ICCID, 0, 48);
		memset(IMEI, 0, 48);
		at_recv_count = 1;
		sprintf_s(str,"%s\r\n","AT");
		mySerialPort.WriteData((unsigned char *)str,4);
		memset(str, 0, 128);
		Sleep(1000);
		/**  清空串口缓冲区 */
		//PurgeComm(mySerialPort.m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
		//std::cout << "AT WAKEUP" << std::endl;
		//sprintf_s(str, "%s\r\n", "AT");
		//mySerialPort.WriteData((unsigned char *)str, 4);
		//Sleep(1000);
		//sprintf_s(str, "%s\r\n", "AT+CGSN=1");
		//mySerialPort.WriteData((unsigned char *)str, 11);
		//Sleep(1000);

		//sprintf_s(str, "%s\r\n", "AT*MICCID");
		//mySerialPort.WriteData((unsigned char *)str, 11);
		//Sleep(1000);
		at_recv_count = 2;
		sprintf_s(str, "%s\r\n", "AT*MICCID");
		mySerialPort.WriteData((unsigned char *)str, 11);
		memset(str, 0, 128);
		Sleep(1000);
		printf("ICCID:%s\r\n", ICCID);

		at_recv_count = 3;
		sprintf_s(str, "%s\r\n", "AT+CGSN=1");
		mySerialPort.WriteData((unsigned char *)str, 11);
		memset(str, 0, 128);
		Sleep(2000);
		printf("IMEI:%s\r\n", IMEI);

		sprintf_s(str, "%s,%s", ICCID, IMEI);
		printf("str:%s\r\n",str);
		//qrencode((const unsigned char *)str, strlen(str), NULL);
		//printf("len %d,ICCID %s.\n", strlen(ICCID),ICCID);
		if(strlen(ICCID) != 0 && strlen(IMEI)!=0)
		//qrencode((const unsigned char *)ICCID, (int)strlen(ICCID), NULL);
			qrencode((const unsigned char *)str, (int)strlen(str), NULL);
		else{
			fprintf(stderr, "未能获取ICCID，请检查硬件.\n");
			system("pause");
		}
		memset(ICCID,0,48);
		//std::cout << " SUCCESS" << std::endl;
		//fprintf(stderr, "333333333333333333.\n");
		if (!mySerialPort.CloseListenTread())
		{
			std::cout << "串口接收线程关闭失败! 请重启程序!" << std::endl;
			system("pause");
			exit(EXIT_FAILURE);
		}
		else
		{
			 //std::cout << "CloseListenTread success !" << std::endl;
			 //std::cout << " 先连接下一块设备，再按键继续" << std::endl;
			printf("先连接下一块设备，再按键继续.\r\n");
		}
		mySerialPort.ClosePort();
		//std::cout << " 先连接下一块设备，再按键继续" << std::endl;
		system("pause");
	}
	std::cout << " SUCCESS" << std::endl;

	//Sleep(1000);
	//QRcode_encodeString("123341",0, QR_ECLEVEL_Q, QR_MODE_8,1);
	//qrencode((const unsigned char *)ICCID,strlen(ICCID),NULL);
	//std::cout << "ICCID SUCCESS" << std::endl;
	//Sleep(1000);
	//qrencode((const unsigned char *)IMEI, strlen(IMEI), NULL);
	//std::cout << "IMEI SUCCESS" << std::endl;
	//qrencode(tpname, 21, NULL);
	//Sleep(1000);
	//std::cout << "QRcode_encodeString success !" << std::endl;

	//int temp2;
	//std::cin >> temp2;
	getchar();
	getchar();
	return 0;
}
