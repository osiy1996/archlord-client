
#include <AgpmAuctionCategory/AgpmAuctionCategory.h>
#include <ApMemoryTracker/ApMemoryTracker.h>
#include <AuExcelTxtLib/AuExcelBinaryLib.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgpmItem/AgpmItem.h>

AgpmAuctionCategory::AgpmAuctionCategory()
{
	SetModuleName( "AgpmAuctionCategory" );
	m_cAdminCategory1.InitializeObject( sizeof(AgpdAuctionCategory1Info *), AGPA_AUCTION_CATEGORYCOUNT );
	m_cAdminCategory2.InitializeObject( sizeof(AgpdAuctionCategory2Info *), AGPA_AUCTION_CATEGORYCOUNT );

	m_lCurrentCategory1 = 1;
	m_lCurrentCategory2 = 1;
}

AgpmAuctionCategory::~AgpmAuctionCategory()
{
	AgpdAuctionCategory1Info	**ppcsCategory1Info;
	AgpdAuctionCategory2Info	**ppcsCategory2Info;

	INT32				lIndex;

	lIndex = 0; 

	for( ppcsCategory1Info = (AgpdAuctionCategory1Info **)m_cAdminCategory1.GetObjectSequence( &lIndex ); ppcsCategory1Info; ppcsCategory1Info = (AgpdAuctionCategory1Info **)m_cAdminCategory1.GetObjectSequence( &lIndex ) )
	{
		if( (*ppcsCategory1Info) )
		{
			delete (*ppcsCategory1Info);
		}
	}

	lIndex = 0; 

	for( ppcsCategory2Info = (AgpdAuctionCategory2Info **)m_cAdminCategory2.GetObjectSequence( &lIndex ); ppcsCategory2Info; ppcsCategory2Info = (AgpdAuctionCategory2Info **)m_cAdminCategory2.GetObjectSequence( &lIndex ) )
	{
		if( (*ppcsCategory2Info) )
		{
			delete (*ppcsCategory2Info);
		}
	}
}

BOOL AgpmAuctionCategory::OnAddModule()
{
	m_pcsAgpmItem = (AgpmItem *)GetModule( "AgpmItem" );

	if( !m_pcsAgpmItem )
		return FALSE;

	return TRUE;
}

ApAdmin	*AgpmAuctionCategory::GetCategory1()
{
	return 	&m_cAdminCategory1;
}

ApAdmin	*AgpmAuctionCategory::GetCategory2()
{
	return &m_cAdminCategory2;
}

