extern RwBool
RpDWSectorSetWriteMode(RpWorld *world, RwUInt8 mode);

// ������ Write ��常 �����Ѵ�. , ���带 ���̺��Ҷ�
// ��� DWSector �� ������ �����صΰ� , rpDWSECTOR_WRITEMODE_CURRENT ��带 �����Ͽ�
// BSP�ͽ���Ʈ�� �ϸ� ������ ���� ������ ������ ������ ����. 
// ���� Ŭ���̾�Ʈ������ �� ������ �ͽ���Ʈ�� bsp ������ �ε��Ͽ� ������ �����ְ�,
// �÷��̾ �ִ� �κи��� ���� �ε��ؼ� ����Ѵ�.

extern RwStream *
RpDWSectorStreamWrite(RpDWSector *dwSector, RwStream *stream);

extern RwStream *
RpDWSectorStreamRead(RpDWSector *dwSector, RwStream *stream);

extern RwInt32
RpDWSectorStreamGetSize(RpDWSector *dwSector);

// ��Ʈ���� ��ǵ�� , �÷��̾ ���� �ڸ��� �� ����� �Ἥ �޸𸮿� �ε��Ѵ�.
// AgcmMap���� �� ���ͺ��� ���� ���Ϳ� ���� �����͸� ����ó�� ������ �־���Ѵ�.
// �׷��� , �÷��̾��� ��ġ�� ���� ����Ÿ�� �ε��ϴ� ��ƾ�� �ִ´�.
// DWSector �� ������ ������ �������� �ε��ϴ� ���� ��ŭ ������ �Ѵ�. 
// �о���϶��� FindChuck�� �̿��Ͽ� ������� ����� ���� �ϳ��� �Ѿ��
// ã�Ƴ��� �о� ���̵��� �Ѵ�.

// �������� ��ǵ�
1,	AgcmMap::LoadDwSector( int x , int y )
	AgcmMap::LoadDwSector( ApWorldSector *  )
  �ش� ���͸� �ε��Ѵ�.
2,	AgcmMap::ReleaseDwSector( int x , int y )
	AgcmMap::ReleaseDwSector( ApWorldSector *  )
	�ش� ���� ����Ÿ�� ������ �Ѵ�.
3,	AgcmMap::SetPlayerPostion( ApPOS )
	// ���� ����Ÿ�� ü�������� �ε� �Ѵ�.
	// ApPOS�� �������� ������ �ε��� �Ǿ� �ִ� ����Ÿ�� �����ϰ�.
	// �ʿ��Ѱ��� �ε��ϰ� , ������ �ʴ°��� �޸𸮿��� �����Ѵ�.
	// ������ �ܺο��� ����ϴ� �㼱�� �̰� �ϳ� ��.

// �׽�Ʈ Ŭ���̾�Ʈ ����.
�ʿ��� ���.
1,	���带 �ε���
2,	Ư�� ���带 DWSector �� �ε��ϰ� ������ ����
3,	��ü ������ �����ؼ� �ε��ž������� Ȯ��.
