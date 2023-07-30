/*
* Copyright (c) 2005,�Ϻ�ʢ������Ʒ�ƽ̨��
* All rights reserved.
*
* �ļ����ƣ�bsipmacro.h
* ժ    Ҫ���궨�壨���ӿ���Ϣ�嶨����ʹ�ã�
* ��ǰ�汾��1.4
* ��    �ߣ�����
* ������ڣ�2005��9��21��
*
* �޸����ݣ�1.4�汾�������ļ�
* �޸�ʱ�䣺2005��9��21��
*
* �޸����ݣ����MAX_NOTIFY_LEN��
* �޸�ʱ�䣺2005��10��13��
*/

/*
*bsip msg body ,
*/
#ifndef _MSG_MACRO_H_
#define _MSG_MACRO_H_

/*-------------------------��Ϣ���ֶγ��Ⱥ궨��-------------------------*/
#define MAX_TIME_LEN                19  //ʱ�䳤�� ��ʽΪ��YYYY-MM-DD HH24:MI:SS
#define MAX_USERID_LEN              32  //�û����������磺��ɽ����unicode
#define MAX_SDID_LEN                16  //�û������ʺų���
#define MAX_PTID_LEN                32  //poptang id����
#define MAX_USERPWD_LEN             16  //�û����볤
#define MAX_MD5PWD_LEN              16  //�û����볤
#define IP_ADDRESS_LEN              15  //ip��ַ���� �磺61.172.252.37
#define MAX_DIGEST_LEN              16  //���ܽ���ֶγ�
#define MAX_HOSTNAME_LEN            64  //��������
#define MAX_ORDERID_LEN             32  //�����ų� Ŀǰ����unique_id
#define MAX_AWARD_LEN               30  //�콱�ų�
#define MAX_ENC_PWD_LEN             16  //��Ϸ��������������볤
#define MAX_ENC_BUFFER_LEN          1024//���ܺ����ݰ�����
#define MAX_TELE_LEN                15  //���к���
#define MAX_USERNAME_LEN            32  //�û����Ƴ���,ֻ֧��16λ������
#define MAX_CARDNUM_LEN             20  //��ֵ���ų���
#define MAX_SESSID_LEN              32  //�ỰID����
#define MAX_AWARDSN_LEN             20  //���͵������к�
#define MAX_AWARDPATCH_LEN          10  //�콱���κ�
#define MAX_RESERVE_LEN             16  //���������ַ�������
#define MAX_PATCHID_LEN             10  //��ֵ��Ӧ����κ�
#define MAX_CONTENT_LEN             30  //��Ϣ���ܵ��ַ�������

/*-------------------------��Ϣ���ֶ�ȡֵ�궨��-------------------------*/
#define SD_ID_INVALIDATION          0 //�����ʺ���Ч
#define PT_ID_INVALIDATION          0 //�����ʺ���Ч

#define USER_TYPE_REG               1 // '1'-��Ϸע���û�
#define USER_TYPE_OTHER_REG         2 // '2'-������Ϸƽ̨�û�,�������ʹ���

#define PAY_TYPE_DAY                1 // ����
#define PAY_TYPE_SEC                2 // ����
#define PAY_TYPE_POINT              3 // ���㣨PT�ң�
#define PAY_TYPE_PROP               4 // ��ͨ���ߣ�Ԫ����
#define PAY_TYPE_MONEY              5 // ��Ǯ
#define PAY_TYPE_SCORE              6 // ������

#define APP_TYPE_GAME               1   //��Ϸ�Ʒ�
#define APP_TYPE_PROP               2   //�����
#define APP_TYPE_VOD                3   //VOD�㲥
#define APP_TYPE_VOIP               4   //VOIPʱ���Ʒ�
#define APP_TYPE_IVR                5   //IVRʱ���Ʒ�
#define APP_TYPE_SONG               6   //�����㲥
#define APP_TYPE_TIME               7   //��ʱ��
#define APP_TYPE_CONSIGN            8   //���۽���

#define USER_STATUS_NONE            0 //��״̬
#define USER_STATUS_LOGON           1 //��������
#define USER_STATUS_ONLINE          2 //����
#define USER_STATUS_LOGOFF          3 //����
#define USER_STATUS_JUMP            4 //����ͼ����������
#define USER_STATUS_ENDPOINT        5 //����ٽ��

#define UNLOCK_CONFIRM_NO           2 //ȡ��Ԥ��
#define UNLOCK_CONFIRM_YES          1 //ȷ��Ԥ��

#define COUPLE_TYPE_STRICT          1 //�����
#define COUPLE_TYPE_FREE            2 //�����

#define TIME_TYPE_DAY               1 //Ԫ������ʱ����λΪ��
#define TIME_TYPE_SEC               2 //Ԫ������ʱ����λΪ��

#define PATCH_ID_ZERO               0 //����κţ���ҽ��׳�ֵ

#define SYNC_FLAG_FIRST             0 //�״η��ͳ�ֵ��Ϣ
#define SYNC_FLAG_MORE              1 //��η��ͳ�ֵ��Ϣ

#define AWARD_TYPE_BOUNDUSER        1   //���ʺŰ���콱
#define AWARD_TYPE_BOUNDNUM         2   //���콱�Ű󶨵��콱
#define AWARD_TYPE_NOBOUND          3   //�����ʺš��콱�Ű󶨵��콱
#define AWARD_TYPE_DEPOSIT          4   //���߻�Ԫ��

#define RESPONSE_YES                1 //��Ҫ
#define RESPONSE_NO                 2 //����Ҫ

/*-------------------------�ӿں��������궨��-------------------------*/
#define UNION_CLIENT        0x0         //����Ʒ�ϵͳ���������нӿڵĿͻ���
#define NORMAL_CLIENT       0x1         //����ʱ����Ȩ���۷���Ϣ�Ŀͻ���
#define GOLD_CONSUME_CLIENT 0x2         //�����ʱ����Ȩ���۷�������Ϣ�Ŀͻ���

#endif
