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
1.0  	2013.06.26		  zhouhm 	 			creation
*****************************************************************************/
#include <gxtype.h>
#include "app_common_porting_stb_api.h"
#include "gx_demux.h"
#include "gui_core.h"
#include "gui_timer.h"
#include "gxapp_sys_config.h"
#include "app_desai_cas_5.0_api_pop.h"
#include "app_ds_ads_porting_stb_api.h"
#include "app_common_prog.h"
#include "app_common_porting_ca_os.h"
 
static uint8_t flashMapBuff[DS_AD_FLASH_LEN];
static ST_DSAD_LOG_INFO logo_pic_config = {0,};
static DS_AD_PIC_t epg_ad_pic ;
static DS_AD_PIC_t full_screen_ad_pic;
static DS_AD_PIC_t unauthorized_screen_ad_pic;
static DS_AD_PIC_t menu_ad_pic;
static DS_AD_PIC_t volume_ad_pic;

static BOOL full_screen_ad_is_showing = FALSE;
static BOOL unauthorized_ad_is_showing = FALSE;
static EN_DSAD_OSD_SHOW_POS osd_showing_type = SHOW_UNKNOWN_TYPE_AD;

static uint16_t ad_pid = 0x1fff;
static uint8_t osd_bottom_widget_name[DS_AD_WIDGET_NAME_LEN];
static uint8_t osd_top_widget_name[DS_AD_WIDGET_NAME_LEN];
//static uint8_t osd_buf_top[DS_AD_OSD_BUFFER_LEN];
//static uint8_t osd_buf_bottom[DS_AD_OSD_BUFFER_LEN];

extern BOOL full_screen_ad_need_show;
static BOOL program_is_unauthorized;
extern ST_DSAD_PROGRAM_SHOW_INFO full_screen_pic_info;
static handle_t ds_ad_osd_sem ;

static void ds_ad_rolling_osd_show(uint8_t byStyle,uint8_t showFlag,char* message);

