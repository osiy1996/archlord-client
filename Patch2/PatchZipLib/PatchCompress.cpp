
/* zlib �� ����ϴ� �� */
//#include <afxwin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

#include "zlib.h"
#include "PatchCompress.h"

FILE* CCompress::OpenFileMakeDir( char *pstrFileName, char *pstrOption )
{
	FILE			*fpTemp;

	fpTemp = fopen( pstrFileName, pstrOption );

	if( fpTemp == NULL )
	{
		int		iErrorCode;

		iErrorCode = GetLastError();
		
		//ErrorCode�� 0x03�̸� ������ ���°��!! ������ ������ش�.
		if( iErrorCode == 0x03 )
		{
			int				iCurrentPos;
			int				iDPathSize;
			char			strDir[256];

			iCurrentPos = 0;
			iDPathSize = strlen( pstrFileName );
			memset( strDir, 0, 256 );

			for( int i=0; i<iDPathSize; i++ )
			{
				strDir[iCurrentPos] = pstrFileName[i];
				iCurrentPos++;

				if( pstrFileName[i] == '\\' )
				{
					CreateDirectory( strDir, NULL );
					
					//������ �����Ѵ�.
					fpTemp =fopen( pstrFileName, pstrOption );

					if( fpTemp != NULL )
					{
						break;
					}
					else
					{
						if( GetLastError() == 0x03 )
						{
							continue;
						}
						else
						{
							fpTemp = NULL;
							break;
						}
					}
				}
			}			
		}
		else
		{
			fpTemp = NULL;
		}
	}

	return fpTemp;
}

void CCompress::compressFile(const char* src_filename, char *pstrDestDir, char *pstrDestFileName )
{
	z_stream z;                     /* ���̺귯���� ��ȯ�ϱ� ���� ����ü */
	unsigned char* inbuf = NULL;
	unsigned char* outbuf = NULL;

	char dest_filename[256];
	char strCurrentDir[256];

	unsigned int count, flush, status;
	unsigned long INBUFSIZE;
	unsigned long OUTBUFSIZE;

	//���� ���丮�� �����Ѵ�.
	GetCurrentDirectory( 256, strCurrentDir );

	//���� ���丮�� ������ ���丮(psrtDestDir)�� �����Ѵ�.
	SetCurrentDirectory( pstrDestDir );

	//���� ���� ����
	sprintf( dest_filename, "%s\\%s.jzp", pstrDestDir, pstrDestFileName );

	m_fpSourceFile = fopen(src_filename, "rb");
	if(m_fpSourceFile == NULL)
	{
		printf("%s open failed\n", src_filename);
		exit(1);
	}

	m_fpDestFile = OpenFileMakeDir( pstrDestFileName, "wb" );
	if(m_fpDestFile == NULL)
	{
		printf("%s open failed\n", dest_filename);
		exit(1);
	}

	//������ �ϱ� ���� ȯ�� ����
	INBUFSIZE = filelength(fileno(m_fpSourceFile));
	OUTBUFSIZE = INBUFSIZE+1024;
	inbuf = (unsigned char *)malloc(INBUFSIZE);
	outbuf = (unsigned char *)malloc(OUTBUFSIZE);
	
    /* ��� �޸� �޴�����Ʈ�� ���̺귯���� �ñ�� */
    z.zalloc = Z_NULL;
    z.zfree = Z_NULL;
    z.opaque = Z_NULL;

    /* �ʱ�ȭ */
    /* ��2 �μ��� �����ǵ� ��. 0~9 �� ������ ������, 0 �� ������ */
    /* Z_DEFAULT_COMPRESSION (= 6)�� ǥ�� */
    if (deflateInit(&z, Z_DEFAULT_COMPRESSION) != Z_OK) {
        fprintf(stderr, "deflateInit: %s\n", (z.msg) ?  z.msg : "???");
        exit(1);
    }

    z.avail_in = 0;             /* �Է� �������� �������� ����Ʈ�� */
    z.next_out = outbuf;        /* ��� ������ */
    z.avail_out = OUTBUFSIZE;    /* ��� ������ ������ */

    /* ����� deflate()�� ��2 �μ��� Z_NO_FLUSH �� �� ȣ���Ѵ� */
    flush = Z_NO_FLUSH;

    while (1) {
        if (z.avail_in == 0) {  /* �Է��� ���ϸ� */
            z.next_in = inbuf;  /* �Է� �����͸� �Է� ������ ���ο� */
            z.avail_in = fread(inbuf, 1, INBUFSIZE, m_fpSourceFile); /* �����͸� �о���δ� */

            /* �Է��� �������� �Ǹ�(��) deflate()�� ��2 �μ��� Z_FINISH �� �Ѵ� */
            if (z.avail_in < INBUFSIZE) flush = Z_FINISH;
        }
        status = deflate(&z, flush); /* �����Ѵ� */
        if (status == Z_STREAM_END) break; /* �Ϸ� */
        if (status != Z_OK) {   /* ���� */
            fprintf(stderr, "deflate: %s\n", (z.msg) ?  z.msg : "???");
            exit(1);
        }
        if (z.avail_out == 0) { /* ��� ���۰� ���ϸ� */
            /* ������ �᳽�� */
            if (fwrite(outbuf, 1, OUTBUFSIZE, m_fpDestFile) != OUTBUFSIZE) {
                fprintf(stderr, "Write error\n");
                exit(1);
            }
            z.next_out = outbuf; /* ��� ���� �ܷ��� �������� �ǵ����� */
            z.avail_out = OUTBUFSIZE; /* ��� �����͸� �������� �ǵ����� */
        }
    }

    /* �������� ���س��� */
    if ((count = OUTBUFSIZE - z.avail_out) != 0) {
        if (fwrite(outbuf, 1, count, m_fpDestFile) != count) {
            fprintf(stderr, "Write error\n");
            exit(1);
        }
    }

    /* ��ó�� */
    if (deflateEnd(&z) != Z_OK) {
        fprintf(stderr, "deflateEnd: %s\n", (z.msg) ?  z.msg : "???");
        exit(1);
    }

	free(inbuf);
	free(outbuf);
	fclose(m_fpSourceFile);
	fclose(m_fpDestFile);

	//���� ���丮�� ������.
	SetCurrentDirectory( strCurrentDir );
}

