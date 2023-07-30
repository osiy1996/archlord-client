#ifndef GPKIT_H
#define GPKIT_H

#ifndef GPK_API
#define GPK_API __stdcall
#endif


/* ====================================================================
* ISDCltDynCode: client side dynamic code component. ISDCltDynCode provides
* dynamic encrypt and decrypt methods.
*
* Usage: 
* 1. Create ISDCltDynCode component, call SDCreateCltDynCode
* 2. Call ISDCltDynCode.SetDynCode to when getting dynamic code from server
* 3. To encrypt: call ISDCltDynCode::Encode
* 4. To decrypt: call ISDCltDynCode::Decode
* 5. when finish, call ISDCltDynCode::Release to release the component
*/

typedef void (GPK_API * fnGPK_CltCallBack)(void * Object,unsigned char *lpData,unsigned long nLen);

namespace SGPK{

	class IGPKCltDynCode
	{
	public:
		//Function: set dynamic code
		//Return: true if successful
		//Remark: this will overwrite the dynamic doe set before
		virtual bool GPK_API SetDynCode(const unsigned char* lpDynCode, int nLen) = 0;

		//Function: encrypt the data, it will return false if called without setting dynamic code
		//Parameter: lpData, the original data, it is used as a [in, out] parameter
		//		       nLen, the data length, it will not change after encryption
		//Return: true if successful
		virtual bool GPK_API Encode(unsigned char * lpData, unsigned long nLen) = 0;

		//Function: decrypt the data, it will return false if called without setting dynamic code
		//Parameter: lpData, the original data to be decrypted, it is used as a [in, out] parameter
		//		       nLen, the data length, it will not change after decryption
		//Return: true if successful
		virtual bool GPK_API Decode(unsigned char * lpData, unsigned long nLen) = 0;

		//Function: Release the component
		virtual void GPK_API Release() = 0;


		//GPK2.X
        //Function: �����ɷ���˷�����Auth���ݡ�
		//Parameter: Object, ���ڻص��Ķ���ָ�룬������һ��socket��Ҳ������һ���û���ȡ�����Ҳ����ΪNULL,���������������Ϸ�ܹ�����
		//		     SendToSvr���ص�����
		//           lpData:[in,out],����ָ�룬�������Ϊ�������ָ��
		//           nLen:���ݳ���
		//Return: ����0Ϊ��������ݵĳ��ȣ������ʾ����ʧ��
		
		virtual int GPK_API ProcessAuth(void * Object,fnGPK_CltCallBack SendToSvr,unsigned char * lpData, unsigned long nLen) = 0;

		

	};

	/* ====================================================================

	*����˵��:
	*����GPK

	*����˵��:
	*UpdateServerURL - ���·�������URL��ַ
	*GameCode - ��Ϸ����, 8�ֽڳ�, ��\0��β�Ϊ9�ֽڳ�

	*����ֵ:
	*TRUE - �ɹ�
	*FALSE - ʧ��
	*/

extern "C" IGPKCltDynCode * GPK_API GPKStart(IN LPCSTR UpdateServerURL,
												IN LPCSTR GameCode);

//extern "C"  bool GPK_API SetGPKCltCallBack(fnGPK_CltCallBack SendToSvr);
}


#endif
