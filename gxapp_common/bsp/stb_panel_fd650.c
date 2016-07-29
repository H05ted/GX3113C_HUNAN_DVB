/*
 * =====================================================================================
 *
 *       Filename:  bsp_panel.c
 *
 *    Description:  :
 *
 *        Version:  1.0
 *        Created:  2010��10��07�� 22ʱ20��26��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */
/* ---------------------------------------------- */
#include "stb_panel.h"
#if (PANEL_TYPE == PANEL_TYPE_fd650_TAIHUI)
extern	void poll_wakeup(void);
#if defined(LINUX_OS)
#include<linux/kernel.h>
#include <linux/gx_gpio.h>
#define PRESS_COUNT_MAX (2)
#define KEY_COUNT_MAX (1)
#elif defined(ECOS_OS)
#define PRESS_COUNT_MAX (3)
#define KEY_COUNT_MAX (5)
#endif


#define PATH_PRINT()  /*printf("\n%s\n", __FUNCTION__)*/

//******************************************************************************
//********FD650*FD650*FD650*FD650*FD650*FD650*FD650*FD650*FD650*FD650***********
//******************************************************************************

/* key */
#if 0
#define PANEL_KEY_MENU   95
#define PANEL_KEY_DOWN   71
#define PANEL_KEY_UP     79
#define PANEL_KEY_LEFT   103
#define PANEL_KEY_RIGHT   111
#define PANEL_KEY_OK      87
#define PANEL_LEY_NULL    0
#define PANEL_KEY_TOTAL   6
#endif

/* *************************************Ӳ�����****************************** */
#define DELAY 	\
    {\
	    uint32_t temp = 150; \
	    do{\
	        temp--;\
	    }while(temp);\
    }

/* **************************************Ӳ�����*********************************************** */

/* ********************************************************************************************* */
// ����ϵͳ��������

#define FD650_BIT_ENABLE	0x01		// ����/�ر�λ
#define FD650_BIT_SLEEP		0x04		// ˯�߿���λ
#define FD650_BIT_7SEG		0x08		// 7�ο���λ
#define FD650_BIT_INTENS1	0x10		// 1������
#define FD650_BIT_INTENS2	0x20		// 2������
#define FD650_BIT_INTENS3	0x30		// 3������
#define FD650_BIT_INTENS4	0x40		// 4������
#define FD650_BIT_INTENS5	0x50		// 5������
#define FD650_BIT_INTENS6	0x60		// 6������
#define FD650_BIT_INTENS7	0x70		// 7������
#define FD650_BIT_INTENS8	0x00		// 8������

#define FD650_SYSOFF	0x0400			// �ر���ʾ���رռ���
#define FD650_SYSON		( FD650_SYSOFF | FD650_BIT_ENABLE )	// ������ʾ������
#define FD650_SLEEPOFF	FD650_SYSOFF	// �ر�˯��
#define FD650_SLEEPON	( FD650_SYSOFF | FD650_BIT_SLEEP )	// ����˯��
#define FD650_7SEG_ON	( FD650_SYSON | FD650_BIT_7SEG )	// �����߶�ģʽ
#define FD650_8SEG_ON	( FD650_SYSON | 0x00 )	// �����˶�ģʽ
#define FD650_SYSON_1	( FD650_SYSON | FD650_BIT_INTENS1 )	// ������ʾ�����̡�1������
//�Դ�����
#define FD650_SYSON_4	( FD650_SYSON | FD650_BIT_INTENS4 )	// ������ʾ�����̡�4������
//�Դ�����
#define FD650_SYSON_8	( FD650_SYSON | FD650_BIT_INTENS8 )	// ������ʾ�����̡�8������


// ��������������
#define FD650_DIG0		0x1400			// �����λ0��ʾ,�����8λ����
#define FD650_DIG1		0x1500			// �����λ1��ʾ,�����8λ����
#define FD650_DIG2		0x1600			// �����λ2��ʾ,�����8λ����
#define FD650_DIG3		0x1700			// �����λ3��ʾ,�����8λ����

#define FD650_DOT			0x0080			// �����С������ʾ

// ��ȡ������������
#define FD650_GET_KEY	0x0700					// ��ȡ����,���ذ�������


#define GXPIO_BIT_HIGHT_LEVEL 1
#define GXPIO_BIT_LOW_LEVEL    0
extern uint32_t s_panel_key;
	
typedef struct GXPANEL_FD650_Config_s
{
	uint8_t m_chPanelName[MAX_PANEL_NAME_LENGTH];

	uint8_t m_chLedBit[8];

	uint8_t m_chShowMode;//GXPanel_ShowMode_t
	uint8_t m_chSegmentLight;//0��ʾ0���� �����ģ�1��ʾ1���� ������//һ��Ϊ����
	uint8_t m_chStandbyPin;//standby������λѡ 0,1,2,3�ĸ�ѡ��
	uint8_t m_chLockPin;	//Lock������λѡ 0,1,2,3�ĸ�ѡ��

	uint8_t m_chClkPin;	//the clk pin
	uint8_t m_chDataPin;	//the data pin
	uint8_t m_chLedCnt;	//the number of panel led
	uint8_t m_chCurLenCnt;//cur scan led

	uint32_t m_nKeyValue[8];
	uint32_t m_nLedAddr[4];//the led pin
	uint32_t m_nKeyTranslateCode[8];//the led pin	  ��m_nKeyValue[8]һһ��Ӧ

	uint32_t m_nPortBaseAddr[PORT_BASE_ADDRESS_END];//the base address of port

	GXPANEL_PUBLIC_Config_t m_tPanelPublicConfig;
}GXPANEL_FD650_Config_t;

GXPANEL_FD650_Config_t g_PanelFD650Config;