static void app_ds_ad_flash_init(void)
{
	static uint8_t inited = 0;
	handle_t flash_ad_file;
	ssize_t size;
	if(inited)
	{
		return;
	}
	else
	{
		inited = 1;
	}

	memset(flashMapBuff,0,sizeof(uint8_t)*DS_AD_FLASH_LEN);
	memset(&logo_pic_config,0,sizeof(ST_DSAD_LOG_INFO));
#ifdef ECOS_OS
	cyg_scheduler_lock();
#endif
	/*Read FLASH data to memory*/
	flash_ad_file = GxCore_Open(DS_AD_FLASH_FILE, "a+");
	GxCore_Seek(flash_ad_file, 0, GX_SEEK_SET);
	size =  GxCore_Read(flash_ad_file, (void*)flashMapBuff, 1, DS_AD_FLASH_LEN);
	GxCore_Close(flash_ad_file);
	/*Read logo config data to memory*/
	flash_ad_file = GxCore_Open(DS_AD_LOGO_CONFIG_FILE, "a+");
	GxCore_Seek(flash_ad_file, 0, GX_SEEK_SET);
	size =	GxCore_Read(flash_ad_file, (void*)&logo_pic_config, 1, sizeof(ST_DSAD_LOG_INFO));
	GxCore_Close(flash_ad_file);
	/*Read logo data to memory*/
	/*Read the data when shown...*/

#ifdef ECOS_OS
	cyg_scheduler_unlock();
#endif
}
void app_ds_ad_init(void)
{
	BOOL bRet = 0;
	uint8_t adVersion[25] = {0,};

	app_porting_ca_os_sem_create(&ds_ad_osd_sem,1);

	memset(osd_bottom_widget_name,0,sizeof(uint8_t)*DS_AD_WIDGET_NAME_LEN);
	memset(osd_top_widget_name,0,sizeof(uint8_t)*DS_AD_WIDGET_NAME_LEN);

	memset(&epg_ad_pic,0,sizeof(DS_AD_PIC_t));
	memset(&full_screen_ad_pic,0,sizeof(DS_AD_PIC_t));
	memset(&unauthorized_screen_ad_pic,0,sizeof(DS_AD_PIC_t));
	memset(&menu_ad_pic,0,sizeof(DS_AD_PIC_t));
	memset(&volume_ad_pic,0,sizeof(DS_AD_PIC_t));
	bRet = DSAD_Init(GXOS_DEFAULT_PRIORITY,DS_AD_FLASH_START_ADDR,DSAD_STB_SD);
	if(!bRet)
	{
		ADS_ERROR("DSADS init failed\n");
		return;
	}
	DSAD_GetADVersion(adVersion);
	ADS_Dbg("DS-ADS VERSION:	%s\n",adVersion);
	// TODO:For test without NIT.
//	DSAD_SetDataPid(0x1ff1);

}
uint8_t app_ds_ad_show_av_logo(int32_t VideoResolution)
{
    ST_DSAD_LOG_INFO LogoInfo;
	ST_DSAD_AV_SHOW_INFO stAvType;
	ST_DSAD_PICTURE_SHOW_INFO stPictureType;

	app_ds_ad_flash_init();
	memcpy(&LogoInfo,&logo_pic_config,sizeof(ST_DSAD_LOG_INFO));
	if(LogoInfo.ucLogType == 2)
	{
		memcpy(&stAvType,&LogoInfo.Element,sizeof(ST_DSAD_AV_SHOW_INFO));
		/*Show I frame*/
		app_play_i_frame_logo(VideoResolution,DS_AD_LOGO_I_FRAME_FILE);
        return 1;
	}
	else if(LogoInfo.ucLogType == 1)
	{
		memcpy(&stPictureType,&LogoInfo.Element,sizeof(ST_DSAD_PICTURE_SHOW_INFO));
		ADS_FAIL("Do not support logo type %d!!!\n",LogoInfo.ucLogType);
        return 0;
	}
	else
	{
		ADS_FAIL("Do not support logo type %d!!!\n",LogoInfo.ucLogType);
        return 0;
	}

}
void app_ds_ad_logo_sleep(uint32_t ms)
{
    ST_DSAD_LOG_INFO LogoInfo;
	ST_DSAD_AV_SHOW_INFO stAvType;
	ST_DSAD_PICTURE_SHOW_INFO stPictureType;

	app_ds_ad_flash_init();
	if (GxCore_FileExists(DS_AD_LOGO_CONFIG_FILE) == 1)
	{
		memcpy(&LogoInfo,&logo_pic_config,sizeof(ST_DSAD_LOG_INFO));
		if(LogoInfo.ucLogType == 1)
		{
			memcpy(&stPictureType,&LogoInfo.Element,sizeof(ST_DSAD_PICTURE_SHOW_INFO));
			if(stPictureType.enPicType == DSAD_PIC_GIF)
			{
				/*Close JPEG picture*/
				advertisement_spp_layer_close();
			}
			else
			{
				/*Close I frame*/
				app_play_close_av_logo();
			}
		}
		else if(LogoInfo.ucLogType == 2)
		{
			memcpy(&stAvType,&LogoInfo.Element,sizeof(ST_DSAD_AV_SHOW_INFO));
		}
		else
		{
			ADS_FAIL("Do not support logo type %d!!!\n",LogoInfo.ucLogType);
		}
	}
	else
	{
		GxCore_ThreadDelay(ms);
	#if (1 == DVB_JPG_LOGO)
		advertisement_spp_layer_close();
	#else
		app_play_close_av_logo();
	#endif
	}
}
uint8_t app_ds_ad_flash_read(uint32_t puiStartAddr,  uint8_t *pucData,  uint32_t uiLen)
{
	ADS_Dbg("%s>>puiStartAddr=0x%x,pucData=%p,uiLen=%d\n",
		__FUNCTION__,puiStartAddr,pucData,uiLen);
	memcpy(pucData,&flashMapBuff[puiStartAddr-DS_AD_FLASH_START_ADDR],uiLen);
	return TRUE;

}
uint8_t app_ds_ad_flash_write(uint32_t puiStartAddr,  uint8_t *pucData,  uint32_t uiLen)
{
	ADS_Dbg("%s>>puiStartAddr=0x%x,pucData=%p,uiLen=%d\n",
		__FUNCTION__,puiStartAddr,pucData,uiLen);
	memcpy(&flashMapBuff[puiStartAddr-DS_AD_FLASH_START_ADDR],pucData,uiLen);

	app_flash_save_ad_data_to_flash_file((const char*)flashMapBuff,DS_AD_FLASH_LEN,DS_AD_FLASH_FILE);
	return TRUE;
}
uint8_t app_ds_ad_flash_erase(uint32_t puiStartAddr,  uint32_t uiLen)
{
	ADS_Dbg("%s>>puiStartAddr=0x%x,uiLen=%d\n",__FUNCTION__,puiStartAddr,uiLen);
	memset(&flashMapBuff[puiStartAddr-DS_AD_FLASH_START_ADDR],0xff,uiLen);
	app_flash_save_ad_data_to_flash_file((const char*)flashMapBuff,DS_AD_FLASH_LEN,DS_AD_FLASH_FILE);
	return TRUE;
}

static void app_ds_ad_show_gif_callback(void * data)
{
	status_t sret = 0;
	int alu = GX_ALU_ROP_COPY;//GX_ALU_ROP_COPY_INVERT;
	DS_AD_PIC_t * ptr = (DS_AD_PIC_t *)data;
    char *focus_window = NULL;

	if(!ptr)
	{
		return;
	}

    /* avoid temporay dialog box when show main menu ad */
    if(strcasecmp("win_main_menu_image_ad", ptr->IconName) == 0)
    {
        focus_window = (char*)GUI_GetFocusWindow();

        if(strcasecmp("win_main_menu", focus_window) != 0)
            return;
    }
	if(ptr->isShown == 0)
	{
		sret = GUI_SetProperty(ptr->IconName, "state", "show");
		ADS_Dbg("l=%d,sret = %d\n",__LINE__,sret);/**/
		ptr->isShown = 1;
		return;
	}
	if(ptr->isShown == 1 && ptr->ptrTimer)
	{
		ptr->ptrTimer->delta_time = 800;
	}
	sret = GUI_SetProperty(ptr->IconName, "draw_gif", (void *)&alu);
	GUI_SetInterface("flush", NULL);
	if(sret)
	{
		ADS_Dbg("l=%d,sret = %d\n",__LINE__,sret);
	}
}
static void app_ds_ad_set_widget_xywh(char * widgetName,int x,int y,int w,int h)
{
	status_t sret = 0;
	sret = GUI_SetProperty(widgetName,(char *)"x",(void *)&x);
	sret = GUI_SetProperty(widgetName,(char *)"y",(void *)&y);
	sret = GUI_SetProperty(widgetName,(char *)"width",(void *)&w);
	sret = GUI_SetProperty(widgetName,(char *)"height",(void *)&h);
}


