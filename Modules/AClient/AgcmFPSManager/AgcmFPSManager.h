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

	float avg_fps_;		// ��Ŧ FPS
	float frame_count_;	// ���� ������ ī��Ʈ
	int multisample_level_;	// �ɼǿ��� ������ ��Ƽ���� ����
	int adj_multisample_level_; // FPS�� ���� ������ �������� ���� 
};