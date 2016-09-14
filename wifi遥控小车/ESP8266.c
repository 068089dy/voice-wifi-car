//ע�⣺����wifi�ʹ��ڶ�ռ���˽ϴ���ڴ�ռ䣬Ŀǰ�Ѿ������ܼ����ڴ��������������쳣��������Զ���뼸��Ȼ�����ص���Ƭ����
//tip�������ܲ�Ҫ�ں��������ϴ���ڴ���䣬����ֱ���õ�������ȫ�ֱ����ķ�ʽ����

//�� arduinoESP8266�ⲿ�ֺ�����C++��ֲ��C����������51��ARM��Cƽ̨����
//�Է���ֵ��ԭ����true or false ��Ϊ����int�� 0��ʾʧ�� ������ʾ�ɹ�������ԭ�򷵻�
#include <stdlib.h>
#include "serialportAPI.h"
#include "ESP8266.h"
#include "stringAPIext.h"
#include "uart.h"

volatile unsigned long sys_tick = 0;

char data_rec[RECV_BUF_SIZE];

static uint32_t recvPkg(uint8_t *buffer, uint32_t buffer_size, uint32_t *data_len, uint32_t timeout, uint8_t *coming_mux_id);
static int eATUART(uint32_t baud);
static int eATRST(void);
static int eAT(void);
static int sATCWJAP(const char *ssid, const char *pwd);
static int eATCWQAP(void);
static int eATCWSMARTSTART(uint8_t type,char *link_msg);
static int eATCWSMARTSTOP(void);
static int qATCWMODE(uint8_t *mode);
static int sATCWMODE(uint8_t mode);
static int sATCIPMUX(uint8_t mode);

static int recvFindAndFilter(const char *target, const char *begin, const char *end, char *data_rec,uint32_t timeout);
static int recvFind(const char *target,uint32_t timeout);
static int recvString(char *rec_data, const char *target, uint32_t timeout);
static int recvString2(char *rec_data, const char *target1, const char *target2, uint32_t timeout);
static int eATCIPSTATUS(char *data_rec);
static int sATCIPSTARTSingle(const char *type, const char *addr, uint32_t port);
static int sATCIPSENDSingle(const uint8_t *buffer, uint32_t len);
static int recvString3(char *rec_data, const char *target1, const char *target2, const char *target3, uint32_t timeout);

/*******************************************************************************
  �� �� �� ��AutoLink
  �������� ���Զ����ӣ�ǰ10s�Զ����ӣ�������ʧ�������smartlinkģʽ30s������Ȼʧ��
             ���ٴλص��Զ����ӣ�ֱ�����ӳɹ�
  ��    �� ����
  ��    �� ����
*******************************************************************************/
void AutoLink(void)
{
  int status = STATUS_LOSTIP;
  while (status != STATUS_GETIP)
  {
    uint32_t start_time = millis();
    DBG("start auto link");
    //10s�Զ�����ʱ��
    while ((millis() - start_time < 10000) && status != STATUS_GETIP)
    {
      status = getSystemStatus();
			delay(1000);
    }

    //����ʧ�ܽ���smartlinkģʽ 30s
    if (status != STATUS_GETIP)
    {
      char link_msg[RECV_BUF_SIZE];
      DBG("start smartlink");
      stopSmartLink();

      if (1 == smartLink((uint8_t)AIR_LINK, link_msg))
      {
        DBG(link_msg);
        start_time = millis();//�ȴ���ȡIP
        while ((millis() - start_time < 5000) && status != STATUS_GETIP)
        {
          status = getSystemStatus();
          delay(500);
        }
      } else
      {
        stopSmartLink();
        delay(500);
        DBG("link AP fail");
				restart();
      }
    }
  }
  DBG("link AP OK");
}

int WifiInitSmart(const char *addr, uint32_t port)
{
	DBG("Restart");
  while(0 == restart());
  while(0 == setOprToStationSoftAP());
  AutoLink();
  while(0 == disableMUX());
creattcp0:
  if (createTCP(addr, port)) {//��������
    DBG("create tcp ok\r\n");
  } else {
    DBG("create tcp err\r\n");
    delay(2000);
    goto creattcp0;
  }
  delay(1000);
  if (getSystemStatus() == STATUS_GETLINK)
    return 1;
  else
    return 0;
}

