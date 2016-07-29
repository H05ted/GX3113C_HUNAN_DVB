/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app.c
* Author    : 	lijq
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	ģ��ͷ�ļ�
******************************************************************************
* Release History:
	VERSION	Date			  AUTHOR         Description
	 1.0  	2010.04.19		  lijq 	 			creation
*****************************************************************************/
#include "gui_core.h"
#include "app.h"
#include "gxavdev.h"
#if defined(ECOS_OS)
#include "gxcore_hw_bsp.h"
#endif
#include "gxcore_version.h"
//#include "app_key.h"
#include "app_common_porting_ca_flash.h"
//#include "app_common_porting_ca_smart.h"
#include "gxgui_view.h"
#include "development_version.h"
#include "gxapp_sys_config.h"
#include "app_common_panel.h"
#ifdef DVB_AD_TYPE_BY_FLAG
#include "app_by_ads_porting_stb_api.h"
#endif
#ifdef DVB_AD_TYPE_DS_FLAG
#include "app_ds_ads_porting_stb_api.h"
#endif
/*wangjian add on 20141208*/

extern GuiWidgetOps file_image_ops;
handle_t g_app_msg_self = 0;
extern 	void app_porting_psi_create_filter_task(void);
extern 	int32_t GxDemux_Init(void);
//GUI�������
status_t app_msg_init(handle_t self)
{
/*
	if(s_unlisten_flag == 1)
	{

		// TODO: GxBus_MessageEmptyByOps(&g_service_list[SERVICE_GUI_VIEW]);
		*/
	g_app_msg_self = self;

	/*
	* ����������Ϣ
	*/
	GxBus_MessageListen(self, GXMSG_SEARCH_NEW_PROG_GET);//�½�Ŀ��Ϣ
	GxBus_MessageListen(self, GXMSG_SEARCH_SAT_TP_REPLY);// ��ǰ���������Ǻ� transport ��
	GxBus_MessageListen(self, GXMSG_SEARCH_STATUS_REPLY);//��������״̬��Ϣ
	GxBus_MessageListen(self, GXMSG_SEARCH_STOP_OK);
	/*
	* �����źż����Ϣ
	*/
	GxBus_MessageListen(self, GXMSG_FRONTEND_LOCKED);
	GxBus_MessageListen(self, GXMSG_FRONTEND_UNLOCKED);
	/*
	* ����CA��Ϣ
	*/
	GxBus_MessageRegister(GXMSG_CA_ON_EVENT, 
							  sizeof(GxMsgProperty0_OnEvent));  
	GxBus_MessageListen(self, GXMSG_CA_ON_EVENT);
	/*
	* ����ԤԼ��Ϣ
	*/
	GxBus_MessageListen(self, GXMSG_BOOK_TRIGGER);
	GxBus_MessageListen(self, GXMSG_BOOK_FINISH);
	/*
	* ����SI����Ϣ
	*/
	GxBus_MessageListen(self, GXMSG_SI_SUBTABLE_OK);
	GxBus_MessageListen(self, GXMSG_SI_SUBTABLE_TIME_OUT);
	/*
	* ����TDTʱ��ͬ����Ϣ
	*/
	GxBus_MessageListen(self, GXMSG_EXTRA_SYNC_TIME_OK);
	/*
	* ����������Ϣ
	*/
	GxBus_MessageListen(self, GXMSG_PLAYER_STATUS_REPORT);	
	GxBus_MessageListen(self, GXMSG_PLAYER_AVCODEC_REPORT);	
	GxBus_MessageListen(self, GXMSG_PLAYER_SPEED_REPORT);
	GxBus_MessageListen(self, GXMSG_PLAYER_RESOLUTION_REPORT);
	/*
	* ����USB��Ρ�������Ϣ
	*/
#ifdef USB_FLAG
	GxBus_MessageListen(self, GXMSG_UPDATE_STATUS);
    GxBus_MessageListen(self, GXMSG_HOTPLUG_IN);
	GxBus_MessageListen(self, GXMSG_HOTPLUG_OUT);
#endif

	/*
	}
	if(s_unlisten_flag > 0)
	{
		s_unlisten_flag--;
	}*/
	return GXCORE_SUCCESS;
}

