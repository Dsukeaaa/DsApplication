#ifndef _DS_FILE_
#define _DS_FILE_


//#define __USE_IFSTREAM0
//#define __USE_IFSTREAM1
#define __USE_IFSTREAM2		//これが一番早い


namespace DsLib
{
	class DsFile
	{
	public:
		enum class SEEK_TYPE : int
		{
			BEG,
			CUR,
		};

	public:
		DsFile(const char *_Filename, std::ios_base::openmode _Mode = std::ios_base::in, int _Prot = (int)std::ios_base::_Openprot);
		DsFile(const wchar_t *_Filename, std::ios_base::openmode _Mode = std::ios_base::in, int _Prot = (int)std::ios_base::_Openprot);

		virtual ~DsFile();

		void Read(ds_uint8* pBuf, std::streamsize size);

		bool IsFail() const;

		void Seekg(long long int _Off, SEEK_TYPE _Way);

		bool GetLine(std::string& str);

		size_t GetSize() const { return m_bufSize; }

		ds_uint8* GetBufferTop()const { return m_pBufTop; }
		ds_uint8* MoveBufferTop();

		ds_uint8* GetReadBuffer()const { return m_pReadBuf; }

	private:
		void _Init();

	private:
#if defined __USE_IFSTREAM0
		std::ifstream m_ifstream;
#elif defined __USE_IFSTREAM1
		std::ifstream m_ifstream;
		std::string m_bufTop;
		const char* m_pReadBuf;
#elif defined __USE_IFSTREAM2
		ds_uint8* m_pBufTop;
		ds_uint8* m_pReadBuf;
		size_t m_bufSize;
#endif
	};


}

#endif