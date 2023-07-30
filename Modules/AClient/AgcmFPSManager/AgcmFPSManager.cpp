#include "AgcmFPSManager.h"
#include <AgcmAdmin/AgcmAdmin.h>

//-----------------------------------------------------------------------
//

AgcmFPSManager::AgcmFPSManager()
	: bUseHighRes(TRUE)
	, ulPrevTickCount(0)
	, avg_fps_(0)
	, frame_count_(0)
	, multisample_level_(0)
	, adj_multisample_level_(0)
{
	if (QueryPerformanceFrequency(&stFreq))
		stPrevCounter.HighPart = 0;
	else
		bUseHighRes = FALSE;
}

void AgcmFPSManager::AdjustFPS( bool foreground )
{
	AgcmFPSManager & inst = Inst();
	DWORD frameTime;

	if (inst.bUseHighRes) {
		LARGE_INTEGER ct;
		LARGE_INTEGER elapsedTime;

		QueryPerformanceCounter(&ct);
		if (!inst.stPrevCounter.QuadPart) {
			/* 
			 * First frame, no need to adjust.
			 */
			inst.stPrevCounter = ct;
			return;
		}
		/*
		 * No time has passed, skip.
		 */
		if (ct.QuadPart <= inst.stPrevCounter.QuadPart)
			return;
		elapsedTime.QuadPart = ct.QuadPart - inst.stPrevCounter.QuadPart;
		elapsedTime.QuadPart *= 1000000;
		elapsedTime.QuadPart /= inst.stFreq.QuadPart;
		inst.stPrevCounter = ct;
		frameTime = (DWORD)(elapsedTime.QuadPart / 1000000);
		if (!foreground) {
			Sleep(50);
		}
		else {
			LONGLONG toSleep = ((1000000 / 111) - elapsedTime.QuadPart);

			if (toSleep > 0) {
				LONGLONG remainder = toSleep - (toSleep / 1000) * 1000;

				if (toSleep > 1000)
					Sleep((DWORD)(toSleep / 1000));
				inst.stPrevCounter.QuadPart -= (remainder * inst.stFreq.QuadPart) / 1000000;
			}
		}
	}
	else {
		DWORD ct = GetTickCount();

		if (!inst.ulPrevTickCount) {
			/*
			 * First frame, no need to adjust.
			 */
			inst.ulPrevTickCount = ct;
			return;
		}
		/*
		 * No time has passed, skip.
		 */
		if (ct <= inst.ulPrevTickCount)
			return;
		frameTime = ct - inst.ulPrevTickCount;
		if (!foreground) {
			Sleep(50);
		}
		else {
			const DWORD ft = (1000 / 111);

			if (frameTime < ft)
				Sleep(ft - frameTime);
		}
	}
	inst.CheckAvgFPS((int)frameTime);
}

//-----------------------------------------------------------------------
//

void AgcmFPSManager::CheckAvgFPS(int frameTime)
{
	frame_count_ += 1.0f;

	static int frame_check_time_ = 0;
	frame_check_time_ += frameTime;

	if( frame_check_time_ > 10000 ) // 10초마다 평균 프레임 갱신
	{
		avg_fps_ = frame_count_ / (frame_check_time_ * 0.001f);

		frame_check_time_ = 0;
		frame_count_ = 0;

		AdjustMultisampleLevel();
	}
}

//-----------------------------------------------------------------------
//

void AgcmFPSManager::AdjustMultisampleLevel()
{
	static int prev_multisample_level = 1;

	if( multisample_level_ > 1 )
	{
		if( avg_fps_ < 50 )
		{
			/*
			if( adj_multisample_level_ > 2 )
			{
				adj_multisample_level_ -= 4;

				if( adj_multisample_level_ < 2 )
					adj_multisample_level_ = 2;

				if( prev_multisample_level != adj_multisample_level_ )
				{
					RwD3D9ChangeMultiSamplingLevels( adj_multisample_level_ );

					prev_multisample_level = adj_multisample_level_;
				}
			}
			*/

			/*
			if( adj_multisample_level_ != 2 )
			{
				adj_multisample_level_ = 2;
				RwD3D9ChangeMultiSamplingLevels( adj_multisample_level_ );
			}
			*/
		}
		/*
		else if( avg_fps_ > 60 )
		{
			if( adj_multisample_level_ < multisample_level_ )
			{
				++adj_multisample_level_;

				if( prev_multisample_level != adj_multisample_level_ )
				{
					RwD3D9ChangeMultiSamplingLevels( adj_multisample_level_ );
					prev_multisample_level = adj_multisample_level_;
				}
			}
		}
		*/
	}
}

//-----------------------------------------------------------------------