unset CHIP
unset DEMOD
unset ARCH
unset OS
unset CROSS_PATH
unset GXLIB_PATH
unset GXSRC_PATH
unset GX_KERNEL_PATH

if [ "$1" = "csky-ecos" ] ; then
	echo ARCH=csky OS=ecos Configuration  !
	export ARCH=csky
	export OS=ecos
	export CROSS_PATH=csky-ecos
fi

if [ "$1" = "csky-linux" ] ; then
	echo ARCH=csky OS=linux Configuration  !
	export ARCH=csky
	export OS=linux
	export CROSS_PATH=csky-linux
fi

if [ -z "$CROSS_PATH" ] ; then
	echo -e "\033[31m Config Errror!!! ,please check  ARCH_OS \033[0m"
	echo "               "              
	echo eg: source env.sh csky-ecos
	echo "    "source env.sh csky-linux
fi

# goxceed��汾·��
export GXLIB_PATH=/opt/goxceed/$CROSS_PATH
export GXSRC_PATH=`pwd`
# linux�������ں˴���·��
export GX_KERNEL_PATH=$GXSRC_PATH/../linux-2.6.27.55

unset DVB_CUSTOM
unset DVB_MARKET
unset DVB_CA_MARKET
unset DVB_AD_MARKET
unset DVB_CA_FLAG
unset DVB_USB_FLAG
unset DVB_PVR_SPEED_SUPPORT
unset DVB_PANEL_TYPE
unset PANEL_CLK_GPIO
unset PANEL_DATA_GPIO
unset PANEL_STANDBY_GPIO
unset PANEL_LOCK_GPIO
unset DVB_KEY_TYPE
unset DVB_TUNER_TYPE
unset DVB_DEMOD_TYPE
unset DVB_DEMOD_MODE
unset DVB_TS_SRC
unset DVB_AUTO_TEST_FLAG
unset DVB_THEME
unset DVB_MEDIA_FLAG
unset DVB_ZOOM_RESTART_PLAY
unset DVB_BAD_SIGNAL_SHOW_LOGO
unset CUSTOMER

unset MOVE_FUNCTION_FLAG
unset DVB_CA_FREE_STOP

# CAö�٣���app_common_porting_stb_api.h��dvb_ca_type_t�ṹ���ж�Ӧ�������òο���ӦCA����Ŀ¼�µ�readme.txt
unset DVB_CA_TYPE
# ��Ӧ�г������ң�CA���ӿ����ƣ���libY1120-tonghui-gx3001-20121212D.a����ΪY1120-tonghui-gx3001-20121212D��
unset DVB_CA_1_LIB
unset DVB_CA_2_LIB
# ��ӦCA�����к궨��
unset DVB_CA_1_FLAG
unset DVB_CA_2_FLAG
# ��ӦCA���ƣ�Ŀ¼��,�磨cdcas3.0��
unset DVB_CA_1_NAME
unset DVB_CA_2_NAME

# ������ƣ�Ŀ¼�������òο���Ӧ������Ŀ¼�µ�readme.txt
unset DVB_AD_NAME
# �������
unset DVB_AD_TYPE
# ��Ӧ�г������ң�������ӿ�����
unset DVB_AD_LIB

# OTAЭ�飨���ͣ�
unset DVB_OTA_TYPE

unset DVB_JPG_LOGO
unset AUDIO_DOLBY
unset DVB_SUBTITLE_FLAG
unset DVB_PVR_FLAG
unset DVB_NETWORK_FLAG
unset LINUX_OTT_SUPPORT

unset DVB_HD_LIST
unset DVB_LOGO_JPG
unset DVB_WORK_PATH
unset DVB_LOGO_PATH
unset DVB_I_FRAME_PATH
unset DVB_RESOLUTION
unset DVB_SYS_MODE
unset DVB_VIDEO_X
unset DVB_VIDEO_Y
unset DVB_VIDEO_W
unset DVB_VIDEO_H
unset DVB_SAT_MAX
unset DVB_TP_MAX
unset DVB_SERVICE_MAX
unset DVB_DDRAM_SIZE
unset DVB_FLASH_SIZE
unset DVB_PAT_TIMEOUT
unset DVB_SDT_TIMEOUT
unset DVB_NIT_TIMEOUT
unset DVB_PMT_TIMEOUT
unset DVB_LCN_DEFAULT
unset DVB_CENTER_FRE
unset DVB_CENTER_SYMRATE
unset DVB_CENTER_QAM
unset DVB_CENTER_BANDWIDTH
unset DVB_DUAL_MODE
unset DVB_SERIAL_SUPPORT
unset CUSTOMER
unset DVB_32MB
# ��оƬ����
export CHIP=gx3113c
# demodоƬ����
export DEMOD=atbm886x

