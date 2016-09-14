#ifndef __ESP8266_H__
#define __ESP8266_H__


typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned int uint32_t;
typedef signed int int32_t;

extern volatile unsigned long sys_tick;

#define RECV_BUF_SIZE  96//������յĻ��棬�����ܵĴ󣬷�ֹ���
#define TIME_OUT 100

#define AI_LINK 0
#define ESP_TOUCH 1
#define AIR_LINK 2

#define STATUS_GETIP 2 //��ȡ��IP
#define STATUS_GETLINK 3 //��������
#define STATUS_LOSTLINK 4 //ʧȥ����
#define STATUS_LOSTIP 5 //δ��ȡ��IP

//function
int WifiInit(const char *ssid, const char *psd, const char *addr, uint32_t port);
void AutoLink(void);
int WifiInitSmart(const char *addr, uint32_t port);
void timer1msINT(void);
unsigned long millis(void);
void delay(unsigned int ms);
int SetBaud(uint32_t baud);
int restart(void);
int setOprToStationSoftAP(void);
int joinAP(const char *ssid, const char *pwd);
int leaveAP(void);
int smartLink(uint8_t  type,char *link_msg);
int stopSmartLink(void);
int getSystemStatus(void);
int disableMUX(void);
int createTCP(const char *addr, uint32_t port);
int send(const uint8_t *buffer, uint32_t len);
int recv(uint8_t *buffer, uint32_t buffer_size, uint32_t timeout);
void rx_empty(void);

#endif
