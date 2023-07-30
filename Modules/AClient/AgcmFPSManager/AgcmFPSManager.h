#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class AgcmFPSManager
{
public:
	static void AdjustFPS( bool foreground );
	static void SetMultisampleLevel( int level ) { Inst().multisample_level_ = level; Inst().adj_multisample_level_ = level; }

private:
	AgcmFPSManager();

	static AgcmFPSManager & Inst() {
		static AgcmFPSManager inst;
		return inst;
	}

	void CheckAvgFPS(int frameTime);
	void AdjustMultisampleLevel();

	BOOL bUseHighRes;
	LARGE_INTEGER stFreq;
	LARGE_INTEGER stPrevCounter;
	DWORD ulPrevTickCount;

	float avg_fps_;		// 평큔 FPS
	float frame_count_;	// 누적 프레임 카운트
	int multisample_level_;	// 옵션에서 지정한 멀티샘플 레벨
	int adj_multisample_level_; // FPS에 따라 조절된 멀팀샘플 레벨 
};