static void do_osd_show(ST_DSAD_OSD_PROGRAM_SHOW_INFO * osd_info,uint8_t curIndex)
{
	int32_t i = 0;
	//char * buf = NULL;
	uint8_t index = 0;
	ST_DSAD_OSD_SHOW_INFO * ad_osd = NULL;
	printf("TRACK %s %d IN\n",__FUNCTION__,__LINE__);
	if(osd_info == NULL)
	{
		return;
	}
	if(curIndex > osd_info->ucOsdNum)
	{
		return;
	}
	for(i = 0;i < 15 ;i++)
	{
		if(osd_info->aucFlag[i])
		{
			index++;
		}
		if(index == curIndex)
		{
			ad_osd = &osd_info->stTextShow[i];
			break;
		}
	}
	if(ad_osd == NULL)
	{
		return;
	}
	#if 0
	if(SHOW_UNKNOWN_TYPE_AD==osd_showing_type)
	{
		app_ds_ad_hide_osd();
	}

	if((ad_osd->enDisplayPos == SHOW_TOP) || (ad_osd->enDisplayPos == SHOW_TOP_BOTTOM))
	{
		memset(osd_buf_top,0,sizeof(uint8_t)*DS_AD_OSD_BUFFER_LEN);
		if(ad_osd->usContentSize >= DS_AD_OSD_BUFFER_LEN)
		{
			memcpy(osd_buf_top,ad_osd->pucDisplayBuffer,DS_AD_OSD_BUFFER_LEN);
			osd_buf_top[DS_AD_OSD_BUFFER_LEN-1]='\0';
		}
		else
		{
			memcpy(osd_buf_top,ad_osd->pucDisplayBuffer,ad_osd->usContentSize);
			osd_buf_top[ad_osd->usContentSize]='\0';
		}
		GUI_SetProperty(osd_top_widget_name,"rolling_stop",NULL);
		printf("osd_buf_top=%s\n",osd_buf_top);
		GUI_SetProperty(osd_top_widget_name,"string", osd_buf_top);
		GUI_SetProperty(osd_top_widget_name,"state", "hide");
		if(ad_osd->enDisplayPos == SHOW_TOP)
		{
			GUI_SetProperty(osd_bottom_widget_name,"rolling_stop",NULL);
			GUI_SetProperty(osd_bottom_widget_name,"string", " ");
			GUI_SetProperty(osd_bottom_widget_name,"state", "hide");
		}
		GUI_SetProperty(osd_top_widget_name,"state", "show");
		osd_showing_type = ad_osd->enDisplayPos;
	}
	if((ad_osd->enDisplayPos == SHOW_BOTTOM) || (ad_osd->enDisplayPos == SHOW_TOP_BOTTOM))
	{
		memset(osd_buf_bottom,0,sizeof(uint8_t)*DS_AD_OSD_BUFFER_LEN);
		if(ad_osd->usContentSize >= DS_AD_OSD_BUFFER_LEN)
		{
			memcpy(osd_buf_bottom,ad_osd->pucDisplayBuffer,DS_AD_OSD_BUFFER_LEN);
			osd_buf_bottom[DS_AD_OSD_BUFFER_LEN-1]='\0';
		}
		else
		{
			memcpy(osd_buf_bottom,ad_osd->pucDisplayBuffer,ad_osd->usContentSize);
			osd_buf_bottom[ad_osd->usContentSize]='\0';
		}
		GUI_SetProperty(osd_bottom_widget_name,"rolling_stop",NULL);
		printf("osd_buf_bottom=%s\n",osd_buf_bottom);
		GUI_SetProperty(osd_bottom_widget_name,"string",osd_buf_bottom);
		GUI_SetProperty(osd_bottom_widget_name,"state", "hide");
		if(ad_osd->enDisplayPos == SHOW_BOTTOM)
		{
			GUI_SetProperty(osd_top_widget_name,"rolling_stop",NULL);
			GUI_SetProperty(osd_top_widget_name,"string", " ");
			GUI_SetProperty(osd_top_widget_name,"state", "hide");
		}
		GUI_SetProperty(osd_bottom_widget_name,"state", "show");
		osd_showing_type = ad_osd->enDisplayPos;
	}
	#else
	static desai_cas50_osd_data_st osdinfo;
	static ST_OSD_SHOW_INFO data;
	static uint8_t str[1024];

	data.ucDisplayPos = ad_osd->enDisplayPos;
	memset(str,0,sizeof(uint8_t)*1024);
	memcpy(data.aucBackgroundColor ,ad_osd->aucBackgroundColor,4);
	memcpy(data.aucDisplayColor ,ad_osd->aucDisplayColor,4);
	memcpy(str,ad_osd->pucDisplayBuffer, ad_osd->usContentSize);

	data.pucDisplayBuffer = str;
	data.ucFontSize = ad_osd->ucFontSize;
	data.usContentSize = strlen((char *)str);
	data.usGetBufferSize = ad_osd->usGetBufferSize;
	data.ucFontType = ad_osd->ucFontType;

	osdinfo.wStatus = TRUE;
	osdinfo.roll_status = FALSE;
	osdinfo.buffer = (uint8_t *)&data;
	osdinfo.from  = 1;
	app_cas_api_on_event(DVB_CA_ROLLING_INFO,CAS_NAME, &osdinfo, sizeof(desai_cas50_osd_data_st));
	osd_showing_type = ad_osd->enDisplayPos;

#endif
}

