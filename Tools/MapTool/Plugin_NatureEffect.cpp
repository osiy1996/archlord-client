// Plugin_NatureEffect.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MapTool.h"
#include "Plugin_NatureEffect.h"
#include "AgcmNatureEffect.h"
#include "NatureEffectDlg.h"
#include "ApModuleStream.h"

extern	AgcmNatureEffect			*g_pcsAgcmNatureEffect;

// CPlugin_NatureEffect

IMPLEMENT_DYNAMIC(CPlugin_NatureEffect, CWnd)
CPlugin_NatureEffect::CPlugin_NatureEffect()
{
	m_iCurSelectNEffectID = -1;

	m_strShortName = "NEffect";
}

CPlugin_NatureEffect::~CPlugin_NatureEffect()
{
}


BEGIN_MESSAGE_MAP(CPlugin_NatureEffect, CWnd)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_NOTIFY(TVN_SELCHANGED, IDC_PLUGIN_NEFFECT_TREE, OnSelChangedTree)
END_MESSAGE_MAP()

BOOL CPlugin_NatureEffect::OnIdle		( UINT32 ulClockCount )
{

	return	TRUE;
}

// CPlugin_NatureEffect �޽��� ó�����Դϴ�.

int CPlugin_NatureEffect::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CUITileList_PluginBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.

	CRect	rect;
	rect.SetRect(10,10,260,350);
	m_TreeNEffect.Create(WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS ,rect,this,IDC_PLUGIN_NEFFECT_TREE);

	ApModuleStream	csStream;
	const CHAR		*szValueName = NULL;
	CHAR			szValue[256];

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// szFile�� �д´�.
	csStream.Open(".\\Ini\\NatureEffect.ini");

	csStream.ReadSectionName(1);
	csStream.SetValueID(-1);
	INT32			lID;
	char			str[50];

	TV_INSERTSTRUCT		tvstruct;
	HTREEITEM			parent = 0;
	HTREEITEM			item = 0;

	while(csStream.ReadNextValue())
	{
		szValueName = csStream.GetValueName();

		if(!strcmp(szValueName, AGCMNATUREEFFECT_INI_NAME_NEFFECT_ID))
		{
			csStream.GetValue(szValue, 256);
			sscanf(szValue, "%d", &lID ); 
			sprintf(str,"%d",lID);

			// TreeList�� �׸� �߰�
			tvstruct.hParent= parent;
			tvstruct.hInsertAfter = TVI_LAST;
			tvstruct.item.pszText = str;
			tvstruct.item.mask = TVIF_TEXT;
			
			item = m_TreeNEffect.InsertItem(&tvstruct);
			
			m_TreeNEffect.SetItemData(item,lID);		
		}
	}

	rect.SetRect(10,370,250,390);
	m_buttonNEffectSet.Create("NEffect Set",WS_VISIBLE | BS_PUSHBUTTON,rect,this,IDC_PLUGIN_NEFFECT_SET_BUTTON);

	rect.SetRect(10,400,100,420);
	m_buttonStart.Create("Start",WS_VISIBLE | BS_PUSHBUTTON,rect,this,IDC_PLUGIN_START);

	rect.SetRect(110,400,200,420);
	m_buttonEnd.Create("Stop",WS_VISIBLE | BS_PUSHBUTTON,rect,this,IDC_PLUGIN_STOP);
	
	return 0;
}

void CPlugin_NatureEffect::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CUITileList_PluginBase::OnLButtonDown(nFlags, point);
}

