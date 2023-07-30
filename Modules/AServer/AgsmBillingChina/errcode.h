#ifndef _ERRCODE_H_
#define _ERRCODE_H_

enum
{
	BSIP_OK			  = 0,
    SOCKDISCONNECT    = -1001,  //socket���ӶϿ�
    SOCKIP_OR_PORTERR = -1002,  //��������ip port���ô���
    SOCKCONNECTFAIL   = -1003,  //sock����ʧ��
    NOINITCLIENT      = -1011,  //û�г�ʼ���Ŀͻ���

    QUEUECACHEFULL    = -2001,  //���л�������
    QUEUELOCKERROR    = -2002,  //������������
    QUEUEOPERATEERROR = -2003,  //����Push Pop���������(��ָ��)
    QUEUEEMPTY        = -2004,  //���пգ�Pop������
    QUEUEDATAERR      = -2005,  //�������ݵ����ݴ��糤�ȴ���С�ڿɻ���ķ�Χ

    USERDATALENERROR  = -3001,  //�û�����Push���Ⱥ�ҵ��ṹ�ĳ��Ȳ�һֱ
    COMMUDATAERROR    = -3002,  //���յ�ͨѶ���ݰ����ܴ�

    POINTERNULL       = -4001,  //�����˿�ָ��(��Ҫָ��ĵط��ǲ��ܽ��տ�ָ���)

    ENCRYPTERROR      = -5001,  //���ܴ�
    DECRYPTERROR      = -5002,  //���ܴ�

    PARAMPATHERROR    = -6001,  //������߻����ļ�·������
	CONFLOADFAILED	  = -6002,  //�����ļ�����ʧ��
	CONFGETFAILED	  = -6003,	//�����÷�������ȡ����ʧ��
	RELOADFAILED	  = -6004,  //����Reʧ��
	
	BSIP_BE_START_FAILD			=-7001,//��������ʧ��
	BSIP_BE_LOGIN_FAILD			=-7002,//�û�����ʧ��
	BSIP_BE_LOGOUT_FAILD		=-7003,//�û�ע��ʧ��
	BSIP_BE_CHANGEMAP_FAILD		=-7004,//�û�����ͼʧ��
	BSIP_BE_USER_ALREADY_EXIST	=-7005,//�û��Ѿ�����������
	BSIP_BE_USER_NOT_EXIST		=-7006,//�û�������
	BSIP_BE_MEMORY_ERROR		=-7007,//��������ڴ����

    PUSHBUFF          = 9001   //���뻺����ȷ���˷���ֵ��ʾ���ͳɹ����Ǵ������
};

#define BSIP_BE_OK		0

#endif
