#ifndef _HSUSERUTIL_H
#define _HSUSERUTIL_H

//ERROR CODE 
#define	HSUSERUTIL_ERR_OK						0x00000000		//����
#define	HSUSERUTIL_ERR_UNKNOWN					0x0005A001		//�˼����� ����
#define	HSUSERUTIL_ERR_NOT_ADMIN				0x0005A002		//Administrator ������ �ƴ�
#define	HSUSERUTIL_ERR_NOT_NT					0x0005A003		//NT�迭�� �ƴ�

#define	HSUSERUTIL_ERR_DELSHADOWACNT_FAIL		0x0005A005		//��������� ���ϻ��� ����
#define	HSUSERUTIL_ERR_DELHIDEIDINFO_FAIL		0x0005A006		//�α׿½� �������� ���߱�� ���� �������� ����
#define	HSUSERUTIL_ERR_DELSHADOWACNTINFO_FAIL	0x0005A007		//������������� ��������
#define	HSUSERUTIL_ERR_ADDSHADOWACNT_FAIL		0x0005A008		//��������� ��������

#define	HSUSERUTIL_ERR_SETFLDRPERMISSION_FAIL	0x0005A10A		//���������ֱ� ����
#define	HSUSERUTIL_ERR_GETGROUPSID_FAIL			0x0005A10B		//�׷� SID ��� ����
#define	HSUSERUTIL_ERR_GETSECINFO_FAIL			0x0005A10C		//SD�� DACL���� ������
#define	HSUSERUTIL_ERR_ADDNEWACE_FAIL			0x0005A10D		//�� ACE ���� ����
#define	HSUSERUTIL_ERR_ADDNEWDACL_FAIL			0x0005A10E		//�� DACL ���� ����
#define	HSUSERUTIL_ERR_COPYOLDDACL_FAIL			0x0005A10F		//�� DACL�� ����DACL ���� ����
#define	HSUSERUTIL_ERR_ADDNEWACETONEWDACL_FAIL	0x0005A110		//�� DACL�� �� ACE �߰� ����

DWORD   __stdcall _AhnHsUserUtil_CreateUser ( );
DWORD	__stdcall _AhnHsUserUtil_SetFolderPermission(LPTSTR szPath);

#endif //_HSUSERUTIL_H