void CPlugin_NatureEffect::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CUITileList_PluginBase::OnPaint()��(��) ȣ������ ���ʽÿ�.

	CString	str;
	dc.SetBkMode( TRANSPARENT );
	dc.SetTextColor( RGB( 255 , 255 , 255 ) );

	if(g_pcsAgcmNatureEffect && m_iCurSelectNEffectID != -1)
	{
		NatureEffectInfo	* pstNEInfo = &g_pcsAgcmNatureEffect->m_vectorstNEInfo[ m_iCurSelectNEffectID ];
		RwTexture			* pTexture	= g_pcsAgcmNatureEffect->m_vectorTexture[ pstNEInfo->texID ];

		str.Format("ID : %d",m_iCurSelectNEffectID);
		dc.TextOut( 0 , 400	, str );

		switch( pstNEInfo->eType )
		{
		case NATURE_EFFECT_SNOW					:	dc.TextOut( 0 , 420	, "Type : Snow"				);	break;
		case NATURE_EFFECT_RAIN					:	dc.TextOut( 0 , 420	, "Type : Rain"				);	break;
		case NATURE_EFFECT_RAIN_WITH_THUNDER	:	dc.TextOut( 0 , 420	, "Type : RainWithThunder"	);	break;
		case NATURE_EFFECT_RAIN_WITH_FOG		:	dc.TextOut( 0 , 420	, "Type : RainWithFog"		);	break;
		case NATURE_EFFECT_SANDSTORM			:	dc.TextOut( 0 , 420	, "Type : SandStorm"		);	break;
		}

		if(pTexture)
		{
			char*		tex_name = RwTextureGetName(pTexture);
			str.Format("Texture : %s",tex_name);
			dc.TextOut( 0 , 440	, str );
		}
		else
		{
			str.Format("Texture : none");
			dc.TextOut( 0 , 440	, str );
		}

		str.Format("Size Min : %f , Max : %f",pstNEInfo->fSizeMin
            ,pstNEInfo->fSizeMax);
		dc.TextOut( 0 , 460	, str );

		str.Format("Speed Min : %f , Max : %f",pstNEInfo->fSpeedMin
            ,pstNEInfo->fSpeedMax);
		dc.TextOut( 0 , 480	, str );

		str.Format("Swing Min : %f , Max : %f",pstNEInfo->fSwingMin
            ,pstNEInfo->fSwingMax);
		dc.TextOut( 0 , 500	, str );

		str.Format("Density Min : %f , Max : %f",pstNEInfo->fDensityMin
            ,pstNEInfo->fDensityMax);
		dc.TextOut( 0 , 520	, str );

		str.Format("Red Min : %d , Max : %d",pstNEInfo->cRedMin
            ,pstNEInfo->cRedMax);
		dc.TextOut( 0 , 540	, str );

		str.Format("Green Min : %d , Max : %d",pstNEInfo->cGreenMin
            ,pstNEInfo->cGreenMax);
		dc.TextOut( 0 , 560	, str );

		str.Format("Blue Min : %d , Max : %d",pstNEInfo->cBlueMin
            ,pstNEInfo->cBlueMax);
		dc.TextOut( 0 , 580	, str );

		str.Format("Alpha Min : %d , Max : %d",pstNEInfo->cAlphaMin
            ,pstNEInfo->cAlphaMax);
		dc.TextOut( 0 , 600	, str );
	}
}

void	CPlugin_NatureEffect::OnSelChangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	m_iCurSelectNEffectID = m_TreeNEffect.GetItemData(pNMTreeView->itemNew.hItem);
	
	//AfxGetMainWnd()->InvalidateRect(NULL);

	GetParent()->InvalidateRect(NULL);
}

LRESULT CPlugin_NatureEffect::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	#define	CW_CLICKED_BUTTON( uID )	if( message == WM_COMMAND && uID == LOWORD( wParam ) && BN_CLICKED == HIWORD( wParam ) )

	CW_CLICKED_BUTTON( IDC_PLUGIN_NEFFECT_SET_BUTTON )
	{
		ChangeNEffectSetVal();
	}
	CW_CLICKED_BUTTON( IDC_PLUGIN_START )
	{
		if( m_iCurSelectNEffectID != -1 && g_pcsAgcmNatureEffect )
		{
			g_pcsAgcmNatureEffect->StopNatureEffect( TRUE );
			g_pcsAgcmNatureEffect->StartNatureEffect( m_iCurSelectNEffectID , TRUE );
			g_pcsAgcmEventNature->ApplySkySetting();
		}
	}
	CW_CLICKED_BUTTON( IDC_PLUGIN_STOP )
	{
		if( g_pcsAgcmNatureEffect )
		{
			g_pcsAgcmNatureEffect->StopNatureEffect( TRUE );
			g_pcsAgcmEventNature->ApplySkySetting();
		}
	}

	return CUITileList_PluginBase::WindowProc(message, wParam, lParam);
}