bool AgpmAuctionCategory::LoadCategoryInfo( char *pstrFileName, bool bEncrypt )
{
	bool				bResult;

	bResult = false;

	using namespace AuExcel;

	AuExcelLib * pExcel = LoadExcelFile( pstrFileName , bEncrypt );
	AuAutoPtr< AuExcelLib >	ptrExcel = pExcel;

	if( pExcel == NULL)
	{
		TRACE("AgpmAuctionCategory::LoadCategoryInfo() Error (1) !!!\n");
		return FALSE;
	}

	{
		AgpdItemTemplate	*pcsItemTemplate;

		INT32				lColumn, lRow;
		INT32				lMaxColumn, lMaxRow;

		INT32				lTID;
		char				*pstrItemName;
		bool				bStackable;
		INT32				lFirstCategoryID, lSecondCategoryID;
		char				*pstrFirstCategoryName, *pstrSecondCategoryName;

		char				*pstrData;

		lMaxColumn = pExcel->GetColumn();
		lMaxRow = pExcel->GetRow();

		for( lRow=1; lRow<lMaxRow; lRow++ )
		{
			lTID = 0;
			pstrItemName = NULL;
			bStackable = false;
			lFirstCategoryID = 0;
			lSecondCategoryID = 0;
			pstrFirstCategoryName = NULL;
			pstrSecondCategoryName = NULL;

			for( lColumn=0; lColumn<lMaxColumn; lColumn++ )
			{
				pstrData = pExcel->GetData( lColumn, 0 );

				if( pstrData )
				{
					if( _stricmp( pstrData, "TID" ) == 0 )
					{
						pstrData = pExcel->GetData( lColumn, lRow );
						
						if( pstrData )
						{
							lTID = atoi(pstrData);
						}
					}
					else if(_stricmp( pstrData, "ItemName" ) == 0 )
					{
						pstrData = pExcel->GetData( lColumn, lRow );
						
						if( pstrData )
						{
							pstrItemName = pstrData;							
						}
						else
						{
							pstrItemName = NULL;
						}
					}
					else if(_stricmp( pstrData, "Stack" ) == 0 )
					{
					}
					else if(_stricmp( pstrData, "FirstCategory" ) == 0 )
					{
						pstrData = pExcel->GetData( lColumn, lRow );
						
						if( pstrData )
						{
							 lFirstCategoryID = atoi(pstrData);
						}
					}
					else if(_stricmp( pstrData, "FirstCategoryName" ) == 0 )
					{
						pstrData = pExcel->GetData( lColumn, lRow );
						
						if( pstrData )
						{
							 pstrFirstCategoryName = pstrData;
						}
					}
					else if(_stricmp( pstrData, "SecondCategory" ) == 0 )
					{
						pstrData = pExcel->GetData( lColumn, lRow );
						
						if( pstrData )
						{
							 lSecondCategoryID = atoi(pstrData);
						}
					}
					else if(_stricmp( pstrData, "SecondCategoryName" ) == 0 )
					{
						pstrData = pExcel->GetData( lColumn, lRow );
						
						if( pstrData )
						{
							 pstrSecondCategoryName = pstrData;
						}
					}
				}
			}

			//�������~ TID, ItemNameȮ��~
			if( (lTID != 0) && (pstrItemName != NULL) && (lFirstCategoryID != 0) && (lSecondCategoryID != 0) && (pstrFirstCategoryName != NULL) && (pstrSecondCategoryName != NULL) )
			{
				pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate( lTID );

				if( pcsItemTemplate )
				{
					if( strcmp( pcsItemTemplate->m_szName, pstrItemName ) == 0 )
					{
						AgpdAuctionCategory1Info	**ppcsAgpdAuctionCategory1Info;
						AgpdAuctionCategory2Info	**ppcsAgpdAuctionCategory2Info;

						//Categoey1 �˻�
						ppcsAgpdAuctionCategory1Info = (AgpdAuctionCategory1Info **)m_cAdminCategory1.GetObject( lFirstCategoryID );

						//�̹� �ִ°�?
						if( ppcsAgpdAuctionCategory1Info != NULL && (*ppcsAgpdAuctionCategory1Info) != NULL )
						{
							if( !strcmp( (*ppcsAgpdAuctionCategory1Info)->m_strName, pstrFirstCategoryName ) )
							{
								//�����Ŷ�� �������.
							}
							else
							{
								//ID�� ������ �̸��� �ٸ����? �̰Ŵ� Error��!
							}
						}
						else
						{
							AgpdAuctionCategory1Info		*pcsAgpdAuctionCategory1Info;

							pcsAgpdAuctionCategory1Info = new AgpdAuctionCategory1Info;

							if( pcsAgpdAuctionCategory1Info )
							{
								pcsAgpdAuctionCategory1Info->m_lCategoryID = lFirstCategoryID;
								strcat( pcsAgpdAuctionCategory1Info->m_strName, pstrFirstCategoryName );

								ASSERT(strlen(pcsAgpdAuctionCategory1Info->m_strName) < 80);

								m_cAdminCategory1.AddObject( (PVOID *)&pcsAgpdAuctionCategory1Info, lFirstCategoryID );
							}
						}

						//Category2 �˻�
						ppcsAgpdAuctionCategory2Info = (AgpdAuctionCategory2Info **)m_cAdminCategory2.GetObject( lSecondCategoryID );

						//�̹� �ִ°�?
						if( ppcsAgpdAuctionCategory2Info != NULL && (*ppcsAgpdAuctionCategory2Info) != NULL )
						{
							if( !strcmp( (*ppcsAgpdAuctionCategory2Info)->m_strName, pstrSecondCategoryName ) )
							{
								//�����Ŷ�� �������.
							}
							else
							{
								//ID�� ������ �̸��� �ٸ����? �̰Ŵ� Error��!
							}
						}
						else
						{
							AgpdAuctionCategory2Info		*pcsAgpdAuctionCategory2Info;

							pcsAgpdAuctionCategory2Info = new AgpdAuctionCategory2Info;

							if( pcsAgpdAuctionCategory2Info )
							{
								pcsAgpdAuctionCategory2Info->m_lParentCategoryID = lFirstCategoryID;
								pcsAgpdAuctionCategory2Info->m_lCategoryID = lSecondCategoryID;
								strcat( pcsAgpdAuctionCategory2Info->m_strName, pstrSecondCategoryName );

								ASSERT(strlen(pcsAgpdAuctionCategory2Info->m_strName) < 80);

								m_cAdminCategory2.AddObject( (PVOID *)&pcsAgpdAuctionCategory2Info, lSecondCategoryID );
							}
						}
					}
				}
			}
		}

		bResult = true;

		BuildCategoryTree();

		pExcel->CloseFile();
	}

	return bResult;
}


