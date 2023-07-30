#include "stdafx.h"
#include "Plugin_PathEditor.h"
#include <algorithm>
#include <string>
#include <strstream>

static DWORD colorTable [54] = { 
  0xff003300		, 0xff006666		, 0xffc00ccc		
, 0xff006600		, 0xff006666		, 0xff0066cc	
, 0xff00cc00		, 0xff00cc66		, 0xff00cccc	

, 0xff330000		, 0xff330066		, 0xff3300cc	
, 0xff336600		, 0xff336666		, 0xff3366cc	
, 0xff33cc00		, 0xff33cc66		, 0xff33cccc	

, 0xff660000		, 0xff660066		, 0xff6600cc	
, 0xff666600		, 0xff666666		, 0xff6666cc	
, 0xff66cc00		, 0xff66cc66		, 0xff66cccc	

, 0xff990000		, 0xff990066		, 0xff9900cc	
, 0xff996600		, 0xff996666		, 0xff9966cc	
, 0xff99cc00		, 0xff99cc66		, 0xff99cccc	

, 0xffcc0000		, 0xffcc0066		, 0xffcc00cc	
, 0xffcc6600		, 0xffcc6666		, 0xffcc66cc	
, 0xffcccc00		, 0xffcccc66		, 0xffcccccc	

, 0xffff0000		, 0xffff0066		, 0xffff00cc	
, 0xffff6600		, 0xffff6666		, 0xffff66cc	
, 0xffffcc00		, 0xffffcc66		, 0xffffcccc	};

static void shffule_colorTable()
{
	std::random_shuffle( colorTable, colorTable+54 );
}

CPlugin_PathEditor::CPlugin_PathEditor( VOID )
{
	m_strShortName		=	"PathEditor";
	selectedPoint_		=	0;
	selectedPath_		=	0;

	editBackGround_.CreateSolidBrush( RGB( 0, 0, 0 ) );

	shffule_colorTable();

	modified_ = false;
}

CPlugin_PathEditor::~CPlugin_PathEditor( VOID )
{
	editBackGround_.DeleteObject();

	if( modified_ )
		saveData();
}

BOOL	CPlugin_PathEditor::OnSelectedPlugin( VOID )
{
	return TRUE;
}

BOOL	CPlugin_PathEditor::OnDeSelectedPlugin( VOID )
{
	return TRUE;
}

void	CPlugin_PathEditor::drawPathLine( Agpm::Path * path )
{
	if( path )
	{
		typedef Path::Points Points;
		typedef Point::Neighbor Neighbor;

		Points drawed_points;
		Points const & points = path->getPoints();

		RwV3d start, end;
		float offsetY = 500.0f;

		for( Points::const_iterator iter = points.begin(); iter != points.end(); ++iter )
		{
			Point * point = *iter;
			if( point ) 
			{
				bool startDrawed = false;
				if( !drawed_points.count( point ) )
				{
					startDrawed = true;
					drawed_points.insert( point );
				}

				start.x = point->x;
				start.z = point->y;
				start.y = AGCMMAP_THIS->GetHeight_Lowlevel( start.x, start.z, SECTOR_MAX_HEIGHT) + offsetY;

				for( Neighbor::iterator nIter = point->neighbor.begin(); nIter != point->neighbor.end(); ++nIter )
				{
					Point * neighbor = *nIter;

					if( neighbor )
					{
						if( startDrawed && drawed_points.count( neighbor ) )
							continue;

						drawed_points.insert( neighbor );

						end.x = neighbor->x;
						end.z = neighbor->y;
						end.y = AGCMMAP_THIS->GetHeight_Lowlevel( end.x, end.z, SECTOR_MAX_HEIGHT) + offsetY;

						size_t colorIdx = path->getIndex();

						while( colorIdx > 53 )
						{
							colorIdx -= 53;
						}

						g_csAcuIMDraw.DrawLine( &start, &end, colorTable[colorIdx] );
					}
				}
			}
		}
	}
}

Agpd::PathPoint::eTag CPlugin_PathEditor::getSelectedFrom()
{
	int curSel = fromComboBox_.GetCurSel();

	if( curSel > -1 && curSel < Point::None )
	{
		return (Point::eTag)(curSel);
	}

	return Point::Normal;
}