void CCompress::decompressFile(const char* src_filename, char *pstrDestDir, char *pstrDestFileName )
{
	z_stream z;
	unsigned char* inbuf = NULL;
	unsigned char* outbuf = NULL;

	char			dest_filename[256];
	char			strCurrentDir[256];

	unsigned int count, status;
	unsigned long INBUFSIZE;
	unsigned long OUTBUFSIZE;

	//���� ���丮�� �����Ѵ�.
	GetCurrentDirectory( 256, strCurrentDir );

	//���� ���丮�� ������ ���丮(psrtDestDir)�� �����Ѵ�.
	SetCurrentDirectory( pstrDestDir );

	//���� ���� ����
	int				iDestPathSize;

//	sprintf( dest_filename, "%s%s", pstrDestDir, pstrDestFileName );
	sprintf( dest_filename, "%s\\%s", pstrDestDir, pstrDestFileName );

	//�ڿ����� jzp�� �����.
	iDestPathSize = strlen(dest_filename);
	memset( &dest_filename[iDestPathSize-4], 0, 3 );

	m_fpSourceFile = fopen(src_filename, "rb");
	if(m_fpSourceFile == NULL)
	{
		printf("%s open failed\n", src_filename);
		exit(1);
	}

	m_fpDestFile = OpenFileMakeDir( pstrDestFileName, "wb" );
	if(m_fpDestFile == NULL)
	{
		printf("%s open failed\n", dest_filename);
		exit(1);
	}

	//������ �ϱ� ���� ȯ�� ����
	INBUFSIZE = filelength(fileno(m_fpSourceFile));
	OUTBUFSIZE = INBUFSIZE;
	inbuf = (unsigned char *)malloc(INBUFSIZE);
	outbuf = (unsigned char *)malloc(OUTBUFSIZE);
	
    /* ��� �޸� �޴�����Ʈ�� ���̺귯���� �ñ�� */
    z.zalloc = Z_NULL;
    z.zfree = Z_NULL;
    z.opaque = Z_NULL;

    /* �ʱ�ȭ */
    z.next_in = Z_NULL;
    z.avail_in = 0;
    if (inflateInit(&z) != Z_OK) {
        fprintf(stderr, "inflateInit: %s\n", (z.msg) ?  z.msg : "???");
        exit(1);
    }

    z.next_out = outbuf;        /* ��� ������ */
    z.avail_out = OUTBUFSIZE;    /* ��� ���� �ܷ� */
    status = Z_OK;

    while (status != Z_STREAM_END) {
        if (z.avail_in == 0) {  /* �Է� �ܷ��� ���ΰ� �Ǹ� */
            z.next_in = inbuf;  /* �Է� �����͸� �������� �ǵ����� */
            z.avail_in = fread(inbuf, 1, INBUFSIZE, m_fpSourceFile); /* �����͸� �д´� */
        }
        status = inflate(&z, Z_NO_FLUSH); /* ���� */
        if (status == Z_STREAM_END) break; /* �Ϸ� */
        if (status != Z_OK) {   /* ���� */
            fprintf(stderr, "inflate: %s\n", (z.msg) ?  z.msg : "???");
            exit(1);
        }
        if (z.avail_out == 0) { /* ��� ���۰� ���ϸ� */
            /* ������ �᳽�� */
            if (fwrite(outbuf, 1, OUTBUFSIZE, m_fpDestFile) != OUTBUFSIZE) {
                fprintf(stderr, "Write error\n");
                exit(1);
            }
            z.next_out = outbuf; /* ��� �����͸� �������� �ǵ����� */
            z.avail_out = OUTBUFSIZE; /* ��� ���� �ܷ��� �������� �ǵ����� */
        }
    }

    /* �������� ���س��� */
    if ((count = OUTBUFSIZE - z.avail_out) != 0) {
        if (fwrite(outbuf, 1, count, m_fpDestFile) != count) {
            fprintf(stderr, "Write error\n");
            exit(1);
        }
    }

    /* ��ó�� */
    if (inflateEnd(&z) != Z_OK) {
        fprintf(stderr, "inflateEnd: %s\n", (z.msg) ?  z.msg : "???");
        exit(1);
    }

	fclose( m_fpDestFile );
	free( outbuf );

	//���� ���丮�� ������.
	SetCurrentDirectory( strCurrentDir );
}