# �ͻ�Ӳ������£���
# ͬһ�ͻ���ͬоƬ���������ܴ��ڶ��Ӳ����ƣ�����runde,rundeddr1,runde32m��
export DVB_CUSTOM=gongban
# �г�
export DVB_MARKET=gongban

# �ͻ�Ӳ���г�:${DVB_CUSTOM}_$(DVB_MARKET)��

# ��Ӧ�ͻ�Ӳ����loader-sflash.bin��.boot ·����
# gxappcommon/flashconf/loader-sflash/$(CHIP)/$(DEMOD)/$(DVB_CUSTOM)/loader-sflash.bin

# ��Ӧ�ͻ��г���invariable_oem.ini��variable_oem.ini ·��(����ȷ��Ψһ��):
# gxappcommon/flashconf/ini/$(CHIP)/$(DEMOD)/$(DVB_CUSTOM)_$(DVB_MARKET)/invariable_oem.ini ��variable_oem.ini

# ��Ӧ�ͻ��г���flash.conf��flash_ts.conf ·����
# ����г��汾��������flash��������·��(�����г�����
# gxappcommon/flashconf/conf/$(CHIP)/$(DEMOD)/$(DVB_CUSTOM)_$(DVB_MARKET)/flash.conf��flash_ts.conf
# �г��汾flashͨ������·����
# gxappcommon/flashconf/conf/$(CHIP)/$(DEMOD)/$(DVB_CUSTOM)/flash.conf��flash_ts.conf

# ��Ӧ�ͻ��г���env.sh ·��(����ȷ��Ψһ��):
# gxappcommon/sh/$(CHIP)/$(DEMOD)/$(DVB_CUSTOM)_$(DVB_MARKET)/env.sh

# CA�г�
# ͬһCA�����ܴ��ڲ�ͬ�г��汾
# ���ͬһ�г����ܴ��ڶ�����ҡ���ͬ������Ӳ��ƽ̨��CA��Ȳ��죬����ݲ��컯����
# ��ӦCAĿ¼gxapp_common/cas/$DVB_CA_1_NAME/$DVB_CA_MARKET/; gxapp_common/cas/$DVB_CA_2_NAME/$DVB_CA_MARKET/;
export DVB_CA_MARKET=gongban
export DVB_AD_MARKET=gongban

#�Ƿ����CA����ֵΪno��Ϊ�����汾
export DVB_CA_FLAG=yes

if [ $DVB_CA_FLAG = "yes" ]; then
	#CAö�٣���app_common_porting_stb_api.h��dvb_ca_type_t�ṹ���ж�Ӧ��
#	export DVB_CA_TYPE=DVB_CA_TYPE_MG
#	export DVB_CA_1_NAME=mg_cas
#	export DVB_CA_1_LIB=mgcaslib_v423-GX3201
#	export DVB_CA_1_FLAG=DVB_CA_TYPE_MG_FLAG

#ȫ��
#	export DVB_CA_TYPE=DVB_CA_TYPE_QZ
#	export DVB_CA_1_NAME=qz_cas
#	export DVB_CA_1_LIB=
#	export DVB_CA_1_FLAG=DVB_CA_TYPE_QZ_FLAG
# ��Զ
#export DVB_CA_TYPE=DVB_CA_TYPE_BY
#export DVB_CA_1_NAME=by_cas
#export DVB_CA_1_LIB=byca
#export DVB_CA_1_FLAG=DVB_CA_TYPE_BY_FLAG
#����
#export DVB_CA_TYPE=DVB_CA_TYPE_CDCAS30
#export DVB_CA_1_NAME=cd_cas30
#export DVB_CA_1_LIB=Y1120-tonghui-gx3001-20121212D
#export DVB_CA_1_FLAG=DVB_CA_TYPE_CD_CAS30_FLAG
#�ֶ�
#export DVB_CA_TYPE=DVB_CA_TYPE_DVB
#export DVB_CA_1_NAME=dvb_cas
#export DVB_CA_1_LIB=
#export DVB_CA_1_FLAG=DVB_CA_TYPE_DVB_FLAG
#DVB
export DVB_CA_TYPE=DVB_CA_TYPE_DVB
export DVB_CA_1_NAME=dvb_cas
export DVB_CA_1_LIB=
export DVB_CA_1_FLAG=DVB_CA_TYPE_DVB_FLAG