static uint8_t gs_chLedData[LED_DATA_DARK+1]={
		(BIT_A|BIT_B|BIT_C|BIT_D|BIT_E|BIT_F),		//0
		(BIT_B|BIT_C),								//1
		(BIT_A|BIT_B|BIT_D|BIT_E|BIT_G),			//2
		(BIT_A|BIT_B|BIT_C|BIT_D|BIT_G),			//3
		(BIT_B|BIT_C|BIT_F|BIT_G),					//4
		(BIT_A|BIT_C|BIT_D|BIT_F|BIT_G),			//5
		(BIT_A|BIT_C|BIT_D|BIT_E|BIT_F|BIT_G),		//6
		(BIT_A|BIT_B|BIT_C),						//7
		(BIT_A|BIT_B|BIT_C|BIT_D|BIT_E|BIT_F|BIT_G),//8
		(BIT_A|BIT_B|BIT_C|BIT_D|BIT_F|BIT_G),		//9
		(BIT_A|BIT_D|BIT_E|BIT_F|BIT_G),			//E
		(BIT_A|BIT_E|BIT_F|BIT_G),					//F
		(BIT_A|BIT_B|BIT_C|BIT_E|BIT_F),			//N
		(BIT_A|BIT_B|BIT_E|BIT_F|BIT_G),			//P
	(BIT_C|BIT_D|BIT_E|BIT_F|BIT_G),            //t
	(BIT_D|BIT_E|BIT_F),                        //L
	(BIT_C|BIT_D|BIT_E|BIT_F|BIT_G),            //b
	(BIT_C|BIT_D|BIT_E|BIT_G),                  //o
	0x00,                                       //HIDE
        (BIT_A|BIT_D|BIT_E|BIT_F),                  //C
	(BIT_B|BIT_C|BIT_D|BIT_E|BIT_F),            //U
	0x00					    //dark
	};/*0,1,2,3,4,5,6,7,8,9,E,F,N,P,T,dark*/

static uint8_t gs_chLedValue[5];
static uint8_t s_standby;
static uint8_t panel_old_string[5];
static uint8_t panel_old_lock;

extern void gx_stb_panel_fd650_send_keymsg(uint32_t nKeyIndex);
extern void gx_stb_panel_fd650_set_led_string(unsigned char* str);
extern void gx_stb_panel_fd650_lock(void);
extern void gx_stb_panel_fd650_set_led_signal_value(uint32_t nLedValue);
extern void gx_stb_panel_fd650_set_led_string(unsigned char* str);
extern void gx_stb_panel_fd650_set_led_value(uint32_t nLedValue);
extern void gx_stb_panel_fd650_unlock(void);
extern void gx_stb_panel_fd650_standby(void);


static void gx_stb_panel_fd650_init_led_data(void)
{
	gs_chLedData[LED_DATA_0] = (g_PanelFD650Config.m_chLedBit[BIT_A]
					| g_PanelFD650Config.m_chLedBit[BIT_B]
					| g_PanelFD650Config.m_chLedBit[BIT_C]
					| g_PanelFD650Config.m_chLedBit[BIT_D]
					| g_PanelFD650Config.m_chLedBit[BIT_E]
					| g_PanelFD650Config.m_chLedBit[BIT_F]);
	gs_chLedData[LED_DATA_1] = (g_PanelFD650Config.m_chLedBit[BIT_B]
					| g_PanelFD650Config.m_chLedBit[BIT_C]);
	gs_chLedData[LED_DATA_2] = (g_PanelFD650Config.m_chLedBit[BIT_A]
					| g_PanelFD650Config.m_chLedBit[BIT_B]
					| g_PanelFD650Config.m_chLedBit[BIT_D]
					| g_PanelFD650Config.m_chLedBit[BIT_E]
					| g_PanelFD650Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_3] = (g_PanelFD650Config.m_chLedBit[BIT_A]
					| g_PanelFD650Config.m_chLedBit[BIT_B]
					| g_PanelFD650Config.m_chLedBit[BIT_C]
					| g_PanelFD650Config.m_chLedBit[BIT_D]
					| g_PanelFD650Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_4] = (g_PanelFD650Config.m_chLedBit[BIT_B]
					| g_PanelFD650Config.m_chLedBit[BIT_C]
					| g_PanelFD650Config.m_chLedBit[BIT_F]
					| g_PanelFD650Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_5] = (g_PanelFD650Config.m_chLedBit[BIT_A]
					| g_PanelFD650Config.m_chLedBit[BIT_C]
					| g_PanelFD650Config.m_chLedBit[BIT_D]
					| g_PanelFD650Config.m_chLedBit[BIT_F]
					| g_PanelFD650Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_6] = (g_PanelFD650Config.m_chLedBit[BIT_A]
					| g_PanelFD650Config.m_chLedBit[BIT_C]
					| g_PanelFD650Config.m_chLedBit[BIT_D]
					| g_PanelFD650Config.m_chLedBit[BIT_E]
					| g_PanelFD650Config.m_chLedBit[BIT_F]
					| g_PanelFD650Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_7] = (g_PanelFD650Config.m_chLedBit[BIT_A]
					| g_PanelFD650Config.m_chLedBit[BIT_B]
					| g_PanelFD650Config.m_chLedBit[BIT_C]);
	gs_chLedData[LED_DATA_8] = (g_PanelFD650Config.m_chLedBit[BIT_A]
					| g_PanelFD650Config.m_chLedBit[BIT_B]
					| g_PanelFD650Config.m_chLedBit[BIT_C]
					| g_PanelFD650Config.m_chLedBit[BIT_D]
					| g_PanelFD650Config.m_chLedBit[BIT_E]
					| g_PanelFD650Config.m_chLedBit[BIT_F]
					| g_PanelFD650Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_9] = (g_PanelFD650Config.m_chLedBit[BIT_A]
					| g_PanelFD650Config.m_chLedBit[BIT_B]
					| g_PanelFD650Config.m_chLedBit[BIT_C]
					| g_PanelFD650Config.m_chLedBit[BIT_D]
					| g_PanelFD650Config.m_chLedBit[BIT_F]
					| g_PanelFD650Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_E] = (g_PanelFD650Config.m_chLedBit[BIT_A]
					| g_PanelFD650Config.m_chLedBit[BIT_D]
					| g_PanelFD650Config.m_chLedBit[BIT_E]
					| g_PanelFD650Config.m_chLedBit[BIT_F]
					| g_PanelFD650Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_F] = (g_PanelFD650Config.m_chLedBit[BIT_A]
					| g_PanelFD650Config.m_chLedBit[BIT_E]
					| g_PanelFD650Config.m_chLedBit[BIT_F]
					| g_PanelFD650Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_N] = (g_PanelFD650Config.m_chLedBit[BIT_A]
					| g_PanelFD650Config.m_chLedBit[BIT_B]
					| g_PanelFD650Config.m_chLedBit[BIT_C]
					| g_PanelFD650Config.m_chLedBit[BIT_E]
					| g_PanelFD650Config.m_chLedBit[BIT_F]);
	gs_chLedData[LED_DATA_P] = (g_PanelFD650Config.m_chLedBit[BIT_A]
					| g_PanelFD650Config.m_chLedBit[BIT_B]
					| g_PanelFD650Config.m_chLedBit[BIT_E]
					| g_PanelFD650Config.m_chLedBit[BIT_F]
					| g_PanelFD650Config.m_chLedBit[BIT_G]);
    gs_chLedData[LED_DATA_C] = (g_PanelFD650Config.m_chLedBit[BIT_A]
					| g_PanelFD650Config.m_chLedBit[BIT_D]
					| g_PanelFD650Config.m_chLedBit[BIT_E]
					| g_PanelFD650Config.m_chLedBit[BIT_F]);
	gs_chLedData[LED_DATA_t] = (g_PanelFD650Config.m_chLedBit[BIT_D]
					| g_PanelFD650Config.m_chLedBit[BIT_E]
					| g_PanelFD650Config.m_chLedBit[BIT_F]
					| g_PanelFD650Config.m_chLedBit[BIT_G]);
    gs_chLedData[LED_DATA_U] = (g_PanelFD650Config.m_chLedBit[BIT_B]
                    | g_PanelFD650Config.m_chLedBit[BIT_C]
					| g_PanelFD650Config.m_chLedBit[BIT_D]
					| g_PanelFD650Config.m_chLedBit[BIT_E]
					| g_PanelFD650Config.m_chLedBit[BIT_F]);
	gs_chLedData[LED_DATA_L] = (g_PanelFD650Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_b] = (g_PanelFD650Config.m_chLedBit[BIT_C]
					| g_PanelFD650Config.m_chLedBit[BIT_D]
					| g_PanelFD650Config.m_chLedBit[BIT_E]
					| g_PanelFD650Config.m_chLedBit[BIT_F]
					| g_PanelFD650Config.m_chLedBit[BIT_G]);

}
void gx_stb_panel_fd650_set_led_dark(void)
{
	gs_chLedValue[0] = gs_chLedData[LED_DATA_DARK];
	gs_chLedValue[1] = gs_chLedData[LED_DATA_DARK];
	gs_chLedValue[2] = gs_chLedData[LED_DATA_DARK];
	gs_chLedValue[3] = gs_chLedData[LED_DATA_DARK];
}

