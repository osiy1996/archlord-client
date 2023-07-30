#pragma once

class AgcmFPSManager
{
public:
	static void AdjustFPS( bool foreground );
	static void SetMultisampleLevel( int level ) { Inst().multisample_level_ = level; Inst().adj_multisample_level_ = level; }

private:
	AgcmFPSManager()
		: prev_tickcount_(0)
		, avg_fps_(0)
		, frame_count_(0)
		, multisample_level_(0)
		, adj_multisample_level_(0)
	{}

	static AgcmFPSManager & Inst() {
		static AgcmFPSManager inst;
		return inst;
	}

	void CheckAvgFPS(int frameTime);
	void AdjustMultisampleLevel();

	int prev_tickcount_; // ���� ������ ƽ ī��Ʈ

	float avg_fps_;		// ��Ŧ FPS
	float frame_count_;	// ���� ������ ī��Ʈ
	int multisample_level_;	// �ɼǿ��� ������ ��Ƽ���� ����
	int adj_multisample_level_; // FPS�� ���� ������ �������� ���� 
};