fi

#����˫CA�����������ֻ��һ��CA���ڶ���CA��������
#if [ $DVB_CA_FLAG = "yes" ]; then
#	export DVB_CA_2_NAME=byca
#	export DVB_CA_2_LIB=byca
#	export DVB_CA_2_FLAG=DVB_CA_TYPE_BY_FLAG
#fi

# set ad support type
#export DVB_AD_NAME=byads
#export DVB_AD_FLAG=DVB_AD_TYPE_BY_FLAG
#export DVB_AD_NAME=dsads
#export DVB_AD_FLAG=DVB_AD_TYPE_DS_FLAG
#export DVB_AD_LIB=dsad
export DVB_AD_NAME=maikeads
export DVB_AD_FLAG=DVB_AD_TYPE_MAIKE_FLAG

# ѡ��OTAЭ��
export DVB_OTA_TYPE=DVB_NATIONALCHIP_OTA

# �����Ƿ�֧�ָ����б�����ʾ
export DVB_HD_LIST=no

# ����loader���Ƿ��ѿ���jpg logo��ʾ֧��(����gxloader�е�.config����һ��)
export DVB_JPG_LOGO=yes

# ����оƬ�Ƿ�֧��ac3����
export AUDIO_DOLBY=no

# �����Ƿ���PVR���ܣ�¼�ơ�ʱ�Ƶȣ�
export DVB_PVR_FLAG=yes

# �����Ƿ���subtitle��Ļ��ʾ����
export DVB_SUBTITLE_FLAG=no

# �����Ƿ������繦�ܣ�youtube�ȣ� (��linux����֧�ִ˹���)
export DVB_NETWORK_FLAG=no

# �����Ƿ�֧��OTT����
export LINUX_OTT_SUPPORT=no

#�Ƿ�֧��USB����
export DVB_USB_FLAG=yes

# �Ƿ�֧��PVR���������
export DVB_PVR_SPEED_SUPPORT=0

# ѡ��ǰ������� panel_xxx(�г�)_xxx(����)_xxx(�������)����PANEL_TYPE_fd650_RUNDE
# ��֧�����ο���include/bsp/panel_enum.h
export DVB_PANEL_TYPE=PANEL_TYPE_fd650_THINEWTEC
export PANEL_CLK_GPIO=36
export PANEL_DATA_GPIO=35
export PANEL_STANDBY_GPIO=34
export PANEL_LOCK_GPIO=33

# ѡ��ң�����궨������ keymap_xxx(�г�)_(����)����KEY_GONGBAN_NATIONALCHIP_NEW
# ��֧�����ο�:key_xmlĿ¼�µ�.xml group="KEY_GONGBAN_NATIONALCHIP_NEW"�ȣ�ע��ͬһ��kex(x).xml���ܳ���ң�������������������ң�������벻��ͬkey(x).xml�ж��壩
export DVB_KEY_TYPE=KEY_THINEWTEC

# ���оƬ���� ���ο�demod_enum.h����
export DVB_DEMOD_TYPE=DVB_DEMOD_ATBM886X

# ���ģʽ���ο�demod_enum.h���壨Ŀǰ��֧��DVB_DEMOD_DVBS��
export DVB_DEMOD_MODE=DVB_DEMOD_DTMB

# ����ѡ�õڼ�·ts��� : ��ѡ0,1,2��
export DVB_TS_SRC=2

# ����tuner���ͣ��ο�demod/include/tuner_neum.h����
export DVB_TUNER_TYPE=TUNER_MXL608

#export CUSTOMER=CUST_TAOYUAN
export CUSTOMER=CUST_TAIKANG
#export CUSTOMER=CUST_LINLI
#export CUSTOMER=CUST_JINGANGSHAN
#export CUSTOMER=CUST_SHANGSHUI
#export DVB_LOGO_JPG=logo_thinewtec.jpg
echo DVB_CUSTOMER_NAME=$CUSTOMER
if [ $CUSTOMER = "CUST_LINLI" ]; then
export DVB_LOGO_JPG=logo_linli.jpg		#�������
#export DVB_LOGO_JPG=logo_thinewtec.jpg
export DVB_CENTER_FRE=794