bool AgpmAuctionCategory::BuildCategoryTree()
{
	bool				bResult;

	bResult = FALSE;

	if( m_pcsAgpmItem )
	{
		AgpdAuctionCategory1Info	**ppcsAgpdAuctionCategory1Info;
		AgpdAuctionCategory2Info	**ppcsAgpdAuctionCategory2Info;

		INT32				lCategory1Index;
		INT32				lCategory2Index;

		//���� ������ ����.
		lCategory2Index = 0;
		//2�� ī�װ��� �˻��ؼ� 1�� ī�װ��� Child List�� ������ �˾Ƴ���.
		for( ppcsAgpdAuctionCategory2Info = (AgpdAuctionCategory2Info **)m_cAdminCategory2.GetObjectSequence( &lCategory2Index ) ;
			 ppcsAgpdAuctionCategory2Info;
			 ppcsAgpdAuctionCategory2Info = (AgpdAuctionCategory2Info **)m_cAdminCategory2.GetObjectSequence( &lCategory2Index ) )
		{
			ppcsAgpdAuctionCategory1Info = (AgpdAuctionCategory1Info **)m_cAdminCategory1.GetObject( (*ppcsAgpdAuctionCategory2Info)->m_lParentCategoryID );

			if( ppcsAgpdAuctionCategory1Info )
			{
				(*ppcsAgpdAuctionCategory1Info)->m_ChildCount++;
			}
		}

		//Catogory1�� �ش��ϴ� �༮���� �޸𸮸� ����ش�.
		lCategory1Index = 0;
		for( ppcsAgpdAuctionCategory1Info = (AgpdAuctionCategory1Info **)m_cAdminCategory1.GetObjectSequence( &lCategory1Index ) ;
			 ppcsAgpdAuctionCategory1Info;
			 ppcsAgpdAuctionCategory1Info = (AgpdAuctionCategory1Info **)m_cAdminCategory1.GetObjectSequence( &lCategory1Index ) )
		{
			if( (*ppcsAgpdAuctionCategory1Info)->m_ChildCount != 0 )
			{
				(*ppcsAgpdAuctionCategory1Info)->m_plChildID = new INT32[(*ppcsAgpdAuctionCategory1Info)->m_ChildCount];
				memset( (*ppcsAgpdAuctionCategory1Info)->m_plChildID, 0, sizeof(INT32)*(*ppcsAgpdAuctionCategory1Info)->m_ChildCount );
			}
		}

		//1�� ī�װ��� ���� TID�� �־��ش�.
//		lCategory1Index = 0;
		lCategory2Index = 0;
		//2�� ī�װ��� �˻��ؼ� 1�� ī�װ��� Child List�� ä���ִ´�.
		for( ppcsAgpdAuctionCategory2Info = (AgpdAuctionCategory2Info **)m_cAdminCategory2.GetObjectSequence( &lCategory2Index ) ;
			 ppcsAgpdAuctionCategory2Info;
			 ppcsAgpdAuctionCategory2Info = (AgpdAuctionCategory2Info **)m_cAdminCategory2.GetObjectSequence( &lCategory2Index ) )
		{
			ppcsAgpdAuctionCategory1Info = (AgpdAuctionCategory1Info **)m_cAdminCategory1.GetObject( (*ppcsAgpdAuctionCategory2Info)->m_lParentCategoryID );

			if( ppcsAgpdAuctionCategory1Info )
			{
				INT32				lEmptySlot;

				for( lEmptySlot=0; lEmptySlot<(*ppcsAgpdAuctionCategory1Info)->m_ChildCount; lEmptySlot++ )
				{
					if( (*ppcsAgpdAuctionCategory1Info)->m_plChildID[lEmptySlot] == 0 )
						break;
				}

				(*ppcsAgpdAuctionCategory1Info)->m_plChildID[lEmptySlot] = (*ppcsAgpdAuctionCategory2Info)->m_lCategoryID;
			}
		}

		//�������� �˻��ؼ� 1�� ī�װ��� Child List�� ����.
		for( AgpaItemTemplate::iterator it = m_pcsAgpmItem->csTemplateAdmin.begin(); it != m_pcsAgpmItem->csTemplateAdmin.end(); ++it)
		{
			ppcsAgpdAuctionCategory2Info = (AgpdAuctionCategory2Info **)m_cAdminCategory2.GetObject( (it->second)->m_lSecondCategory );

			if( ppcsAgpdAuctionCategory2Info )
			{
				(*ppcsAgpdAuctionCategory2Info)->m_ChildCount++;
			}
		}

		//Category2�� �ش��ϴ� �༮���� �޸𸮸� ����ش�.
		lCategory2Index = 0;
		for( ppcsAgpdAuctionCategory2Info = (AgpdAuctionCategory2Info **)m_cAdminCategory2.GetObjectSequence( &lCategory2Index ) ;
			 ppcsAgpdAuctionCategory2Info;
			 ppcsAgpdAuctionCategory2Info = (AgpdAuctionCategory2Info **)m_cAdminCategory2.GetObjectSequence( &lCategory2Index ) )
		{
			if( (*ppcsAgpdAuctionCategory2Info)->m_ChildCount != 0 )
			{
				(*ppcsAgpdAuctionCategory2Info)->m_plChildID = new INT32[(*ppcsAgpdAuctionCategory2Info)->m_ChildCount];
				memset( (*ppcsAgpdAuctionCategory2Info)->m_plChildID, 0, sizeof(INT32)*(*ppcsAgpdAuctionCategory2Info)->m_ChildCount );
			}
		}

		//2�� ī�װ��� ���� TID�� �־��ش�.
		for( AgpaItemTemplate::iterator it = m_pcsAgpmItem->csTemplateAdmin.begin(); it != m_pcsAgpmItem->csTemplateAdmin.end(); ++it)
		{
			ppcsAgpdAuctionCategory2Info = (AgpdAuctionCategory2Info **)m_cAdminCategory2.GetObject( (it->second)->m_lSecondCategory );

			if( ppcsAgpdAuctionCategory2Info )
			{
				INT32				lEmptySlot;

				for( lEmptySlot=0; lEmptySlot<(*ppcsAgpdAuctionCategory2Info)->m_ChildCount; lEmptySlot++ )
				{
					if( (*ppcsAgpdAuctionCategory2Info)->m_plChildID[lEmptySlot] == 0 )
						break;
				}

				(*ppcsAgpdAuctionCategory2Info)->m_plChildID[lEmptySlot] = (it->second)->m_lID;
			}
		}
	}

	return bResult;
}