Agpd::PathPoint::eTag CPlugin_PathEditor::getSelectedTo()
{
	int curSel = toComboBox_.GetCurSel();

	if( curSel > -1 && curSel < Point::None )
	{
		return (Point::eTag)(curSel);
	}

	return Point::Normal;
}

BOOL	CPlugin_PathEditor::OnWindowRender( VOID )
{
	static bool first = true;

	if( first )
	{
		refreshViewPaths();

		CDC * dc = GetDC();

		first = false;
	}

	for( Agpm::PathManager::Paths::iterator iter = viewPaths_.begin(); iter != viewPaths_.end(); ++iter )
		drawPathLine( *iter );

	if( selectedPoint_ )
	{
		AuPOS	pos;

		pos.x	=	selectedPoint_->x;
		pos.y	=	0;
		pos.z	=	selectedPoint_->y;

		pos.y = AGCMMAP_THIS->GetHeight_Lowlevel( pos.x, pos.z, SECTOR_MAX_HEIGHT);

		/*
		RpClump*		pClump	=	g_pcsAgcmObject->LoadClump( "Mon_Tiger.rws");
		RwFrameTranslate(RpClumpGetFrame(pClump), (RwV3d *)(&pos), rwCOMBINEREPLACE);
		g_pcsAgcmRender->AddClumpToWorld( pClump );
		*/

		g_MainWindow.DrawAreaSphere( pos.x, pos.y, pos.z, 700.0f );
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//

void CPlugin_PathEditor::refreshInfo()
{
	refreshPathInfo();
	refreshPointInfo();
}

//////////////////////////////////////////////////////////////////////////////
//

template< typename Control, typename ArgType >
static void setControlText( Control & control, std::string text, ArgType arg )
{
	char buf[1024] = {0,};

	std::strstream stream(buf, sizeof(buf));

	stream << text << arg;

	control.SetWindowText( stream.str() );

	control.SetReadOnly( TRUE );
}

//////////////////////////////////////////////////////////////////////////////
//

void CPlugin_PathEditor::setSelectedPath( Agpm::Path * path )
{
	selectedPath_ = path;

	refreshPathInfo();
}

void CPlugin_PathEditor::setSelectedPoint( Agpd::PathPoint * point )
{
	selectedPoint_ = point;

	refreshPointInfo();
}

void CPlugin_PathEditor::refreshPathInfo()
{
	size_t index = 0;
	Point::eTag from = Point::Normal;
	Point::eTag to = Point::Normal;

	if( selectedPath_ )
	{
		index = selectedPath_->getIndex();

		from = selectedPath_->from();

		to = selectedPath_->to();
	}

	setControlText( pathIndexInfo_, "index : ", index );

	setControlText( fromInfo_, "from : ", from );

	setControlText( toInfo_, "to : ", to );

	InvalidateRect( 0 );

	OnPaint();
}

void CPlugin_PathEditor::refreshPointInfo()
{
	float x = 0;
	float y = 0;
	size_t index = 0;
	Point::eTag tag = Point::None;

	if( selectedPoint_ )
	{
		x = selectedPoint_->x;
		y = selectedPoint_->y;
		tag = selectedPoint_->tag;
		index = selectedPoint_->index;
	}

	setControlText( xInfo_, "x : ", x );

	setControlText( yInfo_, "y : ", y );

	setControlText( pointIndexInfo_, "index : ", index );

	tagComboBox_.SetCurSel( tag );

	InvalidateRect( 0 );

	OnPaint();
}

//////////////////////////////////////////////////////////////////////////////
//

BOOL	CPlugin_PathEditor::OnLButtonDownGeometry( RwV3d * pPos )
{
	if( !pPos )
		return FALSE;

	bool select = checkPickClump( pPos );

	if( !select )
	{
		if( selectedPath_ )
		{
			setSelectedPoint( selectedPath_->insert( pPos->x, pPos->z, Point::Normal, selectedPoint_ ) );

			if( selectedPoint_ )
			{
				/*
				각인 장소인지 체크할 수 있으면 테그를 자동으로 넣을 수 있을듯.
				근데 어떻게 확인하는지 모르겠당.

				ApmMap::RegionTemplate* regionData = AGCMMAP_THIS->GetTemplate(
					AGCMMAP_THIS->GetRegion( selectedPoint_->x, selectedPoint_->y ));

				if(regionData && regionData->ti.stType.bJail)
					return TRUE; 

				*/

				_AddClump( selectedPoint_->x, selectedPoint_->y, selectedPath_->getIndex(), selectedPoint_->index );
				modified_ = true;
				return TRUE;
			}
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
//

BOOL	CPlugin_PathEditor::OnRButtonDownGeometry( RwV3d * pPos )
{
	if( !pPos )
		return FALSE;

	Path * oldPath = selectedPath_;
	Point * oldPoint = selectedPoint_;

	bool select = checkPickClump( pPos );

	if( select )
	{
		if( selectedPath_ && selectedPath_ == oldPath )
		{
			if( selectedPoint_ && selectedPoint_ != oldPoint )
			{
				selectedPath_->link( selectedPoint_, oldPoint );
				modified_ = true;
			}
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
//

BOOL	CPlugin_PathEditor::OnLButtonUpGeometry( RwV3d * pPos )
{
	return TRUE;
}

BOOL	CPlugin_PathEditor::OnMouseMoveGeometry( RwV3d * pPos )
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//

BEGIN_MESSAGE_MAP(CPlugin_PathEditor, CUITileList_PluginBase)
	ON_WM_PAINT()
	ON_WM_CREATE()
END_MESSAGE_MAP()

VOID	CPlugin_PathEditor::OnPaint( VOID )
{
	CPaintDC dc(this);

	dc.SetTextColor( RGB( 255 , 255 , 255 ) );
	//dc.SetBkMode( TRANSPARENT );
	dc.SetBkColor( RGB( 99, 99, 99 ) );

	struct printInfoLable : std::unary_function<CEdit*, void> {
		printInfoLable( CPlugin_PathEditor * inst, CPaintDC * dc ) : inst(inst), dc(dc) {}
		void operator()(CEdit* lable) const {
			if( lable )
			{
				CString str;
				lable->GetWindowText(str);
				dc->TextOut( inst->Pos(*lable).left, inst->Pos(*lable).top, str );
			}
		}
		CPlugin_PathEditor * inst;
		CPaintDC * dc;
	};

	for_each( infoLables_.begin(), infoLables_.end(), printInfoLable(this,&dc) );

	/*
	INT			nCurrentSel	=	m_comboSpawnName.GetCurSel();
	CString		strData;

	if( nCurrentSel != -1 )
	{
		m_comboSpawnName.GetLBText( nCurrentSel , strData );
		if( !m_MonsterPath.GetMonsterPath( strData.GetString() ) )
			dc.TextOut( 50 , 300 , _T("경로를 새로 만드세요") );
	}
	*/

}

INT CPlugin_PathEditor::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CUITileList_PluginBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	//m_SpawnDataLoad.Init( ".//ini//spawnExcelData.txt" );
	//m_pEventSpawn		=	g_pcsAgpmEventSpawn;

	pointTypes_[0] = "None";
	pointTypes_[1] = "Orc";
	pointTypes_[2] = "MoonAlf";
	pointTypes_[3] = "Human";
	pointTypes_[4] = "DragonSion";

	int offset_y = 10;
	int offset_x = 5;
	int tap_size = 15;

	UINT btnAttr = WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON;
	UINT lblAttr = WS_CHILD;
	UINT cbxAttr = CBS_DROPDOWNLIST | WS_VISIBLE | WS_CHILD | WS_VSCROLL;


	CreateControl( loadButton_, "불러오기", offset_x, offset_y, 150 - offset_x, 25, btnAttr );

	CreateControl( clearButton_, "초기화", Pos(loadButton_).right + offset_x, offset_y, 150 - offset_x, 25, btnAttr );

	offset_y += 35;

	CreateControl( saveButton_, "저장", offset_x, offset_y, 300 - offset_x*2, 25, btnAttr );

	offset_y += 35;

	CreateControl( fromLable_, "from : ", offset_x, offset_y + 4, 35, 16, lblAttr );

	CreateControl( fromComboBox_, Pos(fromLable_).right + 4, offset_y, 65, 16 + 16 * (int)pointTypes_.size(), cbxAttr | WS_DISABLED );

	for( int i=0; i < (int)pointTypes_.size(); ++i )
		fromComboBox_.AddString( pointTypes_[i].c_str() );
	fromComboBox_.SetCurSel(0);


	CreateControl( toLabel_, "to : ", Pos(fromComboBox_).right + 6, offset_y + 4, 35, 16, lblAttr );

	CreateControl( toComboBox_, Pos(toLabel_).right + 6, offset_y, 65, 16 + 16 * (int)pointTypes_.size(), cbxAttr | WS_DISABLED );

	for( int i=0; i < (int)pointTypes_.size(); ++i )
		toComboBox_.AddString( pointTypes_[i].c_str() );
	toComboBox_.SetCurSel(0);

	
	CreateControl( newPathButton_, "생성", 305 - 70 - offset_x, offset_y + 3, 70 - offset_x, 20, btnAttr );


	offset_y += 35;

	CreateControl( pathLabel_, "선택한 경로", offset_x, offset_y, 200, 16, lblAttr );


	offset_y += 30;

	CreateControl( pathIndexInfo_, "index : ", offset_x + tap_size, offset_y, 80, 16, lblAttr );


	offset_y += 20;

	CreateControl( fromInfo_, "from : ", offset_x + tap_size, offset_y, 170, 16, lblAttr );

	offset_y += 20;

	CreateControl( toInfo_, "to : ", offset_x + tap_size, offset_y, 170, 16, lblAttr );


	offset_y += 30;

	CreateControl( pointLabel_, "선택한 포인트", offset_x, offset_y, 200, 16, lblAttr );


	offset_y += 30;

	CreateControl( pointIndexInfo_, "index : ", offset_x + tap_size, offset_y, 100, 16, lblAttr );

	offset_y += 22;

	CreateControl( tagInfo_, "tag : ", offset_x + tap_size, offset_y+4, 60, 16, lblAttr );

	CreateControl( tagComboBox_, Pos(tagInfo_).right + 7, offset_y, 65, 16 + 16 * (int)pointTypes_.size(), cbxAttr );

	for( int i=0; i < (int)pointTypes_.size(); ++i )
		tagComboBox_.AddString( pointTypes_[i].c_str() );
	tagComboBox_.SetCurSel(0);


	offset_y += 26;

	CreateControl( xInfo_, "x : ", offset_x + tap_size, offset_y, 170, 16, lblAttr );

	offset_y += 20;

	CreateControl( yInfo_, "y : ", offset_x + tap_size, offset_y, 170, 16, lblAttr );

	
	offset_y += 20;

	CreateControl( cameraMoveButton_, "카메라 이동", offset_x, offset_y, 300 - offset_x * 2, 25, btnAttr );


	offset_y += 20;


	// 처음에 한번 로드
	loadData( false );

	return 0;
}

LRESULT CPlugin_PathEditor::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
	case WM_COMMAND:

		{
			int param = LOWORD(wParam);

			if( param == low_16(&newPathButton_) ) // 패스 생성 버튼 클릭
			{
				createNewPath();
			}
			else if( param == low_16(&fromComboBox_) || param == low_16(&toComboBox_) ) // 테그 콤보박스 클릭
			{
				if( HIWORD(wParam) == CBN_SELCHANGE )
					refreshViewPaths();
			}
			else if( param == low_16( &saveButton_ ) ) // 세이브 버튼 클릭
			{
				saveData();
			}
			else if( param == low_16( &loadButton_ ) ) // 로드 버튼 클릭
			{
				loadData();
			}
			else if( param == low_16( &cameraMoveButton_ ) ) // 카메라 이동 클릭
			{
				moveCamera();
			}
			else if( param == low_16( &tagComboBox_ ) ) // 테그 콤보박스 클릭
			{
				if( HIWORD(wParam) == CBN_SELCHANGE )
					setPointTag();
			}
			else if( param == low_16( &clearButton_ ) )
			{
				clearData();
			}
		}

		break;
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////////
//

void CPlugin_PathEditor::setPointTag()
{
	if( selectedPoint_ )
	{
		int sel = tagComboBox_.GetCurSel();

		if( sel > -1 )
		{
			selectedPoint_->tag = (Point::eTag)sel;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//

void CPlugin_PathEditor::createNewPath()
{
	int fromSel = fromComboBox_.GetCurSel();

	if( fromSel > 0 )
	{
		int toSel = toComboBox_.GetCurSel();

		if( toSel > 0 && toSel != fromSel )
		{
			setSelectedPoint( 0 );
			setSelectedPath( pathManager_.get( Owner::BattleGround, -1 ) );
			selectedPath_->from( (Point::eTag)fromSel );
			selectedPath_->to( (Point::eTag)toSel );
			viewPaths_.push_back( selectedPath_ );
		}
		else
		{
			MessageBox( "to 타입을 선택해주세요", "", 0 );
		}
	}
	else
	{
		MessageBox( "from 타입을 선택해주세요", "", 0 );
	}
}

//////////////////////////////////////////////////////////////////////////////
//

void CPlugin_PathEditor::refreshViewPaths()
{
	_ClearClumpList();

	viewPaths_.clear();

	// 테그를 맵툴에서 지정하지 않는다
	/*
	if( getSelectedFrom() == Point::Normal && getSelectedTo() == Point::Normal )
	{
		for( int i=0; i<Point::None; ++i )
		{
			for( int j=0; j<Point::None; ++j )
				pathManager_.find( viewPaths_, Agcm::PathOwner::BattleGround, (Point::eTag)i, (Point::eTag)j );
		}
	}
	else
	{
		pathManager_.find( viewPaths_, Agcm::PathOwner::BattleGround, getSelectedFrom(), getSelectedTo() );
	}
	그냥 전부 보이도록 */
	for( int i=0; i<Point::None; ++i )
	{
		for( int j=0; j<Point::None; ++j )
			pathManager_.find( viewPaths_, Agpm::PathOwner::BattleGround, (Point::eTag)i, (Point::eTag)j );
	}

	for( PathManager::Paths::iterator iter = viewPaths_.begin(); iter != viewPaths_.end(); ++iter )
	{
		Path * path = *iter;

		if( path )
		{
			Path::Points const & points = path->getPoints();

			for( Path::Points::const_iterator pIter = points.begin(); pIter != points.end(); ++pIter )
			{
				Point * point = *pIter;

				if( point )
				{
					_AddClump( point->x, point->y, path->getIndex(), point->index );
				}
			}
		}
	}
	

	setSelectedPath( 0 );
	setSelectedPoint( 0 );
}

//////////////////////////////////////////////////////////////////////////////
//

BOOL CPlugin_PathEditor::Window_OnKeyDown( RsKeyStatus *ks )
{
	switch( ks->keyCharCode )
	{
	case rsDEL:
		{
			if( selectedPath_ && selectedPoint_ )
			{
				_DelClump( selectedPath_->getIndex(), selectedPoint_->index );

				selectedPath_->del( selectedPoint_ );

				setSelectedPoint( 0 );
				
				modified_ = true;
			}
		}
		break;
	default:
		break;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//

bool CPlugin_PathEditor::checkPickClump( RwV3d * pos )
{
	if( !pos )
		return false;

	// 피킹된 클럼프가 있는지 확인한다.
	//RpClump * pClump = AGCMMAP_THIS->GetCursorClump( g_pEngine->m_pCamera , pos , AgcmMap::PICKINGOBJECTCLUMP );

	Point pickPoint( pos->x, pos->z, 0 );


	typedef PathManager::Paths Paths;

	Point * point = 0;

	float dist = 400.0f;

	dist *= dist;

	size_t pathIndex = -1;
	size_t pointIndex = -1;

	// 클릭 지점과 가장 가까운 포인트 검색
	for( Paths::iterator iter = viewPaths_.begin(); iter != viewPaths_.end(); ++iter )
	{
		Path * path = *iter;

		if( path )
		{
			Point * point2 = path->find( pickPoint.x, pickPoint.y );

			if( !point2 )
				continue;

			float dist2 = point2->dist( pickPoint );

			if( dist2 < dist )
			{
				point = point2;
				dist = dist2;

				pathIndex = path->getIndex();
				pointIndex = point->index;
			}
		}
	}

	// 패스 인덱스와 포인트 인덱스로 클럼프를 검색
	ClumpData clump(0,0,0);
	
	if( ( pointIndex != ((size_t)-1) ) && ( pathIndex != (size_t(-1)) ) )
		clump = findClump( pathIndex, pointIndex );


	if( clump.clump )
		return selectPickClump( clump );

	return false;
}

//////////////////////////////////////////////////////////////////////////////
//

CPlugin_PathEditor::ClumpData CPlugin_PathEditor::findClump( size_t pathIndex, size_t pointIndex )
{
	ClumpData clump(0,0,0);

	for( PathClumpListIter iter = clumpList_.begin(); iter != clumpList_.end(); ++iter )
	{
		ClumpData data = *iter;

		if( data.pathIdx == pathIndex && data.pointIdx == pointIndex )
		{
			clump = data;
			break;
		}
	}

	return clump;
}

//////////////////////////////////////////////////////////////////////////////
//

bool CPlugin_PathEditor::selectPickClump( ClumpData clumpData )
{
	bool result = false;

	Path * path = pathManager_.get( Agpm::PathOwner::BattleGround, clumpData.pathIdx );

	if( path ) 
	{
		setSelectedPath( path );
		setSelectedPoint( selectedPath_->find( clumpData.pointIdx ) );
		result = true;
	}

	return result;
}

BOOL CPlugin_PathEditor::Window_OnLButtonDown( RsMouseStatus *ms	)
{
	return FALSE;
}


VOID	CPlugin_PathEditor::_ClearClumpList( VOID )
{
	PathClumpListIter	Iter		=	clumpList_.begin();

	for( ; Iter != clumpList_.end() ; ++Iter )
	{

		_DelClump( (*Iter) );
	}

	clumpList_.clear();
}

BOOL	CPlugin_PathEditor::_AddClump( float x , float z, size_t pathIdx, size_t pointIdx )
{
	AuPOS	pos;

	pos.x	=	x;
	pos.y	=	0;
	pos.z	=	z;

	pos.y = AGCMMAP_THIS->GetHeight_Lowlevel( x, z , SECTOR_MAX_HEIGHT);

	RpClump*		pClump	=	g_pcsAgcmObject->LoadClump( "Poing_Light.rws");
	RwFrameTranslate(RpClumpGetFrame(pClump), (RwV3d *)(&pos), rwCOMBINEREPLACE);
	g_pcsAgcmRender->AddClumpToWorld( pClump );

	ClumpData clump( pClump, pathIdx, pointIdx );

	clumpList_.push_back( clump );

	return TRUE;
}

BOOL	CPlugin_PathEditor::_DelClump( CPlugin_PathEditor::ClumpData clump )
{
	if( clump.clump )
	{
		g_pcsAgcmRender->RemoveClumpFromWorld( clump.clump );
		return TRUE;
	}
	return FALSE;
}

void CPlugin_PathEditor::_DelClump( size_t pathIndex, size_t pointIndex )
{
	RpClump * clump = 0;

	for( PathClumpListIter iter = clumpList_.begin(); iter != clumpList_.end(); ++iter )
	{
		ClumpData data = *iter;

		if( data.pathIdx == pathIndex && data.pointIdx == pointIndex )
		{
			if( data.clump )
				_DelClump( data );

			clumpList_.erase( iter );
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//

VOID	CPlugin_PathEditor::saveData( bool question )
{
	int yes = true;

	if( question )
		yes = AfxMessageBox( "패스 데이터를 저장할까요?", MB_YESNO );

	if( yes )
	{
		pathManager_.save( ".\\ini\\PathData.xml" );
		modified_ = false;
	}
}

//////////////////////////////////////////////////////////////////////////////
//

void CPlugin_PathEditor::loadData( bool question )
{
	int yes = true;
	
	if( question )
		yes = AfxMessageBox( "저장하지 않은 데이터가 손실됩니다.\n데이터를 로드할까요?", MB_YESNO );
	
	if( yes )
	{
		//setSelectedPath( 0 );
		//setSelectedPoint( 0 );
		clearData(false);
		pathManager_.load( ".\\ini\\PathData.xml" );
		refreshViewPaths();
	}
}

//////////////////////////////////////////////////////////////////////////////
//

void CPlugin_PathEditor::clearData( bool question )
{
	int yes = true;

	if( question )
		yes = AfxMessageBox( "저장하지 않은 데이터가 손실됩니다.\n데이터를 초기화 할까요?", MB_YESNO );

	if( yes )
	{
		setSelectedPath( 0 );
		setSelectedPoint( 0 );
		pathManager_.clear();
		fromComboBox_.SetCurSel(0);
		toComboBox_.SetCurSel(0);
		refreshViewPaths();
		shffule_colorTable();
	}
}

//////////////////////////////////////////////////////////////////////////////
//

static void CameraMove(RwCamera *camera, RwV3d *newPos)
{
	RwFrame *cameraFrame;
	RwMatrix *cameraMatrix;
	RwV3d *at, *up, *right, *pos;

	cameraFrame = RwCameraGetFrame(camera);
	cameraMatrix = RwFrameGetMatrix(cameraFrame);

	right = RwMatrixGetRight(cameraMatrix);
	up = RwMatrixGetUp(cameraMatrix);
	at = RwMatrixGetAt(cameraMatrix);
	pos = RwMatrixGetPos(cameraMatrix);

	pos->x = newPos->x;
	pos->y = newPos->y;
	pos->z = newPos->z;

	float dummy1 = 0.00000001f;
	float dummy2 = 0.001f;

	right->x = -1.f;
	right->y = dummy1;
	right->z = -dummy2;

	up->x = dummy1;
	up->y = dummy1;
	up->z = -1.f;

	at->x = dummy1;
	at->y = -1.f;
	at->z = -dummy2;

	return;
}

//////////////////////////////////////////////////////////////////////////////
//

VOID	CPlugin_PathEditor::moveCamera( VOID )
{
	RwCamera * camera = g_pEngine->m_pCamera;

	if( !camera )
		return;	

	if( !selectedPath_ || selectedPath_->getPoints().empty() )
		return;

	RwFrame		*cameraFrame	=	NULL;
	RwMatrix	*cameraMatrix	=	NULL;
	RwV3d		*cameraPos		=	NULL;

	cameraFrame		= RwCameraGetFrame(camera);
	cameraMatrix	= RwFrameGetMatrix(cameraFrame);
	cameraPos		= RwMatrixGetPos(cameraMatrix);

	if( !selectedPoint_ )
	{
		setSelectedPoint( const_cast< Point * >(*selectedPath_->getPoints().begin()) );
	}

	RwV3d cameraEye;

	cameraEye.x = selectedPoint_->x;
	cameraEye.z = selectedPoint_->y;
	cameraEye.y = cameraPos->y;

	CameraMove( camera, &cameraEye );

	return;
}

HBRUSH CPlugin_PathEditor::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CUITileList_PluginBase::OnCtlColor(pDC, pWnd, nCtlColor);

	if(nCtlColor == CTLCOLOR_EDIT)
	{
		pDC->SetBkMode(TRANSPARENT);
		//return editBackGround_;
		return CreateSolidBrush( RGB(128,128,128) );
	}

	return hbr;
}

//////////////////////////////////////////////////////////////////////////////
//

unsigned int CPlugin_PathEditor::low_16 ( void * p ) 
{
	__int64 n = (__int64)(p);

	/*
	n %= ( 1l << 16 );
	if ( n >= ( 1l << 15 ) ) {
	int i = n % ( 1l << 15 );
	return ( - i );
	} 
	*/

	unsigned int n1 = (unsigned int)( ( n & 0xFFFFF00000000000 ) >> 44 );
	unsigned int n2 = (unsigned int)( ( n & 0x00000FFFFF000000 ) >> 24 );
	unsigned int n3 = (unsigned int)( ( n & 0x0000000000FFFFFF ) );

	return (n1 ^ n2 ^ n3) & 0x0000FFFF;
}

//////////////////////////////////////////////////////////////////////////////
//

void CPlugin_PathEditor::CreateControl( CButton & control, TCHAR * text, int x, int y, int width, int height, DWORD attr )
{
	CRect rect;
	rect.SetRect( x, y, x + width, y + height );
	control.Create( text, attr, rect, this, low_16(&control) );
}

void CPlugin_PathEditor::CreateControl( CEdit & control, TCHAR * text, int x, int y, int width, int height, DWORD attr )
{
	CreateControl( control, x, y, width, height, attr );
	control.SetWindowText( text );
	control.SetReadOnly( TRUE );

	infoLables_.insert( &control );

	/*
	CDC * dc = control.GetDC();

	HBRUSH oldBrush = (HBRUSH)SendMessage( WM_CTLCOLOR, (WPARAM)dc->m_hDC, (LPARAM)m_hWnd );

	SelectObject( dc->m_hDC, CreateSolidBrush( RGB( 0,0,0 ) ) );

	DeleteObject( oldBrush );

	dc->SetBkMode(TRANSPARENT);

	dc->SetDCBrushColor( 0x00000000 );

	dc->SetTextColor( 0xffffffff );

	control.ReleaseDC( dc );
	*/
}

//////////////////////////////////////////////////////////////////////////////