export DVB_KEY_TYPE=KEY_THINEWTEC

elif [ $CUSTOMER = "CUST_TAOYUAN" ]; then
export DVB_CENTER_FRE=794               #������Դ

export DVB_LOGO_JPG=logo_taoyuan.jpg
export DVB_KEY_TYPE=KEY_TAOYUAN

elif [ $CUSTOMER = "CUST_TAIKANG" ]; then
export DVB_LOGO_JPG=logo_taikang.jpg			#����̫��
export DVB_CENTER_FRE=730

export DVB_KEY_TYPE=KEY_THINEWTEC
#export DVB_KEY_TYPE=KEY_TAOYUAN
elif [ $CUSTOMER = "CUST_JINGANGSHAN" ]; then
export DVB_LOGO_JPG=logo_maike.jpg			#����̫��
export DVB_CENTER_FRE=730
export DVB_KEY_TYPE=KEY_THINEWTEC
elif [ $CUSTOMER = "CUST_SHANGSHUI" ]; then
export DVB_LOGO_JPG=logo_shangshui.jpg			#��ˮ
export DVB_CENTER_FRE=730
export DVB_KEY_TYPE=KEY_THINEWTEC

fi

# ����theme��Դ�����ߡ����桢���塢����ȣ����ο�development/theme�¶�ӦĿ¼
export DVB_THEME=SD

if [ "$1" = "csky-linux" ] ; then
# XML��ͼƬ��·��
export DVB_WORK_PATH='"/dvb/"'
# �㲥����ͼƬ·��
export DVB_LOGO_PATH='"/dvb/theme/logo.bin"'
export DVB_I_FRAME_PATH='"/dvb/theme/logo.bin"'
fi

if [ "$1" = "csky-ecos" ] ; then
# �㲥����ͼƬ·��
export DVB_LOGO_PATH='"/theme/logo.bin"'
export DVB_I_FRAME_PATH='"/theme/logo_audio.bin"'
export DVB_NETWORK_FLAG=no
fi


# �궨�������
# ������塢����
export DVB_RESOLUTION=SD
# ����Ƶͬ����ʽ 0-PCR_RECOVER 1-VPTS_RECOVER 2-APTS_RECOVER 3-AVPTS_RECOVER 4-NO_RECOVER
export DVB_SYS_MODE=0
# ȫ����ƵXλ��
export DVB_VIDEO_X=0
# ȫ����ƵYλ��
export DVB_VIDEO_Y=0
# ȫ����Ƶ���С
export DVB_VIDEO_W=720
# ȫ����Ƶ�ߴ�С
export DVB_VIDEO_H=576
# ���������������ߡ����淽������Ϊ1��
export DVB_SAT_MAX=1
# TPƵ��������
export DVB_TP_MAX=200
# ��Ŀ������
export DVB_SERVICE_MAX=1000
# DDRAM ��С
export DVB_DDRAM_SIZE=128
# FLASH ��С
export DVB_FLASH_SIZE=8
# ����PAT��ʱʱ�䣨ms��
export DVB_PAT_TIMEOUT=3000
# ����SDT��ʱʱ�䣨ms��
export DVB_SDT_TIMEOUT=5000
# ����NIT��ʱʱ�䣨ms��
export DVB_NIT_TIMEOUT=10000
# ����PMT��ʱʱ�䣨ms��
export DVB_PMT_TIMEOUT=8000
# LCN����ģʽ�£��Ǳ��������߼�Ƶ���ŵĽ�Ŀ��Ĭ����ʼ�߼�Ƶ����
export DVB_LCN_DEFAULT=500
# ��Ƶ�����
export DVB_CENTER_FRE=730
export DVB_CENTER_SYMRATE=6875
export DVB_CENTER_QAM=2
# ����
export DVB_CENTER_BANDWIDTH=8
# �Ƿ�֧��dvbc-dtmb˫ģ
export DVB_DUAL_MODE=no
# Ӳ���Զ����Կ��ء�yes- �����Զ����Թ��� no -�ر��Զ����Թ���
export DVB_AUTO_TEST_FLAG=no
# �Ƿ�֧�ֶ�ý�幦��
export DVB_MEDIA_FLAG=yes
# ��Ƶ�����Ƿ����²���
export DVB_ZOOM_RESTART_PLAY=no
#����
export DVB_SERIAL_SUPPORT=no
#�ƶ�
export MOVE_FUNCTION_FLAG=no

