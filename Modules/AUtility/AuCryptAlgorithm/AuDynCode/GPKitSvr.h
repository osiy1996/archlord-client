#ifndef SVRDYNCODE_H
#define SVRDYNCODE_H

#ifdef WIN32
	#ifndef GPK_API
		#define GPK_API   __stdcall
	#endif
#else
	#define GPK_API	
#endif

enum GPK_CHECK_INFO
{
	CHECK_EXCEPT = -1,    //CSAUTH�쳣
	CHECK_SUCCEED = 0,    //���ɹ�
	CHECK_CSAUTH_ERROR,   //�ͻ��˼���������ȷ���������ͻ���
	CHECK_FIND_SPEEDHACK, //���ּ��� 
	CHECK_FIND_MOUSE,     //����ģ��������
	CHECK_FIND_INJECT,    //����ע��
	CHECK_FIND_MODULE,    //�������ģ��
	CHECK_FIND_MEMORY,    //�����ڴ��޸�
	CHECK_FIND_WINDOW,    //������Ҵ���
	CHECK_NO_ADMIN,       //û�й���ԱȨ��
	CHECK_WINDOWS64BIT    //��ǰ����ϵͳ��64λ
};


/* ====================================================================
 * ISDSvrDynCode: server side dynamic code component. ISDSvrDynCode provides
 * dynamic encrypt and decrypt methods.
 *
 * Dynamic Code Binary File: compiled machine code of Encrypt and Decrypt
 * function are saved in binary files. File name format of these binary 
 * files are *.server.bin and *.client.bin, * stand for number, for example 
 * 1.server.bin, 1.client.bin. These numbers are called CodeIndex. Each 
 * server binary files has a corresponding client binary files with same 
 * CodeIndex. *.server.bin contains binary code for server, *.client.bin 
 * contains binary code for client. Both server and client binary files 
 * are used by server. Generally, there are multiple pairs of server and 
 * client binary files in one server. Each pair contains different encrypt
 * -decrypt binary code.
 *
 * Usage:
 * 1. Create ISDSvrDynCode component, call SDCreateSvrDynCode
 * 2. Load Dynamic Code Binary Files, call ISDSvrDynCode::LoadBinary
 * 3. Get random CodeIndex, call ISDSvrDynCode::GetRandIdx
 * 4. Get Client binary code, call ISDSvrDynCode::GetCltDynCode
 * 5. Transfer the client code to client
 * 6. To encrypt: call ISDSvrDynCode::Encode
 * 7. To decrypt: call ISDSvrDynCode::Decode
 * 8. when finish, call ISDSvrDynCode::Release to release component
 */
typedef void (GPK_API * fnGPK_SvrCallBack)(void * Object,unsigned char *lpData,unsigned long nLen);

namespace SGPK{

class IGPKCSAuth
{
public:
	//Function:����CSAuth����	
	//Parameter:ppDataRet:address of pointer ,recive the auth data
	//			Object:���ڻص��Ķ���ָ�룬������һ��socket��Ҳ������һ���û���ȡ�����Ҳ����ΪNULL,���������������Ϸ�ܹ�����
	//          SendToClt: �ص�����
	//return:   ����0Ϊ���ݳ��ȣ�-1Ϊ�쳣��0��ʾ�ϴε�Auth���ͻ���û�л�Ӧ�������������Auth�����
	virtual int  GetAuthData(const unsigned char **ppDataRet,void *Object,fnGPK_SvrCallBack SendToClt=NULL) =0;

	//Function:���ͻ��˷��ص�Auth���ݡ�
	//Parameter:ppDataRet:address of pointer ,recive the auth data
	//          lpData:���������ָ��
	//          nLen:���ݳ���
	//return:   ����ֵ���ݼ�GPK_CHECK_INFO;
	virtual int  CheckAuthReply(const unsigned char **ppDataRet,unsigned char * lpData, unsigned long nLen)=0;

	//Function: Release the object of IGPKCSAuth
	virtual void Release()=0;

};


class IGPKSvrDynCode
{
public:
    virtual GPK_API ~IGPKSvrDynCode() {}

    //Function: import dynamic code
    //Parameter: pszSvrBinDir, path for server dynamic code files
    //           pszsCltBinDir, path for client dynbamic code files
    //Return: the number of successfully imported pairs of files, -1 if error..
    virtual int GPK_API LoadBinary(const char * pszSvrBinDir, const char * pszsCltBinDir) = 0;

    //Function: get a random index for dynamic code
    //Return: >= 0 if successful��-1 if error
    virtual int GPK_API GetRandIdx() = 0;

    //Function: Get client dynamic code
    //Return: return length of pCodeRet,return zero and pCodeRet is a null point if nCodeIdx is invalid
    virtual int GPK_API GetCltDynCode(int nCodeIdx, const unsigned char **ppCodeRet) = 0;

    //Function: encrypt the data, it will return false if nCodeIdx is invalid
    //Parameter: lpData, the original data, it is used as a [in, out] parameter
    //		       nLen, the data length, it will not change after encryption
    //Return: true if successful
    virtual bool GPK_API Encode(unsigned char * lpData, unsigned long nLen, int nCodeIdx) = 0;

    //Function: decrypt the data, it will return false if nCodeIdx is invalid
    //Parameter: lpData, the original data to be decrypted, it is used as a [in, out] parameter
    //		       nLen, the data length, it will not change after decryption
    //Return: true if successful
    virtual bool GPK_API Decode(unsigned char * lpData, unsigned long nLen, int nCodeIdx) = 0;

	//Function: Release the compoment
    virtual void GPK_API Release() = 0;


	//GPK2.X
	//Function:����Auth�����ļ�
	//Parameter:pszCmdFileName���ļ���
	//return: true if successful
	virtual bool          GPK_API  LoadAuthFile(const char * pszCmdFileName) = 0;


	////Function:Ϊһ�����ӻỰ����һ��IGPKCSAuth����
	virtual IGPKCSAuth*   GPK_API  AllocAuthObject() = 0;
};



//Function: Create ISDSvrDynCode component
#ifdef WIN32
IGPKSvrDynCode * GPK_API  GPKCreateSvrDynCode();
//Parameter: Object,A Object of user,it will be use as a parnmeter in GPKCALLBack


#endif

};





#ifndef WIN32
extern "C" SGPK::IGPKSvrDynCode * GPK_API GPKCreateSvrDynCode();
typedef SGPK::IGPKSvrDynCode* (GPK_API *PFN_GPKCreateSvrDynCode)();
#endif

#endif