void	CPlugin_NatureEffect::ChangeNEffectSetVal()
{
	if(m_iCurSelectNEffectID == -1 || !g_pcsAgcmNatureEffect)	return;

	NatureEffectDlg		dlg( m_iCurSelectNEffectID );
	dlg.m_bDataChange = FALSE;

	int					status_id = m_iCurSelectNEffectID;
	NatureEffectInfo*	pInfo = &g_pcsAgcmNatureEffect->m_vectorstNEInfo[ status_id ];

	// ����ص�.
	NatureEffectInfo	stInfoBackup = *pInfo;

	dlg.m_ID = status_id;
	dlg.m_iSelType = pInfo->eType;

	RwTexture*	pTexture = g_pcsAgcmNatureEffect->m_vectorTexture[pInfo->texID];
	if(pTexture)
		dlg.m_strTextureFile = RwTextureGetName(pTexture);
	else
		dlg.m_strTextureFile = "none";

	dlg.m_fSizeMin		= pInfo->fSizeMin;
	dlg.m_fSizeMax		= pInfo->fSizeMax;
	dlg.m_fSpeedMin		= pInfo->fSpeedMin;
	dlg.m_fSpeedMax		= pInfo->fSpeedMax;
	dlg.m_fVibMin		= pInfo->fSwingMin;
	dlg.m_fVibMax		= pInfo->fSwingMax;
	dlg.m_fDensityMin	= pInfo->fDensityMin;
	dlg.m_fDensityMax	= pInfo->fDensityMax;

	dlg.m_iRedMin		= pInfo->cRedMin;
	dlg.m_iRedMax		= pInfo->cRedMax;
	dlg.m_iGreenMin		= pInfo->cGreenMin;
	dlg.m_iGreenMax		= pInfo->cGreenMax;
	dlg.m_iBlueMin		= pInfo->cBlueMin;
	dlg.m_iBlueMax		= pInfo->cBlueMax;
	dlg.m_iAlphaMin		= pInfo->cAlphaMin;
	dlg.m_iAlphaMax		= pInfo->cAlphaMax;
	dlg.m_nSkySet		= pInfo->nSkySet ;

	// 
	
	if(IDOK == dlg.DoModal())
	{
		if(!dlg.m_bDataChange)	return;

		int		neffect_TID = -1;
		bool	bfind = false;

		bfind  = false;
		// Texture ���� �˻�����
		for(int i = 0;i< ( int ) g_pcsAgcmNatureEffect->m_vectorstNEInfo.size() ;++i)
		{
			RwTexture*	pTexture = g_pcsAgcmNatureEffect->m_vectorTexture[ g_pcsAgcmNatureEffect->m_vectorstNEInfo[i].texID ];
			CString	temp_str;
			if(pTexture)
				temp_str = RwTextureGetName(pTexture);

			if(!strcmp(temp_str,dlg.m_strTextureFile))
			{
				neffect_TID = i;
				bfind = true;
				break;
			}
		}

		if(bfind == false)
		{
			// �󽽷� �� ã�ƺ���..
			BOOL	bFindNone = FALSE;
			for(int j=0;j< ( int ) g_pcsAgcmNatureEffect->m_vectorTexture.size();++j)
			{
				if(!g_pcsAgcmNatureEffect->m_vectorTexture[j])
				{
					neffect_TID = j;
					bFindNone = TRUE;

					// texture �ٽ� �ε�
					//if(g_pcsAgcmNatureEffect->m_pTexture[j]) RwTextureDestroy(g_pcsAgcmNatureEffect->m_pTexture[j]);

					g_pcsAgcmNatureEffect->m_vectorTexture[j] = RwTextureRead(dlg.m_strTextureFile, NULL);
					ASSERT( NULL != g_pcsAgcmNatureEffect->m_vectorTexture[j] && "�ؽ��İ� �����ϴ�!" );
					if( g_pcsAgcmNatureEffect->m_vectorTexture[j] )
					{
						RwTextureSetFilterMode(g_pcsAgcmNatureEffect->m_vectorTexture[j], rwFILTERLINEAR);
					}

					break;
				}
			}

			if(!bFindNone)
			{
				BOOL	bFindInStatus = FALSE;

				for(int j=0;j< ( int ) g_pcsAgcmNatureEffect->m_vectorstNEInfo.size();++j)
				{
					if(status_id != j)
					{
						if(pInfo->texID == g_pcsAgcmNatureEffect->m_vectorstNEInfo[j].texID)
						{
							bFindInStatus = TRUE;
							break;
						}
					}
				}

				RwTexture * pTexture = RwTextureRead(dlg.m_strTextureFile, NULL);

				if(bFindInStatus)		// ���� ���� 
				{
					// �ٸ��� ���°� �ִ°��ڿ��� �߰��ϰ�..
					if(g_pcsAgcmNatureEffect->m_vectorTexture.size() == NATURE_EFFECT_TEXTURE_NUM)
					{
						MessageBox("Texture���ϼ��� �ʰ��Ͽ����ϴ�"," ���",MB_OK);
						return;
					}
					neffect_TID = g_pcsAgcmNatureEffect->m_vectorTexture.size() + 1;
					g_pcsAgcmNatureEffect->m_vectorTexture.push_back( pTexture );
				}
				else
				{
					// �ٸ��� ���°� ���� ��� �ö� �ؽ��ĸ� �����Ѵ�.
					neffect_TID = pInfo->texID;
					if(g_pcsAgcmNatureEffect->m_vectorTexture[neffect_TID]) RwTextureDestroy(g_pcsAgcmNatureEffect->m_vectorTexture[neffect_TID]);
					g_pcsAgcmNatureEffect->m_vectorTexture[neffect_TID] = pTexture;
				}

				// texture �ٽ� �ε�
				ASSERT( NULL != g_pcsAgcmNatureEffect->m_vectorTexture[neffect_TID] && "�ؽ��İ� �����ϴ�!" );
				RwTextureSetFilterMode(g_pcsAgcmNatureEffect->m_vectorTexture[neffect_TID], rwFILTERLINEAR);
			}
		}

		pInfo->eType		= (enumNatureEffectType)dlg.m_iSelType;
		pInfo->texID		= neffect_TID;
		pInfo->fSizeMin		= dlg.m_fSizeMin;
		pInfo->fSizeMax		= dlg.m_fSizeMax;
		pInfo->fSpeedMin	= dlg.m_fSpeedMin;
		pInfo->fSpeedMax	= dlg.m_fSpeedMax;
		pInfo->fSwingMin	= dlg.m_fVibMin;
		pInfo->fSwingMax	= dlg.m_fVibMax;
		pInfo->fDensityMin	= dlg.m_fDensityMin;
		pInfo->fDensityMax	= dlg.m_fDensityMax;
		pInfo->cRedMin		= dlg.m_iRedMin;
		pInfo->cRedMax		= dlg.m_iRedMax;
		pInfo->cGreenMin	= dlg.m_iGreenMin;
		pInfo->cGreenMax	= dlg.m_iGreenMax;
		pInfo->cBlueMin		= dlg.m_iBlueMin;
		pInfo->cBlueMax		= dlg.m_iBlueMax;
		pInfo->cAlphaMin	= dlg.m_iAlphaMin;
		pInfo->cAlphaMax	= dlg.m_iAlphaMax;

		pInfo->nSkySet		= dlg.m_nSkySet;

		SetSaveData();
	}
	else
	{
		// ���� ������ �ٲ�.
		*pInfo = stInfoBackup;
	}

	// ����Ʈ �÷���
	g_pcsAgcmNatureEffect->StopNatureEffect( TRUE );
	g_pcsAgcmNatureEffect->StartNatureEffect( m_iCurSelectNEffectID , TRUE );
	g_pcsAgcmEventNature->ApplySkySetting();

	GetParent()->InvalidateRect(NULL);
}

BOOL CPlugin_NatureEffect::OnQuerySaveData		( char * pStr )
{
	strcpy( pStr , _T("������ ����Ʈ ���ø�") );
	return  TRUE;
}

BOOL CPlugin_NatureEffect::OnSaveData				()
{
	// ���ø� ���̺�..
	return g_pcsAgcmNatureEffect->SaveNatureEffectInfoToINI( ".\\Ini\\NatureEffect.ini" , FALSE );
}

BOOL CPlugin_NatureEffect::OnLoadData				()
{
	// �ϰ͵� �� ����..
	return TRUE;
}