void gx_stb_panel_fd650_start( void )
{
	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioOutPutFun(g_PanelFD650Config.m_chDataPin);
	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chDataPin
											, GXPIO_BIT_HIGHT_LEVEL);

	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioOutPutFun(g_PanelFD650Config.m_chClkPin);
	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chClkPin
											, GXPIO_BIT_HIGHT_LEVEL);
	DELAY;
	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chDataPin
											, GXPIO_BIT_LOW_LEVEL);
	DELAY;
	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chClkPin
											, GXPIO_BIT_LOW_LEVEL);
}

void gx_stb_panel_fd650_stop( void )
{
	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chDataPin
											, GXPIO_BIT_LOW_LEVEL);
	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioOutPutFun(g_PanelFD650Config.m_chDataPin);
	DELAY;
	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chClkPin
											, GXPIO_BIT_HIGHT_LEVEL);
	DELAY;
	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chDataPin
											, GXPIO_BIT_HIGHT_LEVEL);
	DELAY;
	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioInPutFun(g_PanelFD650Config.m_chDataPin);
}

/*******************************************************************************
  ��������������:  gx_stb_panel_fd650_send_data(u8 Data)
  �������ܡ�����:  ����һ�����ݻ�����
  �������������:  Data:Ҫ���͵����ݻ�����
*******************************************************************************/
static void gx_stb_panel_fd650_send_data(uint8_t Data)
{
    uint8_t i=0;

	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioOutPutFun(g_PanelFD650Config.m_chDataPin);
	for (i = 0; i < 8; i++)
	{
		if(Data & 0x80)
		{
			//GXBSP_WSSetDataOut();
			g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chDataPin
											, GXPIO_BIT_HIGHT_LEVEL);

		}
		else
		{
			//GXBSP_WSClearDataOut();
			g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chDataPin
											, GXPIO_BIT_LOW_LEVEL);

		}

        DELAY;

		g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chClkPin
											, GXPIO_BIT_HIGHT_LEVEL);

		Data <<= 1;

        DELAY;

		g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chClkPin
											, GXPIO_BIT_LOW_LEVEL);
	}

	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioInPutFun(g_PanelFD650Config.m_chDataPin);
	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chDataPin
											, GXPIO_BIT_HIGHT_LEVEL);
	DELAY;

	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chClkPin
											, GXPIO_BIT_HIGHT_LEVEL);
	DELAY;

	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chClkPin
											, GXPIO_BIT_LOW_LEVEL);

	return;

}
/*******************************************************************************
  ��������������:  gx_stb_panel_fd650_send_data_cmd
  �������ܡ�����:  ����һ���������Ӧ������
  �������������:
  ����Command:Ҫ���͵������������д��˭��
*********************************************************************************/
static void gx_stb_panel_fd650_send_data_cmd(uint16_t Command)
{
	gx_stb_panel_fd650_start();
	gx_stb_panel_fd650_send_data(((uint8_t)(Command>>7)&0x3E)|0x40);
	gx_stb_panel_fd650_send_data(Command);
	gx_stb_panel_fd650_stop();
	return;

}

