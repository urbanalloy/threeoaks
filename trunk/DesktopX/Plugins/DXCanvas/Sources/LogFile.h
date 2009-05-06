#pragma once

#include <stdio.h>
#include <string.h>
#include <tchar.h>

#pragma warning(disable: 4996)

/////////////////////////////////////////////////////////////////
//features:	not depends on MFC ore ATL.
//			file name could use absolute path or just the name, in which case the 
//			file will be created at the same place with the binary module, no concern 
//			with current directory, which always bring me trouble.
//			every log line has a time stamp attached, with million seconds.
//			uses printf like format to write log lines
//			uses a preprocessor definition _DEBUG_LOG to switch if write log file
//			multi thread safe, finally added:)

////////////////////////////////////////////////////
//	CLogFile, a debug log file wrapper
class CLogFile
{
public:
	//////////////////////////////
	//	Constructor, open the logfile
	CLogFile(LPCTSTR strFile, bool bAppend = FALSE, long lTruncate = 400000)
	{
#ifdef _DEBUG_LOG
		m_lTruncate = lTruncate;
		TCHAR	szFile[MAX_PATH+1];
		if (_tcslen(strFile)>3 && strFile[1]!=':')	//no absolute path designated
		{
			::GetModuleFileName(NULL, szFile, MAX_PATH);
			size_t llength = _tcslen(szFile);
			TCHAR*	pcat = szFile+(llength - 1);	//point to the last char
			while (llength--)
			{
				pcat--;
				if (*pcat == '\\')
					break;
			}
			if (*pcat == '\\')
			{
				pcat++;
				_tcscpy(pcat, strFile);
			}
			else	//something wrong, use the original filename, ignore path problem
				_tcscpy(szFile, strFile);
		}
		else
			_tcscpy(szFile, strFile);

		m_pLogFile = fopen(szFile, bAppend ? "a" : "w");
		InitializeCriticalSection(&m_cs);
#endif
	}

	/////////////////////////////////
	//	Destructor, close if logfile if opened
	~CLogFile()
	{
#ifdef _DEBUG_LOG
		if (m_pLogFile)
		{
			fputs("\n===============Finish Loging================\n\n", m_pLogFile);
			fclose(m_pLogFile);
		}
		DeleteCriticalSection(&m_cs);
#endif
	}
	/////////////////////////////////
	//	Write log info into the logfile, with printf like parameters support
	void Write(LPCTSTR pszFormat, ...)
	{
#ifdef _DEBUG_LOG
		if (!m_pLogFile)
			return;

		EnterCriticalSection(&m_cs);
		//write the formated log string to szLog
		TCHAR	szLog[256];
		va_list argList;
		va_start( argList, pszFormat );
		vsprintf( szLog, pszFormat, argList );
		va_end( argList );

		//Trancate if the file grow too large
		long	lLength = ftell(m_pLogFile);
		if (lLength > m_lTruncate)
			rewind(m_pLogFile);

		//Get current time
		SYSTEMTIME	time;
		::GetLocalTime(&time);
		TCHAR	szLine[256];
		
		sprintf(szLine, "%02d:%02d:%02d:%03d \t%s\n", 
			time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
			szLog);
		
		OutputDebugString(szLine);

		fputs(szLine, m_pLogFile);
		fflush(m_pLogFile);

		LeaveCriticalSection(&m_cs);
#endif
	}

#ifdef _DEBUG_LOG
private:
	FILE*	m_pLogFile;
	long	m_lTruncate;
	CRITICAL_SECTION	m_cs;
#endif
};