status_t app_msg_destroy(handle_t self)
{
	GxBus_MessageUnListen(self, GXMSG_SEARCH_NEW_PROG_GET);
	GxBus_MessageUnListen(self, GXMSG_SEARCH_SAT_TP_REPLY);
	GxBus_MessageUnListen(self, GXMSG_SEARCH_STATUS_REPLY);
	GxBus_MessageUnListen(self, GXMSG_SEARCH_STOP_OK);
	GxBus_MessageUnListen(self, GXMSG_FRONTEND_LOCKED);
	GxBus_MessageUnListen(self, GXMSG_FRONTEND_UNLOCKED);
	GxBus_MessageUnListen(self, GXMSG_CA_ON_EVENT);
	GxBus_MessageUnListen(self, GXMSG_BOOK_TRIGGER);
	GxBus_MessageUnListen(self, GXMSG_BOOK_FINISH);
	GxBus_MessageUnListen(self, GXMSG_SI_SUBTABLE_OK);
	GxBus_MessageUnListen(self, GXMSG_SI_SUBTABLE_TIME_OUT);
	GxBus_MessageUnListen(self, GXMSG_EXTRA_SYNC_TIME_OK);
	GxBus_MessageUnListen(self, GXMSG_PLAYER_STATUS_REPORT);	
	GxBus_MessageUnListen(self, GXMSG_PLAYER_AVCODEC_REPORT);	
	GxBus_MessageUnListen(self, GXMSG_PLAYER_SPEED_REPORT);	
	GxBus_MessageUnListen(self, GXMSG_UPDATE_STATUS);
	GxBus_MessageUnListen(self, GXMSG_PLAYER_RESOLUTION_REPORT);


#ifdef USB_FLAG
	GxBus_MessageUnListen(self, GXMSG_UPDATE_STATUS);
    GxBus_MessageUnListen(self, GXMSG_HOTPLUG_IN);
	GxBus_MessageUnListen(self, GXMSG_HOTPLUG_OUT);
#endif
	return GXCORE_SUCCESS;
}

extern status_t signal_connect_handler(void);

