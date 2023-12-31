#ifndef	__AGPMITEMCONVERTSTREAM_H__
#define	__AGPMITEMCONVERTSTREAM_H__

#define	AGPMITEMCONVERT_TABLE_PHYSICAL						"기본강화단계"
//#define	AGPMITEMCONVERT_TABLE_PHYSICAL_FAIL					"실패처리"
//#define	AGPMITEMCONVERT_TABLE_ADD_SOCKET					"계약의 고리 추가개수"
//#define	AGPMITEMCONVERT_TABLE_ADD_SOCKET_FAIL				"계약의 고리 실패처리"
#define	AGPMITEMCONVERT_TABLE_SPIRITSTONE					"정령석단계"
#define	AGPMITEMCONVERT_TABLE_BONUS							"정령석같은속성추가시"
#define	AGPMITEMCONVERT_TABLE_RUNE							"기원석 추가장착 판별"
#define	AGPMITEMCONVERT_TABLE_RUNE_FAIL						"기원석 추가 단계"

#define	AGPMITEMCONVERT_TABLE_PHYSICAL_SUCCESS				"성공"
#define	AGPMITEMCONVERT_TABLE_PHYSICAL_FAIL_NONE			"그대로남음"
#define	AGPMITEMCONVERT_TABLE_PHYSICAL_FAIL_INIT			"기본강화만 초기화"
#define	AGPMITEMCONVERT_TABLE_PHYSICAL_FAIL_DESTROY			"파괴"

//	AGPMITEMCONVERT_TABLE_PHYSICAL
///////////////////////////////////////////////
#define	AGPMITEMCONVERT_COLUMN_PHYSICAL_WEAPON_ADD			"무기능력치 상승"
//#define	AGPMITEMCONVERT_COLUMN_PHYSICAL_WEAPON_PROB			"무기 개별확률"
#define	AGPMITEMCONVERT_COLUMN_PHYSICAL_ARMOUR_ADD			"방어구능력치 상승"
//#define	AGPMITEMCONVERT_COLUMN_PHYSICAL_ARMOUR_PROB			"방어구 개별확률"
#define	AGPMITEMCONVERT_COLUMN_PHYSICAL_RANK				"Rank"
#define	AGPMITEMCONVERT_COLUMN_PHYSICAL_SPIRIT_1			"Spirit1"
#define	AGPMITEMCONVERT_COLUMN_PHYSICAL_SPIRIT_2			"Spirit2"
#define	AGPMITEMCONVERT_COLUMN_PHYSICAL_SPIRIT_3			"Spirit3"
#define	AGPMITEMCONVERT_COLUMN_PHYSICAL_SPIRIT_4			"Spirit4"
#define	AGPMITEMCONVERT_COLUMN_PHYSICAL_SPIRIT_5			"Spirit5"

/*
//	AGPMITEMCONVERT_TABLE_PHYSICAL_FAIL
///////////////////////////////////////////////
#define	AGPMITEMCONVERT_COLUMN_PHYSICAL_FAIL_NONE			"그대로남음"
#define	AGPMITEMCONVERT_COLUMN_PHYSICAL_FAIL_INIT_SAME		"기본강화만 초기화"
#define	AGPMITEMCONVERT_COLUMN_PHYSICAL_FAIL_INIT			"초기화"
#define	AGPMITEMCONVERT_COLUMN_PHYSICAL_FAIL_DESTROY		"아이템파괴"
*/