//the function is setted as local one
static uint8_t app_ds_ad_show_pic(DS_AD_PIC_OPT opt,char * IconName,EN_DSAD_POS_TYPE adType)
{
	GxBusPmDataProg prog={0};
	uint32_t ppos = 0;
	ST_DSAD_PROGRAM_PARA stProgramPara={0};
	ST_DSAD_PROGRAM_SHOW_INFO stProgramShow = {{0},{0},{0}};
	EN_DSAD_FUNCTION_TYPE ret = DSAD_FUN_OK;
	uint8_t bRet = 1;
	int alu = GX_ALU_ROP_COPY;
	status_t sret = DSAD_FUN_OK;
	DS_AD_PIC_t * ptrPic = NULL;
	int x,y,w,h;

	switch (adType)
	{
		case DSAD_STB_EPG:
		{
			ptrPic = &epg_ad_pic;
			ptrPic->filePath = DS_AD_EPG_FILE_PATH;
			break;
		}
		case  DSAD_STB_VOLUME:
		{
			ptrPic = &volume_ad_pic;
			ptrPic->filePath = DS_AD_VOLUME_FILE_PATH;
			break;
		}
		case  DSAD_STB_FULLSCREEN:
		{
			ptrPic = &full_screen_ad_pic;
			ptrPic->filePath = DS_AD_FULLSCREEN_FILE_PATH;
			break;
		}
		case  DSAD_STB_UNAUTHORIZED:
		{
			ptrPic = &unauthorized_screen_ad_pic;
			ptrPic->filePath = DS_AD_UNAUTHORIZED_FILE_PATH;
			break;
		}
		case  DSAD_STB_MENU:
		{
			ptrPic = &menu_ad_pic;
			ptrPic->filePath = DS_AD_MENU_FILE_PATH;
			break;
		}
		default:
		{
			bRet = 0;
			return bRet;
		}
	}


	if(opt == LOAD_PIC_DATA)/*load*/
	{
		if((adType == DSAD_STB_EPG)||(adType == DSAD_STB_VOLUME))
		{
			app_prog_get_playing_pos_in_group(&ppos);
			GxBus_PmProgGetByPos(ppos,1,&prog);
			stProgramPara.usNetworkId = prog.original_id;
			stProgramPara.usTsId      = prog.ts_id;
			stProgramPara.usServiceId = prog.service_id;
			ADS_Dbg("original_id=%d,ts_id=%d,service_id=%d\n",prog.original_id,prog.ts_id,prog.service_id);
			ret = DSAD_ProgramGetElement(stProgramPara,&stProgramShow,adType);
		}
		else if(adType == DSAD_STB_FULLSCREEN)
		{
			memcpy(&stProgramShow,&full_screen_pic_info,sizeof(ST_DSAD_PROGRAM_SHOW_INFO));
			ret = DSAD_FUN_OK;
		}
		else
		{
			ret = DSAD_NoProgramGetElement(&stProgramShow,adType);
		}

		if(ret == DSAD_FUN_OK)
		{
			strcpy(ptrPic->IconName,IconName);
			/*Copy pic data to memory*/
			app_save_data_to_ddram_file((char *)stProgramShow.stPictureShow.pucPicData,
            				            stProgramShow.stPictureShow.uiDataLen,
            				            ptrPic->filePath);
			/*Relocate AD widget*/
			if ((adType == DSAD_STB_FULLSCREEN) || (adType == DSAD_STB_UNAUTHORIZED))
			{
				x = stProgramShow.stPictureShow.usStartX;
				y = stProgramShow.stPictureShow.usStartY;
				w = stProgramShow.stPictureShow.usWidth;
				h = stProgramShow.stPictureShow.usHeigth;
				app_ds_ad_set_widget_xywh(ptrPic->IconName,x,y,w,h);
			}
			/*Load and GIF data to widget*/
			sret = GUI_SetProperty((char *)ptrPic->IconName,(char *)"load_zoom_img"/*"load_img"*/,(void *)ptrPic->filePath);
			ADS_Dbg("l=%d,sret = %d\n",__LINE__,sret);
			sret = GUI_SetProperty(ptrPic->IconName, "init_gif_alu_mode", (void *)&alu);
			ADS_Dbg("l=%d,sret = %d\n",__LINE__,sret);
			sret = GUI_SetProperty(ptrPic->IconName, "draw_gif", &alu);
			ADS_Dbg("l=%d,sret = %d\n",__LINE__,sret);
			//sret = GUI_SetProperty(ptrPic->IconName, "state", "show");
			ADS_Dbg("l=%d,sret = %d\n",__LINE__,sret);
			/*Create GIF update timer*/
			if(ptrPic->ptrTimer)
			{
				remove_timer(ptrPic->ptrTimer);
			}

			ptrPic->ptrTimer = create_timer((timer_event)app_ds_ad_show_gif_callback,10,ptrPic,TIMER_REPEAT);
			bRet = 1;
		}
		else
		{
			ADS_ERROR("DSAD_ProgramGetElement error code = %d\n",ret);
			bRet = 0;
		}
	}
	else if(opt == SHOW_PIC_DATA)/*show*/
	{
		sret = GUI_SetProperty(ptrPic->IconName, "state", "show");
		ADS_Dbg("l=%d,sret = %d\n",__LINE__,sret);
	}
	else if (opt == DRAW_PIC_DATA)/*draw*/
	{
		sret = GUI_SetProperty(ptrPic->IconName, "draw_gif", &alu);
		ADS_Dbg("l=%d,sret = %d\n",__LINE__,sret);
	}
	else if (opt == HIDE_PIC_DATA)/*hide*/
	{
		sret = GUI_SetProperty(ptrPic->IconName, "state", "hide");
		ADS_Dbg("l=%d,sret = %d\n",__LINE__,sret);
	}
	else if (opt == FREE_PIC_DATA)/*free*/
	{
		if(ptrPic->ptrTimer)
		{
			remove_timer(ptrPic->ptrTimer);
		}
		sret = GUI_SetProperty((char *)ptrPic->IconName,(char *)"load_img",(void *)NULL);
		ADS_Dbg("l=%d,sret = %d\n",__LINE__,sret);
		sret = GUI_SetProperty((char *)ptrPic->IconName, (char *)"state", "hide");
    	GUI_SetInterface("flush", NULL);

		ADS_Dbg("l=%d,sret = %d\n",__LINE__,sret);
		memset(ptrPic,0,sizeof(DS_AD_PIC_t));
	}

	return bRet;
}
void app_ds_ad_menu_pic_display(char * IconeName)
{
	app_porting_ca_os_sem_wait(ds_ad_osd_sem);
	app_ds_ad_show_pic(LOAD_PIC_DATA,IconeName,DSAD_STB_MENU);
	app_porting_ca_os_sem_signal(ds_ad_osd_sem);
}
void app_ds_ad_hide_menu_pic(void)
{
	app_porting_ca_os_sem_wait(ds_ad_osd_sem);
	app_ds_ad_show_pic(HIDE_PIC_DATA,NULL,DSAD_STB_MENU);
	app_ds_ad_show_pic(FREE_PIC_DATA,NULL,DSAD_STB_MENU);
	app_porting_ca_os_sem_signal(ds_ad_osd_sem);
}
void app_ds_ad_epg_pic_display(char * IconeName)
{
	app_porting_ca_os_sem_wait(ds_ad_osd_sem);
	app_ds_ad_show_pic(LOAD_PIC_DATA,IconeName,DSAD_STB_EPG);
	app_porting_ca_os_sem_signal(ds_ad_osd_sem);
}
void app_ds_ad_hide_epg_pic(void)
{
	app_porting_ca_os_sem_wait(ds_ad_osd_sem);
	app_ds_ad_show_pic(HIDE_PIC_DATA,NULL,DSAD_STB_EPG);
	app_ds_ad_show_pic(FREE_PIC_DATA,NULL,DSAD_STB_EPG);
	app_porting_ca_os_sem_signal(ds_ad_osd_sem);
}
void app_ds_ad_vol_pic_display(char * IconeName)
{
	app_porting_ca_os_sem_wait(ds_ad_osd_sem);
	app_ds_ad_show_pic(LOAD_PIC_DATA,IconeName,DSAD_STB_VOLUME);
	app_porting_ca_os_sem_signal(ds_ad_osd_sem);
}