export DVB_CA_FREE_STOP=no
export LOGO_SHOW_DELAY=yes
export FACTORY_SERIALIZATION_SUPPORT=no
# echo export path
if [ -z "$CROSS_PATH" ] ; then
	echo
else
	echo CHIP=$CHIP
	echo DEMOD=$DEMOD
	echo PATH:
	echo RC_VERSION=$RC_VERSION
	echo GX_KERNEL_PATH=$GX_KERNEL_PATH
	echo GXLIB_PATH=$GXLIB_PATH
	echo DVB_CUSTOM=$DVB_CUSTOM
	echo DVB_MARKET=$DVB_MARKET
	echo DVB_CA_FLAG=$DVB_CA_FLAG
	if [ $DVB_CA_FLAG = "yes" ]; then
		echo DVB_CA_MARKET=$DVB_CA_MARKET
		echo DVB_CA_TYPE=$DVB_CA_TYPE
		echo DVB_CA_1_NAME=$DVB_CA_1_NAME
#		echo DVB_CA_1_LIB=$DVB_CA_1_LIB
#		echo DVB_CA_1_FLAG=$DVB_CA_1_FLAG
		echo DVB_CA_2_NAME=$DVB_CA_2_NAME
#		echo DVB_CA_2_LIB=$DVB_CA_2_LIB
#		echo DVB_CA_2_FLAG=$DVB_CA_2_FLAG
	fi
	if [ -z "$DVB_AD_NAME" ] ; then
		echo
	else
		echo DVB_AD_MARKET=$DVB_AD_MARKET
		echo DVB_AD_NAME=$DVB_AD_NAME
#		echo DVB_AD_FLAG=$DVB_AD_FLAG
#		echo DVB_AD_LIB=$DVB_AD_LIB
	fi
	echo DVB_OTA_TYPE=$DVB_OTA_TYPE
	echo DVB_JPG_LOGO=$DVB_JPG_LOGO
	echo DVB_HD_LIST=$DVB_HD_LIST
	echo DVB_USB_FLAG=$DVB_USB_FLAG
	echo DVB_PVR_FLAG=$DVB_PVR_FLAG
	echo DVB_SUBTITLE_FLAG=$DVB_SUBTITLE_FLAG
	echo AUDIO_DOLBY=$AUDIO_DOLBY
	echo DVB_NETWORK_FLAG=$DVB_NETWORK_FLAG
	echo LINUX_OTT_SUPPORT=$LINUX_OTT_SUPPORT
	echo DVB_PVR_SPEED_SUPPORT=$DVB_PVR_SPEED_SUPPORT
	echo DVB_PANEL_TYPE=$DVB_PANEL_TYPE
	echo DVB_KEY_TYPE=$DVB_KEY_TYPE
	echo DVB_LOGO_JPG=$DVB_LOGO_JPG
	echo DVB_TUNER_TYPE=$DVB_TUNER_TYPE
	echo DVB_DEMOD_TYPE=$DVB_DEMOD_TYPE
	echo DVB_DEMOD_MODE=$DVB_DEMOD_MODE
	echo DVB_TS_SRC=$DVB_TS_SRC
	echo DVB_RESOLUTION=$DVB_RESOLUTION
	echo DVB_SYS_MODE=$DVB_SYS_MODE
	echo DVB_CENTER_FRE=$DVB_CENTER_FRE
	echo DVB_SERIAL_SUPPORT=$DVB_SERIAL_SUPPORT
	echo MOVE_FUNCTION_FLAG = $MOVE_FUNCTION_FLAG
	#	echo DVB_CENTER_SYMRATE=$DVB_CENTER_SYMRATE
	#	echo DVB_CENTER_QAM=$DVB_CENTER_QAM
	#   	echo DVB_CENTER_BANDWIDTH=$DVB_CENTER_BANDWIDTH
	echo DVB_DUAL_MODE=$DVB_DUAL_MODE
    	echo DVB_AUTO_TEST_FLAG=$DVB_AUTO_TEST_FLAG
	echo DVB_THEME=$DVB_THEME
	echo DVB_MEDIA_FLAG=$DVB_MEDIA_FLAG
	echo DVB_ZOOM_RESTART_PLAY=$DVB_ZOOM_RESTART_PLAY
	echo DVB_CA_FREE_STOP=$DVB_CA_FREE_STOP
fi