int WifiInit(const char *ssid, const char *psd, const char *addr, uint32_t port)
{
	DBG("Restart");
  while(0 == restart());
  while(0 == setOprToStationSoftAP());
	DBG("link AP...\r\n");
	DBG("SSID:");
	DBG(ssid);
	DBG("password:");
	DBG(psd);	
  while(0 == joinAP(ssid,psd))
	{
		DBG("link AP fail\r\n");
		DBG("link AP...\r\n");
		DBG("SSID:");
		DBG(ssid);
		DBG("password:");
		DBG(psd);	
	}
	DBG("link AP OK");
	
  while(0 == disableMUX());
creattcp0:
  if (createTCP(addr, port)) {//��������
    DBG("create tcp ok\r\n");
  } else {
    DBG("create tcp err\r\n");
    delay(2000);
    goto creattcp0;
  }
  delay(1000);
  if (getSystemStatus() == STATUS_GETLINK)
    return 1;
  else
    return 0;
}

void ClearRecBuf(void)
{
	data_rec[0] = '\0';
}

//��1ms��ʱ�����ã��ö�ʱ����Ҫ�нϸߵ����ȼ�
void timer1msINT(void)
{
	sys_tick++;
}

unsigned long millis(void)
{
	return sys_tick;
}

//ms ��ʱ���������� msֵ > 10
void delay(unsigned int ms)
{
	unsigned long start = millis();
	while(millis() - start <= ms);
}

int SetBaud(uint32_t baud)
{
	return eATUART(baud);
}

int restart(void)
{
    unsigned long start;
    if (eATRST()) {
        delay(2000);
        start = millis();
        while (millis() - start < 3000) {
            if (eAT()) {
                delay(1500); /* Waiting for stable */
                return 1;
            }
            delay(100);
        }
    }
    return 0;
}

//�л���AP+stationģʽ
int setOprToStationSoftAP(void)
{
    uint8_t mode=0xff;
    if (!qATCWMODE(&mode)) {
        return 0;
    }
    if (mode == 3) {
        return 1;
    } else {
        if (sATCWMODE(3) && restart()) {
            return 1;
        } else {
            return 0;
        }
    }
}

int joinAP(const char *ssid, const char *pwd)
{
    return sATCWJAP(ssid, pwd);
}

int leaveAP(void)
{
    return eATCWQAP();
}

//ע��link_msg�Ĵ�СҪ�㹻����ֹ�ڴ����,����SSID��pwd��Ϣ
int smartLink(uint8_t  type,char *link_msg)
{
	return eATCWSMARTSTART(type,link_msg);
}

int stopSmartLink(void)
{
	return eATCWSMARTSTOP();
}

int getSystemStatus(void)
{
	int index;
	//  char list[16];
	char *list = malloc(16);//��ARM�����£����ʹ��list[16]�ķ�ʽ������ֳ�ջʱ�����쳣������malloc��ʽ
  
    eATCIPSTATUS(list);
	
	index = StringIndex(list,"STATUS:");
	if (index != -1) {
		int status;
		index += 7;
		status = (int)(list[index]);
		free(list);
		return (status - 0x30);//����״̬
	}else
	{
		free(list);
		return 0;
	}
}

int disableMUX(void)
{
   return sATCIPMUX(0);
}

int createTCP(const char *addr, uint32_t port)
{
   return sATCIPSTARTSingle("TCP", addr, port);
}

int send(const uint8_t *buffer, uint32_t len)
{
   return sATCIPSENDSingle(buffer, len);
}

int recv(uint8_t *buffer, uint32_t buffer_size, uint32_t timeout)
{
   return recvPkg(buffer, buffer_size, NULL, timeout, NULL);
}

/* +IPD,<id>,<len>:<data> */
/* +IPD,<len>:<data> */

uint32_t recvPkg(uint8_t *buffer, uint32_t buffer_size, uint32_t *data_len, uint32_t timeout, uint8_t *coming_mux_id)
{
     
    char a;
    int32_t index_PIPDcomma = 0;
    int32_t index_colon = 0; /* : */
    int32_t index_comma = 0; /* , */
    int32_t len = 0;
    int8_t id = 0;
    int has_data = 0;
    uint32_t ret;
    unsigned long start;
    uint32_t i;
    ClearRecBuf();
    if (buffer == NULL) {
        return 0;
    }
    
    start = millis();
    while (millis() - start < timeout) {
        if(SerialAvailable() > 0) {
            a = SerialRead();
			StringAddchar(data_rec,a);
        }
        
        index_PIPDcomma = StringIndex(data_rec,"+IPD,");
        if (index_PIPDcomma != -1) {
            index_colon = StringIndexCharOffset(data_rec,':', index_PIPDcomma + 5);
            if (index_colon != -1) {
                index_comma = StringIndexCharOffset(data_rec,',', index_PIPDcomma + 5);
                /* +IPD,id,len:data */
                if (index_comma != -1 && index_comma < index_colon) { 
					char str_temp[5];
					StringSubstring(str_temp,data_rec,index_PIPDcomma + 5, index_comma);
					id = atoi(str_temp);
                    if (id < 0 || id > 4) {
                        return 0;
                    }
					StringSubstring(str_temp,data_rec,index_comma + 1, index_colon);
					len = atoi(str_temp);
                    if (len <= 0) {
                        return 0;
                    }
                } else { /* +IPD,len:data */
					char str_temp[5];
					StringSubstring(str_temp,data_rec,index_PIPDcomma + 5, index_colon);
					len = atoi(str_temp);
                    if (len <= 0) {
                        return 0;
                    }
                }
                has_data = 1;
                break;
            }
        }
    }
    
    if (has_data) {
        i = 0;
        ret = len > buffer_size ? buffer_size : len;
        start = millis();
        while (millis() - start < 3000) {
            while(SerialAvailable() > 0 && i < ret) {
                a = SerialRead();
                buffer[i++] = a;
            }
            if (i == ret) {
                rx_empty();
                if (data_len) {
                    *data_len = len;    
                }
                if (index_comma != 0 && coming_mux_id) {
                    *coming_mux_id = id;
                }
                return ret;
            }
        }
    }
    return 0;
}