/*******************************************************************************
  ��������������:  GXBSP_WSDisplayLedData
  �������ܡ�����:  ��ʾLED������
    �������������:
  ����*DisplayData:ָ��Ҫ��ʾ�����ݵ�ָ��

*******************************************************************************/

static void gx_stb_panel_fd650_led_scan(uint8_t* DisplayData, uint8_t sec_flag, uint8_t Lock)
{
	int iLenth;
	if( strcmp((char*)DisplayData, "") == 0 )
	{
		return;
	}
	iLenth = strlen((char*)DisplayData);
	if(iLenth>4)
		iLenth = 4;


	gx_stb_panel_fd650_send_data_cmd(FD650_SYSON_8);// ������ʾ�ͼ��̣�8����ʾ��ʽ
	//����ʾ����
	if(Lock)
		gx_stb_panel_fd650_send_data_cmd( g_PanelFD650Config.m_nLedAddr[0] | (uint8_t)DisplayData[0]  | FD650_DOT);	//������һ�������
	else
		gx_stb_panel_fd650_send_data_cmd( g_PanelFD650Config.m_nLedAddr[0] | (uint8_t)DisplayData[0] );
	//if(Lock)
	//	gx_stb_panel_fd650_send_data_cmd( g_PanelFD650Config.m_nLedAddr[1] | (uint8_t)DisplayData[1] | FD650_DOT ); //�����ڶ��������
	//else
		gx_stb_panel_fd650_send_data_cmd( g_PanelFD650Config.m_nLedAddr[1] | (uint8_t)DisplayData[1] );
  //  if(Lock)
    //    gx_stb_panel_fd650_send_data_cmd( g_PanelFD650Config.m_nLedAddr[2] | (uint8_t)DisplayData[2] | FD650_DOT);
    //else
	    gx_stb_panel_fd650_send_data_cmd( g_PanelFD650Config.m_nLedAddr[2] | (uint8_t)DisplayData[2] ); //���������������
	//if(Lock)
	//	gx_stb_panel_fd650_send_data_cmd( g_PanelFD650Config.m_nLedAddr[3] | (uint8_t)DisplayData[3] | FD650_DOT); //�������ĸ������
	//else
		gx_stb_panel_fd650_send_data_cmd( g_PanelFD650Config.m_nLedAddr[3] | (uint8_t)DisplayData[3] );


}

/*******************************************************************************
  ��������������:  GXBSP_WSReceiveData
  �������ܡ�����:  ����һ��8λ����
  �������������:
  ����*ReceiveData:���յ�������ָ��

*******************************************************************************/
static void gx_stb_panel_fd650_receive_data(uint8_t* ReceiveData)
{
	uint8_t i=0;
	uint8_t TempData=0;
	uint64_t TempRgValue=0;

	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chDataPin
					, GXPIO_BIT_HIGHT_LEVEL);
	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioInPutFun(g_PanelFD650Config.m_chDataPin);

	TempData = 0;
	for(i=0;i<8;i++)
	{
		DELAY;
		g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chClkPin
									, GXPIO_BIT_HIGHT_LEVEL);
		DELAY;
		TempData <<=1;
		g_PanelFD650Config.m_tPanelPublicConfig.m_PanelGetGpioLevelFun(g_PanelFD650Config.m_chDataPin, &TempRgValue);
		if(TempRgValue)
			TempData++;
		g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chClkPin
					, GXPIO_BIT_LOW_LEVEL);
	}

	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chDataPin
					, GXPIO_BIT_HIGHT_LEVEL);
	DELAY;
	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chClkPin
					, GXPIO_BIT_HIGHT_LEVEL);
	DELAY;
	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chClkPin
					, GXPIO_BIT_LOW_LEVEL);
	*ReceiveData = TempData;

	return;
}

static int32_t gx_stb_panel_fd650_translate_key(uint32_t renm_ScanData)
{
//	bsp_printf("gx_stb_panel_fd650_translate_key renm_ScanData=0x%x\n",renm_ScanData);
	if (renm_ScanData == g_PanelFD650Config.m_nKeyTranslateCode[0])
	{
		return (0);
	}
	else if (renm_ScanData == g_PanelFD650Config.m_nKeyTranslateCode[1])
	{
		return (1);
	}
	else if (renm_ScanData == g_PanelFD650Config.m_nKeyTranslateCode[2])
	{
		return (2);
	}
	else if (renm_ScanData == g_PanelFD650Config.m_nKeyTranslateCode[3])
	{
		return (3);
	}
	else if (renm_ScanData == g_PanelFD650Config.m_nKeyTranslateCode[4])
	{
		return (4);
	}
	else if (renm_ScanData == g_PanelFD650Config.m_nKeyTranslateCode[5])
	{
		return (5);
	}
	else if (renm_ScanData == g_PanelFD650Config.m_nKeyTranslateCode[6])
	{
		return (6);
	}
	else if (renm_ScanData == g_PanelFD650Config.m_nKeyTranslateCode[7])
	{
		return (7);
	}
	else
	{
        //bsp_printf("\n 0x%x\n",renm_ScanData);
		return (-1);
	}
}