void app_ds_ad_hide_vol_pic(void)
{
	app_porting_ca_os_sem_wait(ds_ad_osd_sem);
	app_ds_ad_show_pic(HIDE_PIC_DATA,NULL,DSAD_STB_VOLUME);
	app_ds_ad_show_pic(FREE_PIC_DATA,NULL,DSAD_STB_VOLUME);
	app_porting_ca_os_sem_signal(ds_ad_osd_sem);
}

void app_ds_ad_full_screen_pic_display(char * IconeName)
{
	app_porting_ca_os_sem_wait(ds_ad_osd_sem);
	do{
	if(full_screen_ad_need_show == TRUE && full_screen_ad_is_showing == FALSE)
	{
		if(unauthorized_ad_is_showing)
		{
			break;
		}
		app_ds_ad_show_pic(LOAD_PIC_DATA,IconeName,DSAD_STB_FULLSCREEN);
//		app_ds_ad_show_pic(SHOW_PIC_DATA,IconeName,DSAD_STB_FULLSCREEN);
		full_screen_ad_is_showing = TRUE;
	}
	else if(full_screen_ad_need_show == FALSE && full_screen_ad_is_showing == TRUE)
	{
		app_ds_ad_show_pic(HIDE_PIC_DATA,IconeName,DSAD_STB_FULLSCREEN);
		app_ds_ad_show_pic(FREE_PIC_DATA,IconeName,DSAD_STB_FULLSCREEN);
		full_screen_ad_is_showing = FALSE;
	}
	}while(0);
	app_porting_ca_os_sem_signal(ds_ad_osd_sem);
}
void app_ds_ad_hide_full_screen_pic(void)
{
	app_porting_ca_os_sem_wait(ds_ad_osd_sem);
	if(full_screen_ad_is_showing == TRUE)
	{
		app_ds_ad_show_pic(HIDE_PIC_DATA,NULL,DSAD_STB_FULLSCREEN);
		app_ds_ad_show_pic(FREE_PIC_DATA,NULL,DSAD_STB_FULLSCREEN);
		full_screen_ad_is_showing = FALSE;
	}

	app_porting_ca_os_sem_signal(ds_ad_osd_sem);
}
uint8_t app_ds_ad_unauthorized_pic_is_ready(void)
{
	ST_DSAD_PROGRAM_SHOW_INFO stProgramShow;
	app_porting_ca_os_sem_wait(ds_ad_osd_sem);
	if(DSAD_NoProgramGetElement(&stProgramShow,DSAD_STB_UNAUTHORIZED) == DSAD_FUN_OK
		&& program_is_unauthorized)
	{
		app_porting_ca_os_sem_signal(ds_ad_osd_sem);
		return 1;
	}
	else
	{
		app_porting_ca_os_sem_signal(ds_ad_osd_sem);
		return 0;
	}
}
void app_ds_ad_set_unauthorized_status(bool status)
{
	app_porting_ca_os_sem_wait(ds_ad_osd_sem);
	program_is_unauthorized = status;
	app_porting_ca_os_sem_signal(ds_ad_osd_sem);
}
void app_ds_ad_unauthorized_pic_display(char * IconeName)
{
    char *focus_window = NULL;

	app_porting_ca_os_sem_wait(ds_ad_osd_sem);
	if((unauthorized_ad_is_showing == FALSE) && (program_is_unauthorized == TRUE))
	{
		/* Hide prog bar */
        focus_window = (char*)GUI_GetFocusWindow();

        if( strcasecmp("win_prog_bar", focus_window) == 0)
        {
            GUI_EndDialog("win_prog_bar");
        }
		/* Hide ad rolling osd */
	    if(osd_showing_type != SHOW_UNKNOWN_TYPE_AD)
        {
        	ds_ad_rolling_osd_show(SHOW_TOP,DVB_CA_ROLLING_FLAG_HIDE," ");
        	ds_ad_rolling_osd_show(SHOW_BOTTOM,DVB_CA_ROLLING_FLAG_HIDE," ");

        	osd_showing_type = SHOW_UNKNOWN_TYPE_AD;
        }

		/*Hide full screen AD picture*/
		if(full_screen_ad_is_showing == TRUE)
		{
			app_ds_ad_show_pic(HIDE_PIC_DATA,NULL,DSAD_STB_FULLSCREEN);
			app_ds_ad_show_pic(FREE_PIC_DATA,NULL,DSAD_STB_FULLSCREEN);
			full_screen_ad_is_showing = FALSE;
		}

		app_ds_ad_show_pic(LOAD_PIC_DATA,IconeName,DSAD_STB_UNAUTHORIZED);
//		app_ds_ad_show_pic(SHOW_PIC_DATA,IconeName,DSAD_STB_UNAUTHORIZED);
		unauthorized_ad_is_showing = TRUE;
	}
	else if((unauthorized_ad_is_showing == TRUE) && (program_is_unauthorized == FALSE))
	{
		app_ds_ad_show_pic(HIDE_PIC_DATA,NULL,DSAD_STB_UNAUTHORIZED);
		app_ds_ad_show_pic(FREE_PIC_DATA,NULL,DSAD_STB_UNAUTHORIZED);
		unauthorized_ad_is_showing = FALSE;
	}
	app_porting_ca_os_sem_signal(ds_ad_osd_sem);
}
void app_ds_ad_hide_unauthorized_pic(void)
{
	app_porting_ca_os_sem_wait(ds_ad_osd_sem);
	if((unauthorized_ad_is_showing == TRUE)/*&& (program_is_unauthorized == FALSE)*/)
	{
		app_ds_ad_show_pic(HIDE_PIC_DATA,NULL,DSAD_STB_UNAUTHORIZED);
		app_ds_ad_show_pic(FREE_PIC_DATA,NULL,DSAD_STB_UNAUTHORIZED);
		unauthorized_ad_is_showing = FALSE;
	}
	app_porting_ca_os_sem_signal(ds_ad_osd_sem);
}
extern uint8_t app_desai_cas50_api_get_lock_status(void);
extern char app_desai_cas50_rolling_message_one_time(void);