/*
//	AGPMITEMCONVERT_TABLE_ADD_SOCKET
///////////////////////////////////////////////
#define	AGPMITEMCONVERT_COLUMN_ADD_SOCKET_WEAPON			"무기 확률"
#define	AGPMITEMCONVERT_COLUMN_ADD_SOCKET_WEAPON_COST		"비용(무기)"
#define	AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ARMOUR			"갑옷 확률"
#define	AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ARMOUR_COST		"비용(갑옷)"
#define	AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ETC				"기타 확률"
#define	AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ETC_COST			"비용(기타)"

//	AGPMITEMCONVERT_TABLE_ADD_SOCKET_FAIL
#define	AGPMITEMCONVERT_COLUMN_ADD_SOCKET_WEAPON_FAIL_NONE			"그대로남음(무기)"
#define	AGPMITEMCONVERT_COLUMN_ADD_SOCKET_WEAPON_FAIL_INIT_SAME		"소켓 초기화(무기)"
#define	AGPMITEMCONVERT_COLUMN_ADD_SOCKET_WEAPON_FAIL_INIT			"초기화(무기)"
#define	AGPMITEMCONVERT_COLUMN_ADD_SOCKET_WEAPON_FAIL_DESTROY		"아이템파괴(무기)"
#define	AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ARMOUR_FAIL_NONE			"그대로남음(갑옷)"
#define	AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ARMOUR_FAIL_INIT_SAME		"소켓 초기화(갑옷)"
#define	AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ARMOUR_FAIL_INIT			"초기화(갑옷)"
#define	AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ARMOUR_FAIL_DESTROY		"아이템파괴(갑옷)"
#define	AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ETC_FAIL_NONE				"그대로남음(기타)"
#define	AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ETC_FAIL_INIT_SAME		"고리 초기화(기타)"
#define	AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ETC_FAIL_INIT				"초기화(기타)"
#define	AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ETC_FAIL_DESTROY			"아이템파괴(기타)"
*/

//	AGPMITEMCONVERT_TABLE_SPIRITSTONE
///////////////////////////////////////////////
#define	AGPMITEMCONVERT_COLUMN_SPIRITSTONE_WEAPON_ADD		"무기강화수치"
#define	AGPMITEMCONVERT_COLUMN_SPIRITSTONE_WEAPON_PROB		"무기확률"
#define	AGPMITEMCONVERT_COLUMN_SPIRITSTONE_ARMOUR_ADD		"갑옷강화수치(%)"
#define	AGPMITEMCONVERT_COLUMN_SPIRITSTONE_ARMOUR_PROB		"갑옷확률"

//	AGPMITEMCONVERT_TABLE_BONUS
///////////////////////////////////////////////
#define	AGPMITEMCONVERT_COLUMN_SPIRITSTONE_BONUS_WEAPON		"무기 보너스"
#define	AGPMITEMCONVERT_COLUMN_SPIRITSTONE_BONUS_ARMOUR		"갑옷 보너스(%)"

//	AGPMITEMCONVERT_TABLE_RUNE
///////////////////////////////////////////////
#define	AGPMITEMCONVERT_COLUMN_RUNE_WEAPON_PROB				"무기 확률"
#define	AGPMITEMCONVERT_COLUMN_RUNE_ARMOUR_PROB				"방어구 확률"
#define	AGPMITEMCONVERT_COLUMN_RUNE_ETC_PROB				"기타확률"

//	AGPMITEMCONVERT_TABLE_RUNE_FAIL
///////////////////////////////////////////////
#define	AGPMITEMCONVERT_COLUMN_RUNE_WEAPON_FAIL_NONE		"그대로남음(무기)"
#define	AGPMITEMCONVERT_COLUMN_RUNE_WEAPON_FAIL_INIT_SAME	"소켓 초기화(무기)"
#define	AGPMITEMCONVERT_COLUMN_RUNE_WEAPON_FAIL_INIT		"초기화(무기)"
#define	AGPMITEMCONVERT_COLUMN_RUNE_WEAPON_FAIL_DESTROY		"아이템파괴(무기)"
#define	AGPMITEMCONVERT_COLUMN_RUNE_ARMOUR_FAIL_NONE		"그대로남음(갑옷)"
#define	AGPMITEMCONVERT_COLUMN_RUNE_ARMOUR_FAIL_INIT_SAME	"소켓 초기화(갑옷)"
#define	AGPMITEMCONVERT_COLUMN_RUNE_ARMOUR_FAIL_INIT		"초기화(갑옷)"
#define	AGPMITEMCONVERT_COLUMN_RUNE_ARMOUR_FAIL_DESTROY		"아이템파괴(갑옷)"
#define	AGPMITEMCONVERT_COLUMN_RUNE_ETC_FAIL_NONE			"그대로남음(기타)"
#define	AGPMITEMCONVERT_COLUMN_RUNE_ETC_FAIL_INIT_SAME		"고리 초기화(기타)"
#define	AGPMITEMCONVERT_COLUMN_RUNE_ETC_FAIL_INIT			"초기화(기타)"
#define	AGPMITEMCONVERT_COLUMN_RUNE_ETC_FAIL_DESTROY		"아이템파괴(기타)"


