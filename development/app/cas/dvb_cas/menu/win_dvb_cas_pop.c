/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2012, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_ca_api.c
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	ģ��ͷ�ļ�
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2012.12.02		  zhouhm 	 			creation
*****************************************************************************/
#include "app_common_play.h"
#include "app_common_porting_stb_api.h"
#include "app_win_interface.h"
#include "app_dvb_cas_api_email.h"
#include "app_dvb_cas_api_demux.h"
#include "win_dvb_cas_pop.h"
#include "win_dvb_cas_rolling_message.h"
#include "win_dvb_cas_finger.h"
#include "win_dvb_cas_email.h"
#include "app_dvb_cas_api_pop.h"
#include "app_popmsg.h"
#include "app_common_search.h"
/* app_dvb_cas_show_ca_prompt��app_show_promptʵ�ֹ����п��ܴ��ڲ��*/
extern void app_show_prompt(char* context);
extern void app_hide_prompt(void);
void app_dvb_cas_show_ca_prompt(char* context)
{
	app_show_prompt(context);
	return;
}

/*app_dvb_cas_hide_ca_prompt��app_hide_promptʵ�ֹ����п��ܴ��ڲ��*/
void app_dvb_cas_hide_ca_prompt(void)
{
	app_hide_prompt();
	return;
}
void app_win_dvb_upgrade_freq(ST_DVBCA_FREQ *pstFreq, unsigned char ucFreqSum)
{
	int i;
	 popmsg_ret ret_pop = POPMSG_RET_YES;
	search_fre_list t_searchFreList;

	char* osd_language=NULL;
	osd_language = app_flash_get_config_osd_language();

	app_set_popmsg_yes_timeout(30);
	if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
	{
		ret_pop = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"�����½�Ŀ,�Ƿ��������?",POPMSG_TYPE_YESNO);
	}
	else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
	{
		ret_pop = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"Find new program,whether search?",POPMSG_TYPE_YESNO);
	}
	if(ret_pop == POPMSG_RET_YES)
	{
		for(i=0; i < ucFreqSum; i++)
		{
			t_searchFreList.app_fre_array[i] = pstFreq[i].uiFrequency/1000;
			t_searchFreList.app_qam_array[i] = pstFreq[i].ucModunation -1;//ö��������1
			t_searchFreList.app_symb_array[i] = pstFreq[i].uiSymbolrate;
			t_searchFreList.app_fre_tsid[i] = 0;
			printf("[debug] fre = %d mod = %d  sym = %d\n",pstFreq[i].uiFrequency,pstFreq[i].ucModunation,pstFreq[i].uiSymbolrate);
		}
		t_searchFreList.num = ucFreqSum;

		GUI_CreateDialog("win_search_result");
		GUI_SetInterface("flush", NULL);
		app_search_enum_enum_mode(t_searchFreList);
		printf("[debug] %s %d\n",__FUNCTION__,__LINE__);

	}
	printf("[debug] %s %d\n",__FUNCTION__,__LINE__);
}


/*�����㲥*/
bool g_win_popmsg_createflag = false;
static dvb_ca_urgent_message_st dvb_urgent_msg = {0};

//#define POP_DIALOG_X_START          210
//#define POP_DIALOG_Y_START          200
void win_dvb_cas_showDialog(unsigned char *title,unsigned char *content)
{
#ifdef APP_SD
	uint32_t x = 190;
	uint32_t y = 160;
#endif
#ifdef APP_HD

	uint32_t x = 340;
	uint32_t y = 200;
#endif
	if(content == NULL)
		return ;

	dvb_urgent_msg.title = title;
	dvb_urgent_msg.content = content;
	if(g_win_popmsg_createflag)
	{
		GUI_EndDialog("win_popmsg");
	}

	GUI_CreateDialog("win_popmsg");
	GUI_SetProperty("win_popmsg", "move_window_x", &x);
	GUI_SetProperty("win_popmsg", "move_window_y", &y);
	g_win_popmsg_createflag = true;
}

void win_dvb_cas_close_dialog()
{
	GUI_EndDialog("win_popmsg");
}

SIGNAL_HANDLER int win_popmsg_create(const char* widgetname, void *usrdata)
{
	if (dvb_urgent_msg.title != NULL)
	{
		GUI_SetProperty("win_popmsg_head", "string",(char *)dvb_urgent_msg.title);
	}
	if (dvb_urgent_msg.content != NULL)
	{
		GUI_SetProperty("win_popmsg_text", "string",(char *)dvb_urgent_msg.content);
	}
	return 0;
}

SIGNAL_HANDLER int win_popmsg_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;

	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
			case KEY_EXIT:
			case KEY_MENU:
			case KEY_RECALL:
			case KEY_OK:
				GUI_EndDialog("win_popmsg");
				g_win_popmsg_createflag = false;
				return EVENT_TRANSFER_STOP;
			default:
				return EVENT_TRANSFER_KEEPON;
		}
	}
	return EVENT_TRANSFER_KEEPON;
}

