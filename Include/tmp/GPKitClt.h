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
        //Function: 处理由服务端发来的Auth数据。
		//Parameter: Object, 用于回调的对象指针，可能是一个socket，也可能是一个用户类等。甚至也可能为NULL,具体对象类型由游戏架构决定
		//		     SendToSvr，回调函数
		//           lpData:[in,out],数据指针，处理后作为输出数据指针
		//           nLen:数据长度
		//Return: 大于0为计算后数据的长度，否则表示计算失败
		
		virtual int GPK_API ProcessAuth(void * Object,fnGPK_CltCallBack SendToSvr,unsigned char * lpData, unsigned long nLen) = 0;

		

	};

	/* ====================================================================

	*函数说明:
	*启动GPK

	*参数说明:
	*UpdateServerURL - 更新服务器的URL地址
	*GameCode - 游戏代号, 8字节长, 加\0结尾最长为9字节长

	*返回值:
	*TRUE - 成功
	*FALSE - 失败
	*/

extern "C" IGPKCltDynCode * GPK_API GPKStart(IN LPCSTR UpdateServerURL,
												IN LPCSTR GameCode);

//extern "C"  bool GPK_API SetGPKCltCallBack(fnGPK_CltCallBack SendToSvr);
}


#endif