/*******************************************************************************
  ��������������:  gx_stb_panel_fd650_translate_key
  �������ܡ�����:  ��ȡ���������д���
  �������������:  ��
*******************************************************************************/
static void gx_stb_panel_fd650_process_key(void)
{
    uint8_t button_mask_curr;
    uint8_t uKeyCode=-1;
    static uint8_t button_mask=0;
    static uint8_t iPressCount=0;
    static uint32_t uKeyCodePrev=0xff;
    static int32_t iKeyCount=0;
    unsigned char bKeyPressed=0;
//	bsp_printf("gx_stb_panel_fd650_process_key enter\n");

	gx_stb_panel_fd650_start();
	gx_stb_panel_fd650_send_data(((uint8_t)(FD650_GET_KEY>>7)&0x3E)|0x01|0x40);
	gx_stb_panel_fd650_receive_data(&button_mask_curr);
	gx_stb_panel_fd650_stop();
	if((button_mask_curr&0x00000040) ==0)
		button_mask_curr = 0;

    if(button_mask_curr!=0)
    {

        if(button_mask_curr==button_mask)
        {
            iPressCount++;
        }
        else
        {
            iPressCount = 0;
            button_mask = button_mask_curr;
        }
    }
    else
    {
        iPressCount = 0;
        button_mask = 0;
        uKeyCodePrev = -1;
    }

//    bsp_printf("gx_stb_panel_fd650_process_key iPressCount=%d\n",iPressCount);
    if(iPressCount >= PRESS_COUNT_MAX)/*ȥ����*/
    {
        uKeyCode = gx_stb_panel_fd650_translate_key((uint32_t)button_mask);
        bKeyPressed = FALSE;
        button_mask = 0;
        if(uKeyCode != -1)
        {
            if(uKeyCodePrev == uKeyCode)/*����������*/
            {
                iKeyCount++;
                if(iKeyCount>=KEY_COUNT_MAX)
                {
                    iKeyCount = 0;
                    bKeyPressed = TRUE;
                }
            }
            else
            {
                uKeyCodePrev = uKeyCode;
                iKeyCount = 0;
                bKeyPressed = TRUE;
            }
        }
        else
        {
            iPressCount = 0;
            button_mask = 0;
            uKeyCodePrev = -1;
        }

        if(bKeyPressed)
        {
        
		//bsp_printf("uKeyCode = %d\n",uKeyCode);
            gx_stb_panel_fd650_send_keymsg(uKeyCode);
        }
    }

    return;

}

void gx_stb_panel_fd650_scan_manager(void)
{
	static uint32_t count = 0;

	if(PANEL_SHOW_NORMAL == g_PanelFD650Config.m_chShowMode)
	{

	}
	else if(PANEL_SHOW_TIME == g_PanelFD650Config.m_chShowMode)
	{

	}
	else if(PANEL_SHOW_OFF == g_PanelFD650Config.m_chShowMode)
	{
		//gx_stb_panel_fd650_set_led_string(GXLED_OFF);
        gx_stb_panel_fd650_set_led_dark();
		//gx_stb_panel_fd650_unlock();
		gx_stb_panel_fd650_standby();
        count = 6;
        gx_stb_panel_fd650_led_scan(gs_chLedValue, 0, panel_old_lock);
	}

	DELAY;

	count++;
	if(count >5 &&strcmp((char*)panel_old_string,(char*)gs_chLedValue))
	{
		memcpy(panel_old_string,gs_chLedValue,sizeof(panel_old_string));
		gx_stb_panel_fd650_led_scan(gs_chLedValue, 0, panel_old_lock);
	}

//	bsp_printf("gx_stb_panel_fd650_scan_manager count=%d \n",count);

	if(count > 5)
	{
		count = 0;
		gx_stb_panel_fd650_process_key();
	}

	return;

}

void gx_stb_panel_fd650_set_led_value(uint32_t nLedValue)
{
	uint32_t i;

	gs_chLedValue[0] = gs_chLedData[LED_DATA_DARK];//
	gs_chLedValue[1] = gs_chLedData[LED_DATA_DARK];//LED_DATA_DARK   ����ȡ��������ʾ��ʱ��ȡ��
	gs_chLedValue[2] = gs_chLedData[LED_DATA_DARK];//
	gs_chLedValue[3] = gs_chLedData[LED_DATA_DARK];//

	if(nLedValue > 9999)
		nLedValue = 9999;

	for(i = 0; i < g_PanelFD650Config.m_chLedCnt; i++)
	{
		gs_chLedValue[i] = (nLedValue)%10;
		nLedValue = (nLedValue - gs_chLedValue[i])/10;
	}

	for(i = 0; i < g_PanelFD650Config.m_chLedCnt; i++)
	{
		if(g_PanelFD650Config.m_chSegmentLight==1)
		{
			gs_chLedValue[i] = gs_chLedData[gs_chLedValue[i]];
		}
		else
		{
			gs_chLedValue[i] = (~gs_chLedData[gs_chLedValue[i]]);
		}
		//bsp_printf("====gs_chLedValue[%d]===%d\n",i,gs_chLedValue[i]);
	}

}

void gx_stb_panel_fd650_set_led_signal_value(uint32_t nLedValue)
{
	uint32_t i;

	gs_chLedValue[0] = gs_chLedData[LED_DATA_DARK];
	gs_chLedValue[1] = gs_chLedData[LED_DATA_DARK];
	gs_chLedValue[2] = gs_chLedData[LED_DATA_DARK];
	gs_chLedValue[3] = gs_chLedData[LED_DATA_DARK];

	if(nLedValue > 9999)
		nLedValue = 9999;

	for(i = 0; i < g_PanelFD650Config.m_chLedCnt; i++)
	{
		gs_chLedValue[i] = (nLedValue)%10;
		nLedValue = (nLedValue - gs_chLedValue[i])/10;
	}

	for(i = 0; i < g_PanelFD650Config.m_chLedCnt - 1; i++)
	{
		if(g_PanelFD650Config.m_chSegmentLight==1)
		{
			gs_chLedValue[i] = gs_chLedData[gs_chLedValue[i]];
		}
		else
		{
			gs_chLedValue[i] = (~gs_chLedData[gs_chLedValue[i]]);
		}
	}

	if(g_PanelFD650Config.m_chSegmentLight==1)
	{
		gs_chLedValue[g_PanelFD650Config.m_chLedCnt-1] = gs_chLedData[LED_DATA_C];
	}
	else
	{
		gs_chLedValue[g_PanelFD650Config.m_chLedCnt-1] = (~gs_chLedData[LED_DATA_C]);
	}
//    bsp_printf("gx_stb_panel_fd650_set_led_signal_value\n");
}

