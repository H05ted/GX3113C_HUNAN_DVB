/*
* ���ļ��ӿ�ʵ��CA��ֲFLASH��ʼ��������д�����ӿ�
*/
#include <gxtype.h>
#include "app_common_flash.h"
#include "app_common_porting_ads_flash.h"
 #include "app_common_porting_stb_api.h"
#include "gx_nvram.h"

#define ADS_MAX_NUM (50)
handle_t ads_nvram_handle[ADS_MAX_NUM]={0};
#define ADS_MAX_NVRAM_FILE_NAME_LEN (128)
//handle_t p_gsemFlash;

/*
* ��ʼ��ca flash, �����С
* type --CA���ͣ����ݶ�CAϵͳ
*/
void app_porting_ads_flash_init(uint32_t size)
{
	int32_t dvb_ads_flag =0;
    char        file_name[ADS_MAX_NVRAM_FILE_NAME_LEN]={0};
/*	dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	sprintf(file_name,"gxca_nvram_%d.dat",dvb_ca_flag);
	if (dvb_ca_flag >= CA_MAX_NUM)
		{
			return;
		}*/
	sprintf(file_name,"gxads_nvram_%d.dat",dvb_ads_flag);

	ADS_Dbg(" app_porting_ads_flash_init  size=0x%x\n",size);
	ads_nvram_handle[dvb_ads_flag]= GxNvram_Open(file_name,size);
	return ;
}

/*
* ��FLASH�ж�ȡCA ����
* type --CA���ͣ����ݶ�CAϵͳ
*/
uint8_t app_porting_ads_flash_read_data(uint32_t Offset,uint8_t *pbyData,uint32_t *pdwLen)
{
	size_t Size = 0;
	int32_t dvb_ads_flag =0;

	if ((NULL == pbyData)||(NULL == pdwLen))
		return 0;

	size_t readSize = (uint32_t)*pdwLen;
/*	dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();

	if (dvb_ca_flag >= CA_MAX_NUM)
		{
			return 0;
		}*/

	
	if (E_INVALID_HANDLE == ads_nvram_handle[dvb_ads_flag])
		return 0;


	CAS_Dbg(" app_porting_ads_flash_read_data   Offset=0x%x  pdwLen=0x%x\n",Offset,*pdwLen);
	Size = GxNvram_Read(ads_nvram_handle[dvb_ads_flag],Offset, pbyData, readSize);

	
	if( Size != readSize)
	{
		CAS_Dbg(" app_porting_ads_flash_read_data Size=0x%x readSize=0x%x failed \n",
			Size,readSize);

		return 0;
	}

	return 1;

}

/*
* CA����д��FLASH
*/
uint8_t app_porting_ads_flash_write_data(uint32_t Offset,uint8_t *pbyData,uint32_t dwLen)
{

	size_t Size = 0;
	int32_t dvb_ads_flag =0;
	size_t writeSize = dwLen;
/*	dvb_ca_flag=  app_flash_get_config_dvb_ca_flag();

	if (dvb_ca_flag >= CA_MAX_NUM)
		{
			return 0;
		}*/
	
		
	if (E_INVALID_HANDLE == ads_nvram_handle[dvb_ads_flag])
		return 0;


	if (NULL == pbyData)
	{
		CAS_Dbg(" app_porting_ads_flash_write_data   failed \n");
		return 0;
	}

	CAS_Dbg(" app_porting_ads_flash_write_data   Offset=0x%x  pdwLen=0x%x\n",Offset,dwLen);
		
	Size = GxNvram_Write(ads_nvram_handle[dvb_ads_flag],Offset, pbyData, writeSize);

	CAS_Dbg("app_porting_ads_flash_write_data size=0x%x\n",Size);


	if(Size != writeSize)
	{
		CAS_Dbg("app_porting_ads_flash_write_data size not equal writeSize=0x%x size=0x%x\n",(unsigned int)writeSize,Size);
		return 0;
	}

	return 1;	
}


