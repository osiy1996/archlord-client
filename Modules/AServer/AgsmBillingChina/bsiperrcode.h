/*
* Copyright (c) 2005,�Ϻ�ʢ������Ʒ�ƽ̨��
* All rights reserved.
*
* �ļ����ƣ�bsiperrcode.h
* ժ    Ҫ�����ش����붨��
* ��ǰ�汾��1.4
* ��    �ߣ�����
* ������ڣ�2005��9��12��
*
* �޸����ݣ�1.4�汾�������ļ�
* �޸�ʱ�䣺2005��9��12��
*
* �޸����ݣ�1.5�汾�������������룺
* �޸�ʱ�䣺2005-12-29 16:01 zlc
*/


/*
 * cbs���ݿ�洢���̴����뷶Χ��100001��109999��
 * lbs���ݿ�洢���̴����뷶Χ��120001��129999��
 * ���ݸ�ʽ�Ƿ������뷶Χ��200001��299999��
 * cbsҵ��������뷶Χ��300001��399999��
 * lbsҵ��������뷶Χ��400001��499999��
 * ����δ����:-1
 */

#ifndef _BSIP_ERRCODE_H_
#define _BSIP_ERRCODE_H_

enum    //���ݸ�ʽ�Ƿ�������
{
    //�û������ʺ�
    SDIDLONGER             = -200101,   //�������
    SDIDNULL               = -200102,   //Ϊ��
    SDIDILLEGAL            = -200103,   //���Ƿ��ַ�
    SDIDOTHER              = -200199,   //��������

    //�û�PT�ַ��ʺ�
    PTIDLONGER             = -200201,   //�������
    PTIDNULL               = -200202,   //Ϊ��
    PTIDILLEGAL            = -200203,   //���Ƿ��ַ�
    PTIDALLNUM             = -200204,   //ȫΪ���ֻ������ֿ�ͷ
    PTIDOTHER              = -200299,   //��������

    //ҵ�����ͱ��
    SERVIDLONGER           = -200301,   //�������
    SERVIDNULL             = -200302,   //Ϊ��
    SERVIDILLEGAL          = -200303,   //���Ƿ��ַ�
    SERVIDOTHER            = -200399,   //��������

    //��Ϸ�����
    AREAIDLONGER           = -200401,   //�������
    AREAIDNULL             = -200402,   //Ϊ��
    AREAIDILLEGAL          = -200403,   //���Ƿ���
    AREAIDOTHER            = -200499,   //��������

    //�Ựid
    SESSIDLONGER           = -200501,   //�������
    SESSIDNULL             = -200502,   //Ϊ��
    SESSIDILLEGAL          = -200503,   //���Ƿ���
    SESSIDERRFORMAT        = -200504,   //��ʽ������ɲ��Ϸ���
    SESSIDOTHER            = -200599,   //��������

    //���ݰ�id
    UNIQUEIDLONGER         = -200601,   //�������
    UNIQUEIDNULL           = -200602,   //Ϊ��
    UNIQUEIDILLEGAL        = -200603,   //���Ƿ���
    UNIQUEIDERRFORMAT      = -200604,   //��ʽ������ɲ��Ϸ���
    UNIQUEIDREPEAT         = -200605,   //uniqueid�ظ�
    UNIQUEIDOTHER          = -200699,   //��������

    //����Ӧ�̱��
    SPIDLONGER             = -200701,   //�������
    SPIDNULL               = -200702,   //Ϊ��
    SPIDILLEGAL            = -200703,   //���Ƿ���
    SPIDOTHER              = -200799,   //��������

    //����
    GROUPIDLONGER          = -200801,   //�������
    GROUPIDNULL            = -200802,   //Ϊ��
    GROUPIDILLEGAL         = -200803,   //���Ƿ���
    GROUPIDOTHER           = -200899,   //��������

    //��Ϸ���߱��
    ITEMIDLONGER           = -200901,   //�������
    ITEMIDNULL             = -200902,   //Ϊ��
    ITEMIDILLEGAL          = -200903,   //���Ƿ���
    ITEMIDERRVALUE         = -200904,   //�������δȡ�ض�ֵ��
    ITEMIDOTHER            = -200999,   //��������

    //�û�����
    USERTYPELONGER         = -201101,   //�������
    USERTYPENULL           = -201102,   //Ϊ��
    USERTYPEILLEGAL        = -201103,   //���Ƿ���
    USERTYPEOTHER          = -201199,   //��������

    //��ʼʱ��
    BEGINTIMELONGER        = -201301,   //�������
    BEGINTIMENULL          = -201302,   //Ϊ��
    BEGINTIMEILLEGAL       = -201303,   //���Ƿ���
    BEGINTIMEERRLONGFOMAT  = -201304,   //��ʽ���󣬱���ΪYYYY-MM-DD hh:mi:ss
    BEGINTIMEERRSHORTFOMAT = -201305,   //���Ƿ��֣�����ΪYYYY-MM-DD
    BEGINTIMEOTHER         = -201399,   //��������

    //����ʱ��
    ENDTIMELONGER          = -201401,   //�������
    ENDTIMENULL            = -201402,   //Ϊ��
    ENDTIMEILLEGAL         = -201403,   //���Ƿ���
    ENDTIMEERRLONGFOMAT    = -201404,   //��ʽ���󣬱���ΪYYYY-MM-DD hh:mi:ss
    ENDTIMEERRSHORTFOMAT   = -201405,   //���Ƿ��֣�����ΪYYYY-MM-DD
    ENDTIMEERRVALUE        = -201406,   //����ʱ����ڿ�ʼʱ��
    ENDTIMEOTHER           = -201499,   //��������

    //���ݰ�id
    ORDERIDLONGER          = -201501,   //�������
    ORDERIDNULL            = -201502,   //Ϊ��
    ORDERIDILLEGAL         = -201503,   //���Ƿ���
    ORDERIDERRFORMAT       = -201504,   //��ʽ������ɲ��Ϸ���
    ORDERIDREPEAT          = -201505,   //orderid�ظ�
    ORDERIDOTHER           = -201599,   //��������

    //���������
    HOSTIDLONGER           = -201701,   //�������
    HOSTIDNULL             = -201702,   //Ϊ��
    HOSTIDILLEGAL          = -201703,   //���Ƿ���
    HOSTIDOTHER            = -201799,   //��������

    //��������
    PAYTYPELONGER          = -201901,   //�������
    PAYTYPENULL            = -201902,   //Ϊ��
    PAYTYPEILLEGAL         = -201903,   //���Ƿ���
    PAYTYPEERRVALUE        = -201904,   //�������δȡ�ض�ֵ��
    PAYTYPEOTHER           = -201999,   //��������

    //��������
    APPTYPELONGER          = -202001,   //�������
    APPTYPENULL            = -202002,   //Ϊ��
    APPTYPEILLEGAL         = -202003,   //���Ƿ���
    APPTYPEERRVALUE        = -202004,   //�������δȡ�ض�ֵ��
    APPTYPEOTHER           = -202099,   //��������

    //�������
    COUPLETYPELONGER       = -202101,   //�������
    COUPLETYPENULL         = -202102,   //Ϊ��
    COUPLETYPEILLEGAL      = -202103,   //���Ƿ���
    COUPLETYPEERRVALUE     = -202104,   //�������δȡ�ض�ֵ��
    COUPLETYPEOTHER        = -202199,   //��������

    //���
    AMOUNTLONGER           = -202201,   //�������
    AMOUNTNULL             = -202202,   //Ϊ��
    AMOUNTILLEGAL          = -202203,   //���Ƿ���
    AMOUNTNEGATIVE         = -202204,   //����Ϊ����
    AMOUNTZERO             = -202205,   //����Ϊ��
    AMOUNTERRVALUE         = -202206,   //�������
    AMOUNTOTHER            = -202299,   //��������

    //�Ʒ��ۿ���
    DISCOUNTLONGER         = -202301,   //�������
    DISCOUNTNULL           = -202302,   //Ϊ��
    DISCOUNTILLEGAL        = -202303,   //���Ƿ���
    DISCOUNTOTHER          = -202399,   //��������

    //�ͻ���IP��ַ
    IPLONGER               = -202501,   //�������
    IPNULL                 = -202502,   //Ϊ��
    IPILLEGAL              = -202503,   //���Ƿ���
    IPERRVALUE             = -202504,   //������󣨲���ָ����Χ֮�ڣ�
    IPOTHER                = -202599,   //��������

    //Ԥ����������ʱ��
    LOCKPERIODLONGER       = -202801,   //�������
    LOCKPERIODNULL         = -202802,   //Ϊ��
    LOCKPERIODILLEGAL      = -202803,   //���Ƿ���
    LOCKPERIODERRVALUE     = -202804,   //������󣨲���ָ����Χ֮�ڣ�
    LOCKPERIODOTHER        = -202899,   //��������

    //���ؽ��
    RESULTLONGER           = -204101,   //�������
    RESULTNULL             = -204102,   //Ϊ��
    RESULTILLEGAL          = -204103,   //���Ƿ���
    RESULTERRVALUE         = -204104,   //���������ָ��ֵ��һ�£�
    RESULTOTHER            = -204199,   //��������

    //��Ҫ��Ӧ��־
    RESPONSABLELONGER      = -204201,   //�������
    RESPONSABLENULL        = -204202,   //Ϊ��
    RESPONSABLEILLEGAL     = -204203,   //���Ƿ���
    RESPONSABLEERRVALUE    = -204204,   //���������ָ��ֵ��һ�£�
    RESPONSABLEOTHER       = -204299,   //��������

    //ȷ�ϱ�־
    CONFIRMLONGER          = -204301,   //�������
    CONFIRMNULL            = -204302,   //Ϊ��
    CONFIRMILLEGAL         = -204303,   //���Ƿ���
    CONFIRMERRVALUE        = -204304,   //���������ָ��ֵ��һ�£�
    CONFIRMOTHER           = -204399,   //��������

    //״̬
    STATUSLONGER           = -204401,   //�������
    STATUSNULL             = -204402,   //Ϊ��
    STATUSILLEGAL          = -204403,   //���Ƿ���
    STATUSERRVALUE         = -204404,   //���������ָ��ֵ��һ�£�
    STATUSOTHER            = -204499,   //��������

    //�콱����
    AWARDTYPELONGER        = -204601,   //�������
    AWARDTYPENULL          = -204602,   //Ϊ��
    AWARDTYPEILLEGAL       = -204603,   //���Ƿ���
    AWARDTYPEERRVALUE      = -204604,   //���������ָ��ֵ��һ�£�
    AWARDTYPEOTHER         = -204699,   //��������

    //�콱����
    AWARDNUMLONGER         = -205001,   //�������
    AWARDNUMNULL           = -205002,   //Ϊ��
    AWARDNUMILLEGAL        = -205003,   //���Ƿ���
    AWARDNUMOTHER          = -205099,   //��������

    //�콱���к�
    AWARDSNLONGER          = -205101,   //�������
    AWARDSNNULL            = -205102,   //Ϊ��
    AWARDSNILLEGAL         = -205103,   //���Ƿ���
    AWARDSNOTHER           = -205199,   //��������

    //����ʱ��
    AWARDTIMELONGER        = -205201,   //�������
    AWARDTIMENULL          = -205202,   //Ϊ��
    AWARDTIMEILLEGAL       = -205203,   //���Ƿ���
    AWARDTIMEERRLONGFOMAT  = -205204,   //��ʽ���󣬱���ΪYYYY-MM-DD hh:mi:ss
    AWARDTIMEERRSHORTFOMAT = -205205,   //���Ƿ��֣�����ΪYYYY-MM-DD
    AWARDTIMEOTHER         = -205299,   //��������

    //����ʱ������
    TIMEAMOUNTLONGER       = -205501,   //�������
    TIMEAMOUNTNULL         = -205502,   //Ϊ��
    TIMEAMOUNTILLEGAL      = -205503,   //���Ƿ���
    TIMEAMOUNTNEGATIVE     = -205504,   //����Ϊ����
    TIMEAMOUNTZERO         = -205505,   //����Ϊ��
    TIMEAMOUNTOTHER        = -205599,   //��������

    //����ʱ������
    TIMETYPELONGER         = -205601,   //�������
    TIMETYPENULL           = -205602,   //Ϊ��
    TIMETYPEILLEGAL        = -205603,   //���Ƿ���
    TIMETYPEERRVALUE       = -205604,   //�������δȡ�ض�ֵ��
    TIMETYPEOTHER          = -205699,   //��������

    //��ֵ���κ�
    PATCHIDLONGER          = -206201,   //�������
    PATCHIDNULL            = -206202,   //Ϊ��
    PATCHIDILLEGAL         = -206203,   //���Ƿ���
    PATCHIDERRVALUE        = -206204,   //����������κŲ����ڣ�
    PATCHIDOTHER           = -206299,   //��������

    //�콱���κ�
    AWARDPATCHIDLONGER     = -206301,   //�������
    AWARDPATCHIDNULL       = -206302,   //Ϊ��
    AWARDPATCHIDILLEGAL    = -206303,   //���Ƿ���
    AWARDPATCHIDERRVALUE   = -206304,   //����������κŲ����ڣ�
    AWARDPATCHIDOTHER      = -206399,   //��������

    //��������ʺ�
    BUYERSDIDLONGER        = -206401,   //�������
    BUYERSDIDNULL          = -206402,   //Ϊ��
    BUYERSDIDILLEGAL       = -206403,   //���Ƿ��ַ�
    BUYERSDIDOTHER         = -206499,   //��������

    //���PT�ַ��ʺ�
    BUYERPTIDLONGER        = -206501,   //�������
    BUYERPTIDNULL          = -206502,   //Ϊ��
    BUYERPTIDILLEGAL       = -206503,   //���Ƿ��ַ�
    BUYERPTIDALLNUM        = -206504,   //ȫΪ���ֻ������ֿ�ͷ
    BUYERPTIDOTHER         = -206599,   //��������

    //�������
    BUYERGROUPIDLONGER     = -206601,   //�������
    BUYERGROUPIDNULL       = -206602,   //Ϊ��
    BUYERGROUPIDILLEGAL    = -206603,   //���Ƿ���
    BUYERGROUPIDOTHER      = -206699,   //��������

    //���ҵ�����ͱ��
    BUYERSERVIDLONGER      = -206701,   //�������
    BUYERSERVIDNULL        = -206702,   //Ϊ��
    BUYERSERVIDILLEGAL     = -206703,   //���Ƿ��ַ�
    BUYERSERVIDOTHER       = -206799,   //��������

    //�����Ϸ�����
    BUYERAREAIDLONGER      = -206801,   //�������
    BUYERAREAIDNULL        = -206802,   //Ϊ��
    BUYERAREAIDILLEGAL     = -206803,   //���Ƿ���
    BUYERAREAIDOTHER       = -206899,   //��������

    //���������ֶ�(�ַ���)
    RESERVECHAROTHER       = -206999,   //��������

    //���������ֶΣ����ͣ�
    RESERVEINTOTHER        = -207099,   //��������

	//��Ϣ���ܡ�����Ϣ����
	NOTIFYTYPELONGER       = -209301,   //�������
	NOTIFYTYPEUNLL         = -209302,   //Ϊ��
	NOTIFYTYPEILLEGAL      = -209303,   //���Ƿ���
	NOTIFYTYPEOTHER        = -209399,   //��������
	
	//��Ϣ���ܡ�����Ϣ����--����
	NOTIFYDATALONGER       = -209401,   //�������
	NOTIFYDATAUNLL         = -209402,   //Ϊ��
	NOTIFYDATAILLEGAL      = -209403,   //���Ƿ��� 
	NOTIFYDATAERRVALUE     = -209404,   //�������δȡ�ض�ֵ��
	NOTIFYDATAOTHER        = -209499,   //��������

	//��Ϣ���ܡ�����Ϣ����--�ַ���
	NOTIFYCONTENTLONGER    = -209501,   //�������
	NOTIFYCONTENTUNLL      = -209502,   //Ϊ��
	NOTIFYCONTENTILLEGAL   = -209503,   //���Ƿ���
	NOTIFYCONTENTERRVALUE  = -209504,   //�������δȡ�ض�ֵ��
	NOTIFYCONTENTOTHER     = -209599,   //��������

    //��Ч����������
    ITEMNUMZERO            = -209601,   //����������Ϊ0
    ITEMNUMERRVALUE        = -209602,   //�������δȡ�ض�ֵ��
    ITEMNUMOTHER           = -209699,   //��������

    //��Ϸ��������
    ITEMCOUNTZERO          = -209701,  //����������Ϊ0
    ITEMCOUNTILLEGAL       = -209702,  //���Ƿ���
    ITEMCOUNTOTHER         = -209799,  //��������

    //��Ϸ���ߵ���
    ITEMPRICEZERO          = -209801,  //����������Ϊ0
    ITEMPRICEILLEGAL       = -209802,  //���Ƿ���
    ITEMPRICEOTHER         = -209899,  //��������
};

enum  //cbsҵ���������
{
    CBSDB_IS_DISCONNECT    = -300001,  //cbs���ݿ������쳣
    CBSDB_PROC_ERROR       = -300002,  //cbs sqlִ�д���
    UNAUTHOR_OPERATION     = -300003,  //δ��Ȩ�����������

    AWARD_TYPE_OUTLAW      = -300101,  //�콱���ʹ���award_typeȡֵ��Χ1-3��
    AWARD_NUM_ERR          = -300102,  //�콱���벻����
    AWARD_PITCH_ERR        = -300103,  //�콱���κŲ�����
    AWARD_NO_AWARD         = -300104,  //����޽�Ʒ
    AWARD_HAS_TAKE         = -300105,  //��ҽ�Ʒ����ȡ
    AWARD_LOCKED           = -300106,  //��ҽ�Ʒ������
    AWARD_SERV_ERR         = -300107,  //�޴�service
    AMOUNT_NOT_MATCHING    = -300201,  //��һ��
};

enum   //lbsҵ�������
{
    LBSDB_IS_DISCONNECT    = -400001,  //lbs���ݿ������쳣
    LBSDB_PROC_ERROR       = -400002,  //lbs sqlִ�д���
};

enum   //����
{
    RES_TIMEOUT            = -390001,  //��Ӧ��ʱ,ռ��һ��3����ε�,Ҫע��
    UNDECLARE_ERROR        = -1        //δ�����δ֪�Ĵ���
};

#endif