void gx_stb_panel_fd650_set_led_string(unsigned char* str)
{
	unsigned char LedStr=*str;

	gs_chLedValue[0] = gs_chLedData[LED_DATA_DARK];
	gs_chLedValue[1] = gs_chLedData[LED_DATA_DARK];
	gs_chLedValue[2] = gs_chLedData[LED_DATA_DARK];
	gs_chLedValue[3] = gs_chLedData[LED_DATA_DARK];

	switch(LedStr)
	{
		case GXLED_BOOT:
			if(3 == g_PanelFD650Config.m_chLedCnt)
			{
				gs_chLedValue[0] = gs_chLedData[LED_DATA_t];
				gs_chLedValue[1] = gs_chLedData[LED_DATA_o];
				gs_chLedValue[2] = gs_chLedData[LED_DATA_b];
			}
			else if(4 == g_PanelFD650Config.m_chLedCnt)
			{
				gs_chLedValue[0] = gs_chLedData[LED_DATA_t];
				gs_chLedValue[1] = gs_chLedData[LED_DATA_o];
				gs_chLedValue[2] = gs_chLedData[LED_DATA_o];
				gs_chLedValue[3] = gs_chLedData[LED_DATA_b];
			}
			break;
		case GXLED_INIT:
			if(3 == g_PanelFD650Config.m_chLedCnt)
			{
				gs_chLedValue[0] = gs_chLedData[LED_DATA_t];
				gs_chLedValue[1] = gs_chLedData[LED_DATA_N];
				gs_chLedValue[2] = gs_chLedData[LED_DATA_1];
			}
			else if(4 == g_PanelFD650Config.m_chLedCnt)
			{
				gs_chLedValue[0] = gs_chLedData[LED_DATA_t];
				gs_chLedValue[1] = gs_chLedData[LED_DATA_1];
				gs_chLedValue[2] = gs_chLedData[LED_DATA_N];
				gs_chLedValue[3] = gs_chLedData[LED_DATA_1];
			}
			break;
		case GXLED_ON:
			gs_chLedValue[0] = gs_chLedData[LED_DATA_N];
			gs_chLedValue[1] = gs_chLedData[LED_DATA_0];
			break;
		case GXLED_OFF:
			gs_chLedValue[0] = gs_chLedData[LED_DATA_F];
			gs_chLedValue[1] = gs_chLedData[LED_DATA_F];
			gs_chLedValue[2] = gs_chLedData[LED_DATA_0];
			break;
		case GXLED_E:
			gs_chLedValue[0] = gs_chLedData[LED_DATA_E];
			break;
		case GXLED_P:
			gs_chLedValue[0] = gs_chLedData[LED_DATA_P];
			break;
		case GXLED_USB:
			gs_chLedValue[0] = gs_chLedData[LED_DATA_b];
			gs_chLedValue[1] = gs_chLedData[LED_DATA_5];
			gs_chLedValue[2] = gs_chLedData[LED_DATA_U];
            		break;

		case GXLED_SEAR:
			if(3 == g_PanelFD650Config.m_chLedCnt)
			{
				gs_chLedValue[0] = gs_chLedData[LED_DATA_L];
				gs_chLedValue[1] = gs_chLedData[LED_DATA_L];
				gs_chLedValue[2] = gs_chLedData[LED_DATA_L];
			}
			else if(4 == g_PanelFD650Config.m_chLedCnt)
			{
				gs_chLedValue[0] = gs_chLedData[LED_DATA_L];
				gs_chLedValue[1] = gs_chLedData[LED_DATA_L];
				gs_chLedValue[2] = gs_chLedData[LED_DATA_L];
				gs_chLedValue[3] = gs_chLedData[LED_DATA_L];
			}
			break;
		case GXLED_HIDE:
			break;
	}
}

void gx_stb_panel_fd650_lock(void)
{
	panel_old_lock = 1;
	gx_stb_panel_fd650_led_scan(panel_old_string, 0, panel_old_lock);
}

void gx_stb_panel_fd650_unlock(void)
{
	panel_old_lock = 0;
	gx_stb_panel_fd650_led_scan(panel_old_string, 0, panel_old_lock);
}

void gx_stb_panel_fd650_standby(void)
{
	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chStandbyPin
								, GXPIO_BIT_HIGHT_LEVEL);
	gx_stb_panel_fd650_send_data_cmd(FD650_SYSOFF);// ������ʾ�ͼ��̣�8����ʾ��ʽ
}

void gx_stb_panel_fd650_on(void)
{
	g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chStandbyPin
							, GXPIO_BIT_LOW_LEVEL);
	s_standby = 0;
}

void gx_stb_panel_fd650_show_mode(GXPanel_ShowMode_t PanelMode)
{
	g_PanelFD650Config.m_chShowMode = PanelMode;
}

void gx_stb_panel_fd650_power_off(void)
{
	gx_stb_panel_fd650_show_mode(PANEL_POWER_OFF);
	gx_stb_panel_fd650_scan_manager();
	gx_stb_panel_fd650_standby();
}