void app_ds_ad_osd_display(char * TopIconeName,char * BottomIconeName)
{
	static ST_DSAD_OSD_PROGRAM_SHOW_INFO osd_info;
	EN_DSAD_FUNCTION_TYPE  ret;
	ST_DSAD_PROGRAM_PARA stProgramPara;
	GxBusPmDataProg prog={0};
	uint32_t ppos = 0;
	static uint16_t lastServiceId = 0xffff;
	static uint8_t lastIndex = 0;
	static uint8_t maxIndex = 0;
	static uint8_t name1_cpyed = 0;
	static uint8_t name2_cpyed = 0;

	//printf("TRACK %s %d IN\n",__FUNCTION__,__LINE__);
	if((strlen(TopIconeName) < DS_AD_WIDGET_NAME_LEN) && !name1_cpyed)
	{
		strcpy((char *)osd_top_widget_name,TopIconeName);
		name1_cpyed = 1;
	}
	if((strlen(BottomIconeName) < DS_AD_WIDGET_NAME_LEN)&& !name2_cpyed)
	{
		strcpy((char *)osd_bottom_widget_name,BottomIconeName);
		name2_cpyed = 1;
	}

	app_prog_get_playing_pos_in_group(&ppos);
	GxBus_PmProgGetByPos(ppos,1,&prog);
	stProgramPara.usNetworkId = prog.original_id;
	stProgramPara.usTsId	  = prog.ts_id;
	stProgramPara.usServiceId = prog.service_id;
	do{
		//app_ds_ad_hide_osd();
		memset(&osd_info,0,sizeof(ST_DSAD_OSD_PROGRAM_SHOW_INFO));
//		lastServiceId = prog.service_id;
		ret = DSAD_ProgramGetOSD(stProgramPara,&osd_info);
		maxIndex = osd_info.ucOsdNum;
		/*OSD is not showing OR programe is changed*/
		if((lastServiceId != prog.service_id)||(SHOW_UNKNOWN_TYPE_AD==osd_showing_type))
		{
            if (app_desai_cas50_api_get_lock_status() == FALSE)
            {
    			lastServiceId = prog.service_id;
    			//ADS_Dbg("original_id=%d,ts_id=%d,service_id=%d\n",prog.original_id,prog.ts_id,prog.service_id);
    			if (ret == DSAD_FUN_OK )
    			{
    				lastIndex = 1;
    				do_osd_show(&osd_info,lastIndex);
    				//break;
    			}
    			else
    			{
    				//app_ds_ad_hide_osd();
    				break;
           		}
            }
		}
		/*OSD is showing*/
		if(app_desai_cas50_rolling_message_one_time())
		{
			lastIndex++;
			if(lastIndex > maxIndex)
			{
				lastIndex = 1;
			}
			do_osd_show(&osd_info,lastIndex);
		}
	}while(0);
	//printf("TRACK %s %d OUT\n",__FUNCTION__,__LINE__);
}