//	rune item streaming
///////////////////////////////////////////////
#define	AGPMITEMCONVERT_COLUMN_RUNE_NAME					"기원석이름"
#define	AGPMITEMCONVERT_COLUMN_RUNE_TID						"TID"
#define	AGPMITEMCONVERT_COLUMN_RUNE_ANTI_TYPE_NUMBER		"ANTI Number"
#define	AGPMTTEMCONVERT_COLUMN_RUNE_ARMOUR_BODY				"갑옷상체"
#define	AGPMITEMCONVERT_COLUMN_RUNE_ARMOUR_LEGS				"갑옷하체"
#define	AGPMITEMCONVERT_COLUMN_RUNE_WEAPON					"무기"
#define	AGPMITEMCONVERT_COLUMN_RUNE_SHIELD					"방패"
#define	AGPMITEMCONVERT_COLUMN_RUNE_HEAD					"투구"
#define	AGPMITEMCONVERT_COLUMN_RUNE_RING					"반지"
#define	AGPMITEMCONVERT_COLUMN_RUNE_NECKLACE				"목걸이"
#define	AGPMITEMCONVERT_COLUMN_RUNE_ARMS					"팔찌"
#define	AGPMITEMCONVERT_COLUMN_RUNE_FOOT					"신발"
#define	AGPMITEMCONVERT_COLUMN_RUNE_HANDS					"장갑"
#define	AGPMITEMCONVERT_COLUMN_RUNE_PROBABILITY				"성공확률"
#define	AGPMITEMCONVERT_COLUMN_RUNE_RESTRICT_LEVEL			"장착제한레벨"
#define	AGPMITEMCONVERT_COLUMN_RUNE_DESCRIPTION				"설명(툴팁에 뜰내용)"
#define	AGPMITEMCONVERT_COLUMN_RUNE_SKILL_NAME				"스킬 이름"
#define	AGPMITEMCONVERT_COLUMN_RUNE_SKILL_LEVEL				"스킬 레벨"
#define	AGPMITEMCONVERT_COLUMN_RUNE_SKILL_PROBABILITY		"스킬 확률"


//	convert point streaming
///////////////////////////////////////////////
#define	AGPMITEMCONVERT_COLUMN_POINT_PHYSICAL				"물리강화단계"
#define	AGPMITEMCONVERT_COLUMN_POINT_PHYSICAL_POINT			"물리강화포인트"
#define	AGPMITEMCONVERT_COLUMN_POINT_SPIRITSTONE			"정령석단계"
#define	AGPMITEMCONVERT_COLUMN_POINT_SPIRITSTONE_POINT		"정령석 포인트"
#define	AGPMITEMCONVERT_COLUMN_POINT_RUNE					"기원석단계"
#define	AGPMITEMCONVERT_COLUMN_POINT_RUNE_POINT				"기원석 포인트"
#define	AGPMITEMCONVERT_COLUMN_POINT_SOCKET					"계약의고리개수"
#define	AGPMITEMCONVERT_COLUMN_POINT_SOCKET_POINT			"고리 포인트"

#endif	//__AGPMITEMCONVERTSTREAM_H__