void CCompress::decompressMemory(const char* pstrCompressBlock, int iCompressBlockSize, char *pstrDestDir, char *pstrDestFileName )
{
	z_stream z;
	unsigned char* inbuf = NULL;
	unsigned char* outbuf = NULL;

	char			dest_filename[256];
	char			strCurrentDir[256];
	int				iCurrentPos = 0;

	unsigned int count, status;
	unsigned long INBUFSIZE;
	unsigned long OUTBUFSIZE;

	//���� ���丮�� �����Ѵ�.
	GetCurrentDirectory( 256, strCurrentDir );

	//���� ���丮�� ������ ���丮(psrtDestDir)�� �����Ѵ�.
	SetCurrentDirectory( pstrDestDir );

	//���� ���� ����
	int				iDestPathSize;

	sprintf( dest_filename, "%s\\%s", pstrDestDir, pstrDestFileName );

	//�ڿ����� jzp�� �����.
	iDestPathSize = strlen(dest_filename);
	memset( &dest_filename[iDestPathSize-4], 0, 3 );

	//���� �����Ұ��� ������ Read-only �Ӽ��� ���������� �𸣴� Normal�� ���� ó���Ѵ�.
	SetFileAttributes( pstrDestFileName, FILE_ATTRIBUTE_NORMAL );

	m_fpDestFile = OpenFileMakeDir( pstrDestFileName, "wb" );

	if(m_fpDestFile != NULL)
	{
		//������ �ϱ� ���� ȯ�� ����
		INBUFSIZE = iCompressBlockSize;
		OUTBUFSIZE = INBUFSIZE;
		inbuf = (unsigned char *)pstrCompressBlock;
		outbuf = (unsigned char *)malloc(OUTBUFSIZE);
		
		/* ��� �޸� �޴�����Ʈ�� ���̺귯���� �ñ�� */
		z.zalloc = Z_NULL;
		z.zfree = Z_NULL;
		z.opaque = Z_NULL;

		/* �ʱ�ȭ */
		z.next_in = Z_NULL;
		z.avail_in = 0;
		if (inflateInit(&z) != Z_OK) {
			fprintf(stderr, "inflateInit: %s\n", (z.msg) ?  z.msg : "???");
			exit(1);
		}

		z.next_out = outbuf;        /* ��� ������ */
		z.avail_out = OUTBUFSIZE;    /* ��� ���� �ܷ� */
		status = Z_OK;

		while (status != Z_STREAM_END) {
			if (z.avail_in == 0) {  /* �Է� �ܷ��� ���ΰ� �Ǹ� */
				z.next_in = inbuf;  /* �Է� �����͸� �������� �ǵ����� */
	//            z.avail_in = fread(inbuf, 1, INBUFSIZE, m_fpSourceFile); /* �����͸� �д´� */
				z.avail_in = INBUFSIZE;
			}
			status = inflate(&z, Z_NO_FLUSH); /* ���� */
			if (status == Z_STREAM_END) break; /* �Ϸ� */
			if (status != Z_OK) {   /* ���� */
				fprintf(stderr, "inflate: %s\n", (z.msg) ?  z.msg : "???");
				exit(1);
			}
			if (z.avail_out == 0) { /* ��� ���۰� ���ϸ� */
				/* ������ �᳽�� */
				if (fwrite(outbuf, 1, OUTBUFSIZE, m_fpDestFile) != OUTBUFSIZE) {
					fprintf(stderr, "Write error\n");
					exit(1);
				}
				z.next_out = outbuf; /* ��� �����͸� �������� �ǵ����� */
				z.avail_out = OUTBUFSIZE; /* ��� ���� �ܷ��� �������� �ǵ����� */
			}
		}

		/* �������� ���س��� */
		if ((count = OUTBUFSIZE - z.avail_out) != 0) {
			if (fwrite(outbuf, 1, count, m_fpDestFile) != count) {
				fprintf(stderr, "Write error\n");
				exit(1);
			}
		}

		/* ��ó�� */
		if (inflateEnd(&z) != Z_OK) {
			fprintf(stderr, "inflateEnd: %s\n", (z.msg) ?  z.msg : "???");
			exit(1);
		}

		//
		fclose( m_fpDestFile );
		free( outbuf );
	}
	else
	{
		//������ �˷��ش�.
	}

	//���� ���丮�� ������.
	SetCurrentDirectory( strCurrentDir );
}