int  app_ds_ad_osd_display_status(void)
{
	int iRet = 0;

	app_porting_ca_os_sem_wait(ds_ad_osd_sem);
	if(osd_showing_type == SHOW_UNKNOWN_TYPE_AD)
	{
		iRet = -1;
	}
	else
	{
		iRet = osd_showing_type;
	}

	app_porting_ca_os_sem_signal(ds_ad_osd_sem);
	return iRet;
}

#if 0
void app_ds_ad_hide_osd(void)
{
	static desai_cas50_osd_data_st osdinfo;
	app_porting_ca_os_sem_wait(ds_ad_osd_sem);
	if(osd_showing_type == SHOW_UNKNOWN_TYPE_AD)
	{
		app_porting_ca_os_sem_signal(ds_ad_osd_sem);
		return;
	}
	#if 1
	osdinfo.wStatus = FALSE;
	osdinfo.buffer = NULL;
	osdinfo.from  = 1;
	app_cas_api_on_event(DVB_CA_ROLLING_INFO,CAS_NAME, &osdinfo, sizeof(desai_cas50_osd_data_st));

    #else

	app_desai_cas50_rolling_message_show(SHOW_TOP_AD,DVB_CA_ROLLING_FLAG_RESET," ");
	app_desai_cas50_rolling_message_show(SHOW_TOP_AD,DVB_CA_ROLLING_FLAG_HIDE,NULL);
	app_desai_cas50_rolling_message_show(SHOW_BOTTOM_AD,DVB_CA_ROLLING_FLAG_RESET," ");
	app_desai_cas50_rolling_message_show(SHOW_BOTTOM_AD,DVB_CA_ROLLING_FLAG_HIDE,NULL);
	#endif
	osd_showing_type = SHOW_UNKNOWN_TYPE_AD;
	app_porting_ca_os_sem_signal(ds_ad_osd_sem);
}
#else