void rx_empty(void) 
{
//    while(SerialAvailable() > 0) {
//        SerialRead();
//    }
	ClearRxBuf();
}

int eATUART(uint32_t baud)
{
	int int_baud = baud;
    rx_empty();
    SerialPrint("AT+UART=",STRING_TYPE);		
	SerialPrint(&int_baud,INT_TYPE);
	SerialPrintln(",8,1,0,0",STRING_TYPE);
	return recvFind("OK",TIME_OUT);	
}

int eATRST(void) 
{
    rx_empty();
    SerialPrintln("AT+RST",STRING_TYPE);
    return recvFind("OK",TIME_OUT);
}

int eAT(void)
{
    rx_empty();
    SerialPrintln("AT",STRING_TYPE);
    return recvFind("OK",TIME_OUT);
}

int sATCWJAP(const char *ssid, const char *pwd)
{
    rx_empty();
	ClearRecBuf(); 
    SerialPrint("AT+CWJAP=\"",STRING_TYPE);
    SerialPrint(ssid,STRING_TYPE);
    SerialPrint("\",\"",STRING_TYPE);
    SerialPrint(pwd,STRING_TYPE);
    SerialPrintln("\"",STRING_TYPE);
    
	recvString2(data_rec,"OK", "FAIL",10000);
    if (StringIndex(data_rec,"OK") != -1) {
        return 1;
    }
    return 0;
}

int eATCWQAP(void)
{
    ClearRecBuf();
    rx_empty();
    SerialPrintln("AT+CWQAP",STRING_TYPE);
    return recvFind("OK",TIME_OUT);
}

/*******************************************************************************
* �� �� �� ��eATCWSMARTSTART
* �������� ������smartlinkģʽ����Ҫ��30s������//add by LC 2016.01.05 16:27
* ��    �� ��type ������ʽ 0 -AL-LINK    1 - ESP-TOUCH    2 - AIR-KISS
			link_msg ���ص�SSID��PSD
* ��    �� ����
*******************************************************************************/
int eATCWSMARTSTART(uint8_t type,char *link_msg)
{
	int flag;
	int int_type = type;
    rx_empty();
    SerialPrint("AT+CWSMARTSTART=",STRING_TYPE);
    SerialPrintln(&int_type,INT_TYPE);	
	flag = recvFind("OK",TIME_OUT);
	if(flag == 0) return flag;
	delay(50);//��ʱ֮��ȴ��Զ�����
	rx_empty();
	return recvFindAndFilter("OK", "SMART SUCCESS", "\r\n\r\nOK", link_msg,30000);
}

//add by LC 2016.01.05 16:27
int eATCWSMARTSTOP(void)
{
    rx_empty();
    SerialPrintln("AT+CWSMARTSTOP",STRING_TYPE);	
	return recvFind("OK",TIME_OUT);	
}

int qATCWMODE(uint8_t *mode) 
{
    char str_mode[5];
    int ret;
    if (!mode) {
        return 0;
    }
    rx_empty();
    SerialPrintln("AT+CWMODE?",STRING_TYPE);
		
    ret = recvFindAndFilter("OK","+CWMODE:","\r\n\r\nOK",str_mode,TIME_OUT); 
    if (ret != 0) {
        *mode = (uint8_t)atoi(str_mode);
        return 1;
    } else {
        return 0;
    }
}

int sATCWMODE(uint8_t mode)
{
	int int_mode = mode;
    ClearRecBuf(); 
    rx_empty();
    SerialPrint("AT+CWMODE=",STRING_TYPE);
    SerialPrintln(&int_mode,INT_TYPE);
    
    recvString2(data_rec,"OK", "no change",TIME_OUT);
    if (StringIndex(data_rec,"OK") != -1 || StringIndex(data_rec,"no change") != -1) {
        return 0;
    }
    return 1;
}

