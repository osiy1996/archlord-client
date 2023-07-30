#ifndef  __WBANetwork_CircuitQueue_H
#define  __WBANetwork_CircuitQueue_H


namespace WBANetwork
{
	template < class _T >
	class CircuitQueue
	{
	public:
				CircuitQueue()
					: m_buffer( NULL ), m_dataSize( 0 ), m_bufferSize( 0 ),
					m_posHead( 0 ), m_posTail( 0 ), m_surplusSize( 0 )
				{
				}
		virtual ~CircuitQueue()
				{
					Destroy();
				}

				bool	Create( DWORD bufSize )
				{
					if( m_buffer != 0 )
					{
						return false;
					}

					m_surplusSize	= bufSize * 10 / 100;
					m_buffer		= _new_dbg_ _T[bufSize + m_surplusSize];
					m_bufferSize	= bufSize;

					Clear();

					return true;
				}

				void	Destroy()
				{
					if( m_buffer != 0 )
						delete [] m_buffer;

					m_buffer = 0;
					Clear();
				}

				void	Clear()
				{
					m_dataSize	= 0;
					m_posHead	= 0;
					m_posTail	= 0;
				}

				DWORD		GetRemainBufSize()		{	return m_bufferSize - m_dataSize;	}
				DWORD		GetDataSize()			{	return m_dataSize;					}
				DWORD		GetBufferSize()			{	return m_bufferSize;				}

				bool	IsValidWritePtr( _T* pos, DWORD size )
				{
					DWORD bufStart = 0;

					DWORD bufEnd			= m_bufferSize - 1;
					DWORD posWriteStart	= ( DWORD )( pos - m_buffer );
					DWORD posWriteEnd		= ( DWORD )( pos - m_buffer + size - 1 );

					if( m_posHead <= m_posTail )
					{
						//  ������ ���۰� head ���̿� ���� ���
						if( bufStart >= posWriteStart && m_posHead > posWriteEnd )
							return true;

						// tail�� ������ �� ���̿� ���� ���
						if( m_posTail >= posWriteStart && posWriteEnd <= bufEnd )
							return true;
					}
					else
					{
						//  ������ tail�� head ���̿� �־�� �Ѵ�.
						if( m_posTail >= posWriteStart && posWriteEnd < m_posHead )
							return true;
					}

					return false;
				}

				bool	Enqueue( _T* data, DWORD size )
				{
					if( GetRemainBufSize() < size )
					{
						return false;
					}

					if( data != 0 )
					{
						if( m_posHead <= m_posTail )
						{
							DWORD		remainSize = m_bufferSize - m_posTail;

							//  ������ ������ �ִٸ� �ѹ��� �����Ѵ�.
							if( remainSize >= size )
								memcpy( m_buffer + m_posTail, data, sizeof( _T ) * size );
							else
							{
								//  ���ۿ� ������ ���ٸ� �����͸� �ΰ��� ������
								//  ������ ��/�ڿ� �����Ѵ�.
								memcpy( m_buffer + m_posTail, data, sizeof( _T ) * remainSize );
								memcpy( m_buffer, data + remainSize, sizeof( _T ) * ( size - remainSize ) );
							}
						}
						else
							memcpy( m_buffer + m_posTail, data, sizeof( _T ) * size );
					}

					m_posTail	= ( m_posTail + size ) % m_bufferSize;
					m_dataSize	+= size;

					return true;
				}

				bool	Dequeue( _T* dest, DWORD size )
				{
					if( Peek( dest, size ) == false )
					{
						return false;
					}

					m_posHead	+= size;
					m_posHead	%= m_bufferSize;
					m_dataSize	-= size;

					return true;
				}

				bool	Peek( _T* dest, DWORD size )
				{
					if( m_dataSize < size )
					{
						return false;
					}

					if( dest != 0 )
					{
						if( m_posHead < m_posTail )
							memcpy( dest, m_buffer + m_posHead, sizeof( _T ) * size );
						else
						{
							DWORD		cutDataSize = m_bufferSize - m_posHead;

							//  �䱸�� ũ�⸸ŭ ����Ǿ�(Linear) �������� ���
							if( cutDataSize >= size )
								memcpy( dest, m_buffer + m_posHead, sizeof( _T ) * size );
							else
							{
								//  �����Ͱ� tail�� head�� ������ ����� ���
								memcpy( dest, m_buffer + m_posHead, sizeof( _T ) * cutDataSize );
								memcpy( dest + cutDataSize, m_buffer, sizeof( _T ) * ( size - cutDataSize ) );
							}
						}
					}

					return true;
				}

				_T*		GetReadPtr()			{	return ( m_buffer + m_posHead );		}
				_T*		GetWritePtr()			{	return ( m_buffer + m_posTail );		}

				DWORD		GetReadableSize()
				{
					if( m_posHead == m_posTail )
						return ( m_dataSize > 0 ? m_bufferSize - m_posHead : 0 );
					else if( m_posHead < m_posTail )
						return ( m_posTail - m_posHead );

					//  Tail�� ���� ť�� �ѹ��� �ѱ� ��Ȳ���� Surplus buffer�� ����� �� �ִٸ�...
					if( m_posHead > m_posTail &&
						(( m_bufferSize - m_posHead + m_posTail ) < m_surplusSize) )
					{
						//  0 ~ tail������ ���۸� ���Ƿ� Surplus buffer�� �Űܿ�
						//  �ѹ��� ���� �� �ֵ��� �Ѵ�.

						memcpy( m_buffer + m_bufferSize, m_buffer, m_posTail );
						return ( m_bufferSize - m_posHead + m_posTail );
					}

					return ( m_bufferSize - m_posHead );
				}

				DWORD		GetWritableSize()
				{
					if( m_posHead == m_posTail )
					{
						return ( m_dataSize > 0 ? 0 : m_bufferSize - m_posTail );
					}
					else if( m_posHead < m_posTail )
					{
						return m_bufferSize - m_posTail;
					}

					return m_posHead - m_posTail;
				}

	private:
				_T*			m_buffer;
				DWORD		m_dataSize;
				DWORD		m_bufferSize;
				DWORD		m_posHead;
				DWORD		m_posTail;
				DWORD		m_surplusSize;
	};
}

#endif