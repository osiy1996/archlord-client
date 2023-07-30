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
	CHECK_EXCEPT = -1,    //CSAUTH异常
	CHECK_SUCCEED = 0,    //检查成功
	CHECK_CSAUTH_ERROR,   //客户端计算结果不正确，非正常客户端
	CHECK_FIND_SPEEDHACK, //发现加速 
	CHECK_FIND_MOUSE,     //发现模拟鼠标键盘
	CHECK_FIND_INJECT,    //发现注入
	CHECK_FIND_MODULE,    //发现外挂模块
	CHECK_FIND_MEMORY,    //发现内存修改
	CHECK_FIND_WINDOW,    //发现外挂窗口
	CHECK_NO_ADMIN,       //没有管理员权限
	CHECK_WINDOWS64BIT    //当前操作系统是64位
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
	//Function:生成CSAuth数据	
	//Parameter:ppDataRet:address of pointer ,recive the auth data
	//			Object:用于回调的对象指针，可能是一个socket，也可能是一个用户类等。甚至也可能为NULL,具体对象类型由游戏架构决定
	//          SendToClt: 回调函数
	//return:   大于0为数据长度，-1为异常，0表示上次的Auth检查客户端没有回应。有外挂屏蔽了Auth封包。
	virtual int  GetAuthData(const unsigned char **ppDataRet,void *Object,fnGPK_SvrCallBack SendToClt=NULL) =0;

	//Function:检查客户端返回的Auth数据。
	//Parameter:ppDataRet:address of pointer ,recive the auth data
	//          lpData:输入的数据指针
	//          nLen:数据长度
	//return:   返回值内容见GPK_CHECK_INFO;
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
    //Return: >= 0 if successful，-1 if error
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
	//Function:载入Auth数据文件
	//Parameter:pszCmdFileName：文件名
	//return: true if successful
	virtual bool          GPK_API  LoadAuthFile(const char * pszCmdFileName) = 0;


	////Function:为一个连接会话生成一个IGPKCSAuth对象。
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