//@{ 2006/04/10 burumal
int CCompress::decompressMemory(const char* pstrCompressBlock, int iCompressBlockSize, char** ppDestMem)
{
	z_stream z;
	unsigned char* inbuf = NULL;
	unsigned char* outbuf = NULL;

	//char			dest_filename[256];
	//char			strCurrentDir[256];
	//int			iCurrentPos = 0;

	unsigned int count, status;
	unsigned long INBUFSIZE;
	unsigned long OUTBUFSIZE;

	//@{ 2006/04/11 burumal
	bool bUseOutputMemPool = false;
	//@}

	if ( !pstrCompressBlock || !ppDestMem )
		return 0;

	*ppDestMem = NULL;

	long lMemAllocSize = 0;

	//���� ���丮�� �����Ѵ�.
	//GetCurrentDirectory( 256, strCurrentDir );

	//���� ���丮�� ������ ���丮(psrtDestDir)�� �����Ѵ�.
	//SetCurrentDirectory( pstrDestDir );

	//���� ���� ����
	int				iDestPathSize;

	//sprintf( dest_filename, "%s\\%s", pstrDestDir, pstrDestFileName );

	//�ڿ����� jzp�� �����.
	//iDestPathSize = strlen(dest_filename);
	//memset( &dest_filename[iDestPathSize-4], 0, 3 );

	//���� �����Ұ��� ������ Read-only �Ӽ��� ���������� �𸣴� Normal�� ���� ó���Ѵ�.
	//SetFileAttributes( pstrDestFileName, FILE_ATTRIBUTE_NORMAL );

	//m_fpDestFile = OpenFileMakeDir( pstrDestFileName, "wb" );

	//if(m_fpDestFile != NULL)
	{
		//������ Ǯ�� ���� ȯ�� ����
		INBUFSIZE = iCompressBlockSize;
		OUTBUFSIZE = INBUFSIZE;
		inbuf = (unsigned char *) pstrCompressBlock;
		
		//@{ 2006/04/11 burumal
		//outbuf = (unsigned char *) malloc(OUTBUFSIZE);
		if ( m_pMemPool[1] )
		{
			if ( m_lMemPoolSize[1] < OUTBUFSIZE )
				AllocMemPool(OUTBUFSIZE, 1);

			if ( m_lMemPoolSize[1] >= OUTBUFSIZE )
			{
				outbuf = (unsigned char*) m_pMemPool[1];
				bUseOutputMemPool = true;
			}
		}
		else
		{
			outbuf = (unsigned char *) malloc(OUTBUFSIZE);
		}

		if ( outbuf == NULL )
			exit(1);
		//@}

		/* ��� �޸� �޴�����Ʈ�� ���̺귯���� �ñ�� */
		z.zalloc = Z_NULL;
		z.zfree = Z_NULL;
		z.opaque = Z_NULL;

		/* �ʱ�ȭ */
		z.next_in = Z_NULL;
		z.avail_in = 0;
		if (inflateInit(&z) != Z_OK) {
			fprintf(stderr, "inflateInit: %s\n", (z.msg) ?  z.msg : "???");
			exit(1);
		}

		z.next_out = outbuf;        /* ��� ������ */
		z.avail_out = OUTBUFSIZE;    /* ��� ���� �ܷ� */
		status = Z_OK;

		while (status != Z_STREAM_END) 
		{
			if (z.avail_in == 0) {  /* �Է� �ܷ��� ���ΰ� �Ǹ� */
				z.next_in = inbuf;  /* �Է� �����͸� �������� �ǵ����� */
				//            z.avail_in = fread(inbuf, 1, INBUFSIZE, m_fpSourceFile); /* �����͸� �д´� */
				z.avail_in = INBUFSIZE;
			}
			status = inflate(&z, Z_NO_FLUSH); /* ���� */
			if (status == Z_STREAM_END) break; /* �Ϸ� */
			if (status != Z_OK) {   /* ���� */
				fprintf(stderr, "inflate: %s\n", (z.msg) ?  z.msg : "???");
				exit(1);
			}
			if (z.avail_out == 0) { /* ��� ���۰� ���ϸ� */

				/* ������ �᳽�� */
				//if (fwrite(outbuf, 1, OUTBUFSIZE, m_fpDestFile) != OUTBUFSIZE) {
					//fprintf(stderr, "Write error\n");
					//exit(1);
				//}				

				if ( m_pMemPool[0] )
				{
					// memory pool�� ����ϴ� ���
					lMemAllocSize += OUTBUFSIZE;

					if ( lMemAllocSize > m_lMemPoolSize[0] )
					{
						RaiseMemPool(lMemAllocSize - m_lMemPoolSize[0]);
						ASSERT(m_lMemPoolSize[0] >= lMemAllocSize);
						if ( m_lMemPoolSize[0] < lMemAllocSize )
							exit(1);
					}
					
					*ppDestMem = m_pMemPool[0];
					memcpy(*ppDestMem + lMemAllocSize - OUTBUFSIZE, outbuf, OUTBUFSIZE);
				}
				else
				{
					exit(1);
				}

				z.next_out = outbuf; /* ��� �����͸� �������� �ǵ����� */
				z.avail_out = OUTBUFSIZE; /* ��� ���� �ܷ��� �������� �ǵ����� */
			}
		}

		/* �������� ���س��� */
		if ((count = OUTBUFSIZE - z.avail_out) != 0) {

			//if (fwrite(outbuf, 1, count, m_fpDestFile) != count) {
				//fprintf(stderr, "Write error\n");
				//exit(1);
			//}

			if ( m_pMemPool[0] )
			{
				// memory pool�� ����ϴ� ���
				lMemAllocSize += count;

				if ( lMemAllocSize > m_lMemPoolSize[0] )
				{
					RaiseMemPool(lMemAllocSize - m_lMemPoolSize[0]);
					ASSERT(m_lMemPoolSize[0] >= lMemAllocSize);
					if ( m_lMemPoolSize[0] < lMemAllocSize )
						exit(1);
				}

				*ppDestMem = m_pMemPool[0];
				memcpy(*ppDestMem + lMemAllocSize - count, outbuf, count);
			}
			else
			{
				exit(1);
			}
		}

		/* ��ó�� */
		if (inflateEnd(&z) != Z_OK) {
			fprintf(stderr, "inflateEnd: %s\n", (z.msg) ?  z.msg : "???");
			exit(1);
		}

		//
		//fclose( m_fpDestFile );

		//@{ 2006/04/11 burumal
		//free( outbuf );
		if ( bUseOutputMemPool == false )
			free(outbuf);
		//@}
	}
	//else
	{
		//������ �˷��ش�.
	}

	//���� ���丮�� ������.
	//SetCurrentDirectory( strCurrentDir );

	if ( lMemAllocSize > 0 && (*ppDestMem != NULL) )
		return lMemAllocSize;

	return 0;
}