void gx_stb_panel_fd650_send_keymsg(uint32_t nKeyIndex)
{
//	bsp_printf("+++++++++ key index %d\n", nKeyIndex);
	if((nKeyIndex!=0xff)&&(nKeyIndex<8))
		{
			s_panel_key = g_PanelFD650Config.m_nKeyValue[nKeyIndex];
			poll_wakeup();

	}//		g_PanelFD650Config.m_tPanelPublicConfig.m_PanelDealKey(nKeyIndex);

	
}

void gx_stb_panel_fd650_init_pin(void)
{
	//GXPIO_InitParams_t  init_params;
	//GXPIO_OpenParams_t 	open_params;
	uint64_t nCfgPin = 0;

	nCfgPin = (((uint64_t)1) << g_PanelFD650Config.m_chClkPin)//37
			| (((uint64_t)1) << g_PanelFD650Config.m_chDataPin)//36
			|(((uint64_t)1) << g_PanelFD650Config.m_chStandbyPin);

	/*led�Ƴ�ʼ��*/
	gx_stb_panel_fd650_init_led_data();
    gx_stb_panel_fd650_on();
	(g_PanelPublicConfig.m_PanelCfgMultiplexFun)(nCfgPin);

	if (g_PanelFD650Config.m_nPortBaseAddr[0] != 0
		&& g_PanelFD650Config.m_nPortBaseAddr[1] == 0)
	{
		g_PanelFD650Config.m_tPanelPublicConfig.m_PanelInitGpioFun(nCfgPin);
		g_PanelFD650Config.m_tPanelPublicConfig.m_PanelOpenGpioFun(g_PanelFD650Config.m_chClkPin, GPIO_OUTPUT);
		g_PanelFD650Config.m_tPanelPublicConfig.m_PanelOpenGpioFun(g_PanelFD650Config.m_chDataPin, GPIO_OUTPUT);
		g_PanelFD650Config.m_tPanelPublicConfig.m_PanelOpenGpioFun(g_PanelFD650Config.m_chStandbyPin, GPIO_OUTPUT);
		g_PanelFD650Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelFD650Config.m_chStandbyPin, GXPIO_BIT_LOW_LEVEL);

		/*init_params.BaseAddress = g_PanelFD650Config.m_nPortBaseAddr[0];
		init_params.IntNumber = 0;
		init_params.IntSource = 0;
		init_params.IntEn = 0;

		g_PanelFD650Config.m_tPanelPublicConfig.m_PanelInitGpioFun("FrontPanel", &init_params);

		open_params.ReservedBits =(1 << g_PanelFD650Config.m_chClkPin)
					| (1 << g_PanelFD650Config.m_chDataPin);

		if(g_PanelFD650Config.m_chGpioLockEn)
		{
			open_params.ReservedBits |= (1 << g_PanelFD650Config.m_chLockPin);

			open_params.BitCfg[g_PanelFD650Config.m_chLockPin].Direction = GXPIO_BIT_OUTPUT;//Note:INput here
			open_params.BitCfg[g_PanelFD650Config.m_chLockPin].Pole = GXPIO_BIT_POSITIVE;
			open_params.BitCfg[g_PanelFD650Config.m_chLockPin].Level = GXPIO_BIT_HIGHT_LEVEL;
		}

		open_params.BitCfg[g_PanelFD650Config.m_chClkPin].Direction = GXPIO_BIT_OUTPUT;
		open_params.BitCfg[g_PanelFD650Config.m_chClkPin].Pole = GXPIO_BIT_POSITIVE;
		open_params.BitCfg[g_PanelFD650Config.m_chClkPin].Level = GXPIO_BIT_HIGHT_LEVEL;

		open_params.BitCfg[g_PanelFD650Config.m_chDataPin].Direction = GXPIO_BIT_OUTPUT;
		open_params.BitCfg[g_PanelFD650Config.m_chDataPin].Pole = GXPIO_BIT_POSITIVE;
		open_params.BitCfg[g_PanelFD650Config.m_chDataPin].Level = GXPIO_BIT_HIGHT_LEVEL;

		g_PanelFD650Config.m_tPanelPublicConfig.m_PanelOpenGpioFun("FrontPanel", &open_params, &g_PanelPublicConfig.m_nGpioHandle);*/
	}
	else if (g_PanelFD650Config.m_nPortBaseAddr[0] == 0
		&& g_PanelFD650Config.m_nPortBaseAddr[1] != 0)
	{
		/*GPIO��ʼ��*/
		/*init_params.BaseAddress = g_PanelFD650Config.m_nPortBaseAddr[1];
		init_params.IntNumber = 0;
		init_params.IntSource = 0;
		init_params.IntEn = 0;

		g_PanelFD650Config.m_tPanelPublicConfig.m_PanelInitGpioFun("FrontPanel", &init_params);

		open_params.ReservedBits =(1 << (g_PanelFD650Config.m_chClkPin-32))
					| (1 << (g_PanelFD650Config.m_chDataPin-32));

		if(g_PanelFD650Config.m_chGpioLockEn)
		{
			open_params.ReservedBits |= (1 << g_PanelFD650Config.m_chLockPin);

			open_params.BitCfg[g_PanelFD650Config.m_chLockPin-32].Direction = GXPIO_BIT_OUTPUT;//Note:INput here
			open_params.BitCfg[g_PanelFD650Config.m_chLockPin-32].Pole = GXPIO_BIT_POSITIVE;
			open_params.BitCfg[g_PanelFD650Config.m_chLockPin-32].Level = GXPIO_BIT_HIGHT_LEVEL;
		}

		open_params.BitCfg[g_PanelFD650Config.m_chClkPin-32].Direction = GXPIO_BIT_OUTPUT;
		open_params.BitCfg[g_PanelFD650Config.m_chClkPin-32].Pole = GXPIO_BIT_POSITIVE;
		open_params.BitCfg[g_PanelFD650Config.m_chClkPin-32].Level = GXPIO_BIT_HIGHT_LEVEL;

		open_params.BitCfg[g_PanelFD650Config.m_chDataPin-32].Direction = GXPIO_BIT_OUTPUT;
		open_params.BitCfg[g_PanelFD650Config.m_chDataPin-32].Pole = GXPIO_BIT_POSITIVE;
		open_params.BitCfg[g_PanelFD650Config.m_chDataPin-32].Level = GXPIO_BIT_HIGHT_LEVEL;

		g_PanelFD650Config.m_tPanelPublicConfig.m_PanelOpenGpioFun("FrontPanel", &open_params, &g_PanelPublicConfig.m_nGpioHandle);*/
	}
	else
	{
		//bsp_printf("[gx_stb_panel_fd650_init_pin] panel gpio cfg error\n");
	}
}