int eATCIPSTATUS(char *data_list)
{
    //delay(100);//ȥ����ʱ modfied by LC 2016.01.06 23:46
    rx_empty();
    SerialPrintln("AT+CIPSTATUS",STRING_TYPE);
    return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", data_list, TIME_OUT);
}

//�ڽ��յ��ַ����в��� target ����ȡ begin �� end �м���Ӵ�д��data
int recvFindAndFilter(const char *target, const char *begin, const char *end, char *data_get,uint32_t timeout)
{
	  ClearRecBuf();
    recvString(data_rec,target,timeout);
    if (StringIndex(data_rec,target) != -1) {
        int32_t index1 = StringIndex(data_rec,begin);
        int32_t index2 = StringIndex(data_rec,end);
        if (index1 != -1 && index2 != -1) {
            index1 += StringLenth(begin);
            StringSubstring(data_get,data_rec,index1, index2-1);
            return 1;
        }
    }
	ClearRecBuf();
    return 0;
}

int sATCIPMUX(uint8_t mode)
{
	  int int_mode = mode;
     
    rx_empty();
    SerialPrint("AT+CIPMUX=",STRING_TYPE);
    SerialPrintln(&int_mode,INT_TYPE);
    ClearRecBuf();
    recvString2(data_rec,"OK", "Link is builded",TIME_OUT);
    if (StringIndex(data_rec,"OK") != -1) {
        return 1;
    }
    return 0;
}

int sATCIPSTARTSingle(const char *type, const char *addr, uint32_t port)
{
	int int_port = port;
    ClearRecBuf(); 
    rx_empty();
    SerialPrint("AT+CIPSTART=\"",STRING_TYPE);
    SerialPrint(type,STRING_TYPE);
    SerialPrint("\",\"",STRING_TYPE);
    SerialPrint(addr,STRING_TYPE);
    SerialPrint("\",",STRING_TYPE);
    SerialPrintln(&int_port,INT_TYPE);
    
    recvString3(data_rec,"OK", "ERROR", "ALREADY CONNECT", 10000);
    if (StringIndex(data_rec,"OK") != -1 || StringIndex(data_rec,"ALREADY CONNECT") != -1) {
        return 1;
    }
    return 0;
}

int sATCIPSENDSingle(const uint8_t *buffer, uint32_t len)
{
	  uint8_t i;
	  int int_len = len;
    rx_empty();
    SerialPrint("AT+CIPSEND=",STRING_TYPE);
    SerialPrintln(&int_len,INT_TYPE);
    if (-1 != recvFind(">", 5000)) {//5000
        rx_empty();
        for (i = 0; i < len; i++) {
            SerialWrite(buffer[i]);
        }
        return recvFind("SEND OK", 10000);//10000
    }
    return 0;
}



int recvFind(const char *target,uint32_t timeout)
{
	ClearRecBuf();
    recvString(data_rec, target, timeout);
    if (StringIndex(data_rec,target) != -1) {
        return 1;
    }
    return 0;
}

int recvString(char *rec_data, const char *target, uint32_t timeout)
{
    char a;
	unsigned long start;
    start = millis();
    while (millis() - start < timeout) {
        while(SerialAvailable() > 0) {
            a = SerialRead();
			if(a == '\0') continue;
			StringAddchar(rec_data,a);
        }
        if (StringIndex(rec_data,target) != -1) {
            break;
        }   
    }
    return 1;
}

int recvString2(char *rec_data, const char *target1, const char *target2, uint32_t timeout)
{
    char a;
    unsigned long start = millis();
    while (millis() - start < timeout) {
        while(SerialAvailable() > 0) {
            a = SerialRead();
			if(a == '\0') continue;
			StringAddchar(rec_data,a);
        }
        if (StringIndex(rec_data,target1) != -1) {
            break;
        } else if (StringIndex(rec_data,target2) != -1) {
            break;
        }
    }
    return 1;
}

int recvString3(char *rec_data, const char *target1, const char *target2, const char *target3, uint32_t timeout)
{
    char a;
    unsigned long start = millis();
    while (millis() - start < timeout) {
        while(SerialAvailable() > 0) {
            a = SerialRead();
			if(a == '\0') continue;
			StringAddchar(rec_data,a);
        }
        if (StringIndex(rec_data,target1) != -1) {
            break;
        } else if (StringIndex(rec_data,target2) != -1) {
            break;
        }else if (StringIndex(rec_data,target3) != -1) {
			break;
		}
    }
    return 1;	
}