/*wangjian add on 20141208*/
void app_ota_tuner_mainfreq(void)
{
	uint32_t nMainFreq = 0;
	uint32_t nMainFreq1 = 0;
	int32_t symb = 6875;
	int32_t qam = 2;
	status_t lockState = 0;
    uint8_t time = 0;

	nMainFreq = app_flash_get_config_center_freq();
	nMainFreq1 = app_flash_get_config_manual_search_freq(); 
	symb = app_flash_get_config_center_freq_symbol_rate();
	qam = app_flash_get_config_center_freq_qam();
	printf("[app_ota_tuner_mainfreq]lock main freq(%d %d %d).\n",\
			nMainFreq, symb, qam);

    GxFrontend_StopMonitor(0);
    while (time++<1)
    {
    	lockState = app_search_lock_tp(nMainFreq, symb, INVERSION_OFF, qam, 2000);
    	if (1 == lockState)
    	{
    		GxCore_ThreadDelay(1000);
			app_panel_show(PANEL_UNLOCK,0);
    		printf("[app_ota_tuner_mainfreq]lock main freq(%d) fail!!!\n", nMainFreq);
    	}
    	else
    	{
    		GxCore_ThreadDelay(2000);
			app_panel_show(PANEL_LOCK,0);
    		printf("[app_ota_tuner_mainfreq]lock main freq(%d) success@@@\n", nMainFreq);
            break;
    	}
		lockState = app_search_lock_tp(nMainFreq1, symb, INVERSION_OFF, qam, 2000);
    	if (1 == lockState)
    	{
    		GxCore_ThreadDelay(1000);

		app_panel_show(PANEL_UNLOCK,0);
    		printf("[app_ota_tuner_mainfreq]lock main freq(%d) fail!!!\n", nMainFreq);
    	}
    	else
    	{
    		GxCore_ThreadDelay(2000);
		app_panel_show(PANEL_LOCK,0);

    		printf("[app_ota_tuner_mainfreq]lock main freq(%d) success@@@\n", nMainFreq);
            break;
    	}
    }
    GxFrontend_StartMonitor(0);

	return;
}
//Ӧ�ó�ʼ���ӿ�
status_t app_init(void)
{
	VideoColor color;
	status_t ret = GXCORE_SUCCESS;
	struct tm Time = {0,0,0,1,1,2000,0,0,0};	
	int32_t Config;
	char* osd_language=NULL;
    char goxceed_version[20] = {0};

#if (DVB_DEMOD_GX1503 == DVB_DEMOD_TYPE)
	uint32_t bandWidth = app_flash_get_config_center_freq_bandwidth();
	uint32_t dtmb_dvbc = app_flash_get_config_dtmb_dvbc_switch();

	extern void GX1503_Set_BandWidth(unsigned char bandwidth);
	GX1503_Set_BandWidth(bandWidth);
	extern void GX1503_Set_WorkMode(char ChipWorkMode);
	GX1503_Set_WorkMode(dtmb_dvbc);
#endif

    GxBus_ConfigGet(GOXCEED_RELEASE_VERSION, goxceed_version, 20, GOXCEED_VERSON);
	if (0 != strcmp(goxceed_version,GOXCEED_VERSON))
		{
			/*
			* ��ǰӦ�������汾��֮ǰflash�еİ汾��ͬ
			* �������汾��������ʼ��ɾ�����н�Ŀ��ԤԼ����
			* �ָ�ϵͳ���õȽ������Ĭ��ֵ
			* �����ɸ��ݾ����г�ʵ��������
			*/
			GxBusPmDataSat sat;
			GxBus_PmLoadDefault(SAT_MAX_NUM,TP_MAX_NUM,SERVICE_MAX_NUM,NULL);
#if (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)
			sat.type = GXBUS_PM_SAT_C;
			sat.tuner = 0;
			GxBus_PmSatAdd(&sat);
			GxBus_PmSync(GXBUS_PM_SYNC_SAT);
#elif (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
			sat.type = GXBUS_PM_SAT_DTMB;
			sat.tuner = 0;
			sat.sat_dtmb.work_mode = app_flash_get_config_dtmb_dvbc_switch();
			GxBus_PmSatAdd(&sat);
			GxBus_PmSync(GXBUS_PM_SYNC_SAT);
#endif
			app_book_clear_all();
			app_flash_save_default_config_para();
			GxBus_ConfigSet(GOXCEED_RELEASE_VERSION, (const char*)GOXCEED_VERSON);
		}

	app_prog_init_playing();
	app_init_frontend(FRONT_MONITER_DURATION);
	app_init_panel();

	/*
	* �����޽�Ŀ�Ƿ��������ر�־
	*/
	app_search_set_auto_flag(TRUE);
	app_sys_init();
    INFO_MSG("Build Time:%s %s\n",__DATE__, __TIME__);

#ifdef DEVELOPMENT_SVN
    INFO_MSG("development svn : %d \n",DEVELOPMENT_SVN);
	INFO_MSG("%s\n\n",DEVELOPMENT_SVN_URL);
#endif

#ifdef DEVELOPMENT_GIT
    INFO_MSG("development git : %d \n",DEVELOPMENT_GIT);
	INFO_MSG("%s\n\n",DEVELOPMENT_GIT_VER);
#endif


#if AUDIO_DOLBY
    app_play_register_dolby();
#endif

	/*signal connect*/
	ret = signal_connect_handler();
	if(GXCORE_SUCCESS != ret)
	{
		printf("[signal_connect_handler] ERROR\n");
	}

#ifndef LOGO_SHOW_DELAY
#if defined(ECOS_OS)
	widget_register("file_image", &file_image_ops);
	ret = GUI_Init("/theme/theme.conf");
#elif defined(LINUX_OS)
	widget_register("file_image", &file_image_ops);
	ret = GUI_Init(WORK_PATH"theme/theme.conf");
#endif	
	if(GXCORE_SUCCESS != ret)
	{
		printf("=GUI_Init= ERROR\n");
		return GXCORE_ERROR;
	}
#endif

	/*
	  * ����������Ӧ�������ʾ����ʱ2s�رտ�������
	  */
	GxAvdev_SetI2cAckWaitTime(3000);

#if (PANEL_TYPE == PANEL_TYPE_2KEY)
	app_panel_set_gpio_low(34);
#endif
	/*
	  * ����������Ӧ�������ʾ����ʱ2s�رտ�������
	  */
#ifdef DVB_AD_TYPE_BY_FLAG
	app_by_ads_logo_sleep(5000);
#elif defined (DVB_AD_TYPE_DS_FLAG)
    printf("close the ad logo.\n\n");
	app_ds_ad_logo_sleep(5000);
#elif defined DVB_AD_TYPE_MAIKE_FLAG
	app_maike_ads_logo_sleep(5000);
#else
#if (1 == DVB_JPG_LOGO)
//	GxCore_ThreadDelay(5000);
//	advertisement_spp_layer_close();
#else
	GxCore_ThreadDelay(5000);
	app_play_close_av_logo();
#endif
#endif  

	app_epg_set_local_time(&Time,TRUE);
	app_prog_set_view_info_enable();

#ifndef LOGO_SHOW_DELAY
	Config = app_flash_get_config_osd_trans();
	app_play_set_osd_trasn_level(Config);
	#endif

	/*
	* set video color
	*/
	color.brightness = app_flash_get_config_videocolor_brightness();
	color.saturation = app_flash_get_config_videocolor_saturation();
	color.contrast = app_flash_get_config_videocolor_contrast();
	app_play_set_videoColor_level(color);

	//�������л�ȡ����������Ϣ
	#ifndef LOGO_SHOW_DELAY
	osd_language = app_flash_get_config_osd_language();
	app_play_set_osd_language(osd_language);
	#endif
	
	app_book_init();
	GxFrontend_StartMonitor(0);

    
	//app_demodu_monitor_start();

	/*
	* ע��ԤԼ����ص�����(����ڡ�ԤԼ�������)
	*/
	app_book_register_add_event_popmsg_callback(app_book_add_event_pop_msg_call_back);
	/*
	* ע���ѯԤԼʱ�䵽�ص�����(ȷ�ϡ�ȡ��ԤԼ�ۿ���)
	*/
       app_book_register_play_popmsg_callback(app_book_confirm_play_pop_msg_call_back);
#if DVB_PVR_FLAG
	/*
	* ע���ѯԤԼʱ�䵽�ص�����(ȷ�ϡ�ȡ��ԤԼ¼�Ƶ�)
	*/
       app_book_register_record_popmsg_callback(app_book_confirm_record_pop_msg_call_back);
#endif
	/*
	* ע�����������չ��ص�����(SDT\NIT����չ�����)
	*/
	app_search_register_add_extend_table_callback(app_search_add_extend_table_call_back);
	/*
	* ע����ʾ��Ϣ��ʾ�ص�����(���źš����Ž�Ŀ����ʾ)
	*/
	app_play_register_show_prompt_msg_callback(app_win_prompt_msg_exec_call_back);
	app_play_register_password_widget_window("win_password_input");
	/*
	* ע���̨���NIT����������ӻص���(OTA������LCN��̬���µ�)
	*/
	app_table_nit_register_parse_descriptor_callback(app_table_nit_parse_descriptor_call_back);

	/*
	* CA filter���̨���filter(��PMT\CAT\NIT��)��Դ��ʼ��
	*/
#ifdef CA_FLAG
//OTAǿ������ca����
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
	app_flash_save_config_dvb_ca_flag(DVB_CA_TYPE_DSCAS50);
#endif
#ifdef DVB_CA_TYPE_MG312_FLAG
	app_flash_save_config_dvb_ca_flag(DVB_CA_TYPE_MG312);
#endif
#ifdef DVB_CA_TYPE_MG_FLAG
	app_flash_save_config_dvb_ca_flag(DVB_CA_TYPE_MG);
#endif
#ifdef DVB_CA_TYPE_DIVITONE_FLAG
	app_flash_save_config_dvb_ca_flag(DVB_CA_TYPE_DIVITONE);
#endif
#ifdef DVB_CA_TYPE_DVB_FLAG
	app_flash_save_config_dvb_ca_flag(DVB_CA_TYPE_DVB);
#endif


	app_cas_update_dvb_ca_flag();
#endif
	GxDemux_Init();
	app_porting_psi_create_filter_task();
	app_table_nit_monitor_filter_open();
 
	   
#ifdef CA_FLAG
	app_cas_init();//CAģ���ʼ��
#endif
    
	/* gpio un mute low*/
	app_play_set_gpio_unmute();
#ifdef LOGO_SHOW_DELAY
#if defined(ECOS_OS)
	widget_register("file_image", &file_image_ops);
	ret = GUI_Init("/theme/theme.conf");
#elif defined(LINUX_OS)
	widget_register("file_image", &file_image_ops);
	ret = GUI_Init(WORK_PATH"theme/theme.conf");
#endif	
	if(GXCORE_SUCCESS != ret)
	{
		printf("=GUI_Init= ERROR\n");
		return GXCORE_ERROR;
	}
    
    Config = app_flash_get_config_osd_trans();
	app_play_set_osd_trasn_level(Config);
    	//�������л�ȡ����������Ϣ
	osd_language = app_flash_get_config_osd_language();
	app_play_set_osd_language(osd_language);
  app_audio_video_init();
#endif
#if (DVB_DEFINITION_TYPE == SD_DEFINITION)
    //app_play_set_videoColor_level(color);



    if(app_flash_get_config_video_hdmi_mode() == VIDEO_OUTPUT_NTSC_M)
    {
        //�޸�#14186
        app_play_set_hdmi_mode(VIDEO_OUTPUT_PAL);
        app_play_set_hdmi_mode(VIDEO_OUTPUT_NTSC_M);
    }
	app_play_set_videoColor_level(color);

#endif
	/*˽�б��OTA������������
	* 
	*/
	// TODO:  ���ͻ������ư汾ʱһ��Ҫע�������PID��table id
#ifdef OTA_PID
	app_table_ota_monitor_filter_open(OTA_PID,OTA_TABLEID);
#else
	app_table_ota_monitor_filter_open(0x1e61,0x88);
	#endif

	//close the log before create full screen.
	 printf("close the startlog.\n\n");
	app_play_close_av_logo();

#if (DVB_CHIP_TYPE == GX_CHIP_3113C)||(DVB_CHIP_TYPE == GX_CHIP_3115)
		*(unsigned int*)0xa48000a8 = 0x20c020;
#endif

#if  FACTORY_SERIALIZATION_SUPPORT
	extern int app_factory_serialization(void);
	app_factory_serialization();
#else
	GUI_CreateDialog("win_full_screen");
#endif

	/*Must be called later than app_cas_init()*/
#ifdef DVB_AD_TYPE_DS_FLAG
	app_ds_ad_init();
#endif
#ifdef DVB_AD_TYPE_MAIKE_FLAG
	extern int mad_init(void);
	mad_init();
	mad_ts_task_init();
	mad_ads_client_init();
	start_monitor_mad();
#endif


	return GXCORE_SUCCESS;
}