void gx_stb_panel_fd650_isr(void)
{
	//bsp_printf("gx_stb_panel_fd650_isr \n");
#if defined(LINUX_OS)
	create_bsp_timer(gx_stb_panel_fd650_scan_manager,1);
#endif

	//bsp_printf("gx_stb_panel_fd650_isr\n");
	//char_dev_register(&gx3113_panel_dev, PANEL_NAME, NULL);
}

uint32_t gx_stb_panel_fd650_config(void)
{

uint32_t nKeyValue[8]={PANEL_KEY_POWER
				, PANEL_KEY_LEFT
				, PANEL_KEY_UP
				, PANEL_KEY_RIGHT
				, PANEL_KEY_OK
				, PANEL_KEY_MENU
				, PANEL_KEY_DOWN
				, PANEL_KEY_EXIT
				};

	uint8_t chLedBit[8]={1<<0
					, 1<<1
					, 1<<2
					, 1<<3
					, 1<<4
					, 1<<5
					, 1<<6
					, 1<<7
					};

	memset(g_PanelFD650Config.m_chPanelName, 0, sizeof(g_PanelFD650Config.m_chPanelName));
	sprintf((void*)g_PanelFD650Config.m_chPanelName, "%s", "FD650");//?
	memcpy(g_PanelFD650Config.m_nKeyValue,nKeyValue,sizeof(nKeyValue));
	memcpy(g_PanelFD650Config.m_chLedBit,chLedBit,sizeof(chLedBit));

    #if 0
	g_PanelFD650Config.m_chClkPin = 22;
	g_PanelFD650Config.m_chDataPin = 21;//��ͷ
    #endif
   	g_PanelFD650Config.m_chClkPin = PANEL_CLK_GPIO;//13;//23;
	g_PanelFD650Config.m_chDataPin = PANEL_DATA_GPIO;//12;//22;//meimeng
	g_PanelFD650Config.m_chStandbyPin = PANEL_STANDBY_GPIO;//11;

//	g_PanelFD650Config.m_chClkPin = 30;
//	g_PanelFD650Config.m_chDataPin = 29;
//	g_PanelFD650Config.m_chStandbyPin= 31;


	g_PanelFD650Config.m_nLedAddr[0]=0x1700;
	g_PanelFD650Config.m_nLedAddr[1]=0x1600;
	g_PanelFD650Config.m_nLedAddr[2]=0x1500;
	g_PanelFD650Config.m_nLedAddr[3]=0x1400;
    #if 0
	g_PanelFD650Config.m_nLedAddr[0]=0x1400;
	g_PanelFD650Config.m_nLedAddr[1]=0x1500;
	g_PanelFD650Config.m_nLedAddr[2]=0x1600;
	g_PanelFD650Config.m_nLedAddr[3]=0x1700;
    #endif
	g_PanelFD650Config.m_nKeyTranslateCode[0]=0x77;
	g_PanelFD650Config.m_nKeyTranslateCode[1]=0x47;
	g_PanelFD650Config.m_nKeyTranslateCode[2]=0x5f;
	g_PanelFD650Config.m_nKeyTranslateCode[3]=0x57;
	g_PanelFD650Config.m_nKeyTranslateCode[4]=0x67;
	g_PanelFD650Config.m_nKeyTranslateCode[5]=0x6f;
	g_PanelFD650Config.m_nKeyTranslateCode[6]=0x4f;
	g_PanelFD650Config.m_nKeyTranslateCode[7]=0x2000000;

	g_PanelFD650Config.m_chLockPin=0;

	g_PanelFD650Config.m_chSegmentLight=1;

	g_PanelFD650Config.m_chLedCnt = 4;
	g_PanelFD650Config.m_chCurLenCnt = 0;

	g_PanelFD650Config.m_nPortBaseAddr[PROT_BASE_LOW_ADDRESS] = EPORT_ADDR;
	g_PanelFD650Config.m_nPortBaseAddr[PROT_BASE_HIGH_ADDRESS] = 0;

	gx_stb_panel_public_config();
	memcpy(&g_PanelFD650Config.m_tPanelPublicConfig
		, &g_PanelPublicConfig
		, sizeof(GXPANEL_PUBLIC_Config_t));

	//com_gpio_port_clear(12);
	return 0;
}

GXPANEL_PANEL_ControlBlock_t gs_PanelControlBlock ={
	.m_PanelConfigFun = gx_stb_panel_fd650_config,
	.m_PanelIsrFun = gx_stb_panel_fd650_isr,
	.m_PanelInitPinFun = gx_stb_panel_fd650_init_pin,
	.m_PanelLockFun = gx_stb_panel_fd650_lock,
	.m_PanelSetSignalValueFun = gx_stb_panel_fd650_set_led_signal_value,
	.m_PanelSetStringFun = gx_stb_panel_fd650_set_led_string,
	.m_PanelSetValueFun = gx_stb_panel_fd650_set_led_value,
	.m_PanelUnlockFun = gx_stb_panel_fd650_unlock,
	.m_PanelShowModeFun = gx_stb_panel_fd650_show_mode,
	.m_PanelScanMangerFun = gx_stb_panel_fd650_scan_manager,
	.m_PanelPowerOffFun = gx_stb_panel_fd650_power_off,
};


#endif