static void ds_ad_rolling_osd_show(uint8_t byStyle,uint8_t showFlag,char* message)
{
	printf("%s>>style=%d,showFlag=%d,memssage=%s\n",__FUNCTION__,byStyle,showFlag,message);
	switch(byStyle)
	{
		case SHOW_TOP:
			if (DVB_CA_ROLLING_FLAG_HIDE == showFlag)
			{
				GUI_SetProperty("win_full_screen_text_roll_top","rolling_stop",NULL);
				GUI_SetProperty("win_full_screen_text_roll_top","string", message);
				GUI_SetProperty("win_full_screen_text_roll_top","state","hide");
				GUI_SetProperty("win_full_screen_text_roll_top_back","state","hide");
			}
			else if (DVB_CA_ROLLING_FLAG_SHOW == showFlag)
			{
				GUI_SetProperty("win_full_screen_text_roll_top_back","state","show");
				GUI_SetProperty("win_full_screen_text_roll_top","state","show");
				GUI_SetInterface("flush", NULL);
			}
 			else if (DVB_CA_ROLLING_FLAG_RESET == showFlag)
			{
				GUI_SetProperty("win_full_screen_text_roll_top","string","");
				GUI_SetInterface("flush", NULL);
				GUI_SetProperty("win_full_screen_text_roll_top","rolling_stop",NULL);
				GUI_SetProperty("win_full_screen_text_roll_top","string", message);
				GUI_SetProperty("win_full_screen_text_roll_top_back","state","show");
				GUI_SetProperty("win_full_screen_text_roll_top","reset_rolling", "");
				GUI_SetProperty("win_full_screen_text_roll_top","state","hide");
			}
			else if (DVB_CA_ROLLING_FLAG_TRANS_HIDE == showFlag)
			{
				GUI_SetProperty("win_full_screen_text_roll_top","rolling_stop",NULL);
				GUI_SetProperty("win_full_screen_text_roll_top","string", message);
				GUI_SetProperty("win_full_screen_text_roll_top","state","osd_trans_hide");
				GUI_SetProperty("win_full_screen_text_roll_top_back","state","osd_trans_hide");
			}
			break;
		case SHOW_BOTTOM:
			if (DVB_CA_ROLLING_FLAG_HIDE == showFlag)
			{
				GUI_SetProperty("win_full_screen_text_roll_bottom","rolling_stop",NULL);
				GUI_SetProperty("win_full_screen_text_roll_bottom","string", message);
				GUI_SetProperty("win_full_screen_text_roll_bottom","state","hide");
				GUI_SetProperty("win_full_screen_text_roll_bottom_back","state","hide");
			}
			else if (DVB_CA_ROLLING_FLAG_SHOW == showFlag)
			{
				GUI_SetProperty("win_full_screen_text_roll_bottom_back","state","show");
				GUI_SetProperty("win_full_screen_text_roll_bottom","state","show");
				GUI_SetInterface("flush", NULL);
			}
			else if (DVB_CA_ROLLING_FLAG_RESET == showFlag)
			{
				GUI_SetProperty("win_full_screen_text_roll_bottom","string","");
				GUI_SetInterface("flush", NULL);
				GUI_SetProperty("win_full_screen_text_roll_bottom","rolling_stop",NULL);
				GUI_SetProperty("win_full_screen_text_roll_bottom","string", message);
				GUI_SetProperty("win_full_screen_text_roll_bottom_back","state","show");

				GUI_SetProperty("win_full_screen_text_roll_bottom","reset_rolling", "");
				GUI_SetProperty("win_full_screen_text_roll_bottom","state","hide");
			}
			else if (DVB_CA_ROLLING_FLAG_TRANS_HIDE == showFlag)
			{
				GUI_SetProperty("win_full_screen_text_roll_bottom","rolling_stop",NULL);
				GUI_SetProperty("win_full_screen_text_roll_bottom","string", message);
				GUI_SetProperty("win_full_screen_text_roll_bottom","state","osd_trans_hide");
				GUI_SetProperty("win_full_screen_text_roll_bottom_back","state","osd_trans_hide");
			}
			break;
		default:
			break;
	}
	printf("%s %d>>exit\n",__FUNCTION__,__LINE__);
}

void app_ds_ad_hide_rolling_osd(void)
{
	if(osd_showing_type == SHOW_UNKNOWN_TYPE_AD)
	{
		return;
	}
	app_porting_ca_os_sem_wait(ds_ad_osd_sem);
	ds_ad_rolling_osd_show(SHOW_TOP,DVB_CA_ROLLING_FLAG_HIDE," ");
	ds_ad_rolling_osd_show(SHOW_BOTTOM,DVB_CA_ROLLING_FLAG_HIDE," ");
	app_porting_ca_os_sem_signal(ds_ad_osd_sem);

	osd_showing_type = SHOW_UNKNOWN_TYPE_AD;
}
#endif
void app_ds_ad_setpid(uint16_t pid)
{
	ad_pid = pid;
	//DSAD_SetDataPid(pid);
}
#if 0
void mem_test(void)
{
	void * ptr[10240];
	//static void * ptrUnFree[100] = {NULL,};
	int i = 0;
	char isLargeEnugh = 0;
	memset(ptr,NULL,10240*sizeof(void *));
	//gxos_interrupt_disable();
	//gxos_scheduler_lock();
	for(i = 0;i < 10240;i++)
	{
		ptr[i] = GxCore_Malloc(4096);
		if(ptr[i] == NULL)
		{
			break;
		}
	}
	if(i >= 10240)
	{
		printf("Free memory size > 40MB\n");
		isLargeEnugh = 1;
	}
	else
	{
		printf("Free memory size is %d KB\n",i*4);
	}
	for(i = 0;i < 10240;i++)
	{
		if(ptr[i] != NULL)
		{
			GxCore_Free(ptr[i]);
		}
	}

	/*Occupied the free memory*/
#if 0
	if(isLargeEnugh)
	{
		for(i = 0;i < 100;i++)
		{
			if(ptrUnFree[i] != NULL)
			{
				continue;
			}
			ptrUnFree[i] = GxCore_Malloc(2*1024*1024);
			if(ptrUnFree[i] == NULL)
			{
				break;
			}
			else
			{
			}
		}
		printf("%dMB memory occupied!\n",i*2);
	}
#endif
//gxos_interrupt_enable();
//gxos_scheduler_unlock();
}
#endif
