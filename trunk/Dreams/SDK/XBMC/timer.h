////////////////////////////////////////////////////////////////////////////
//
// Author:
//   Joakim Eriksson
//
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "XBMC/types.h"

/***************************** D E F I N E S *******************************/
/****************************** M A C R O S ********************************/
/************************** S T R U C T U R E S ****************************/

////////////////////////////////////////////////////////////////////////////
//
class CTimer
{
public:

				CTimer();
	void		Init(void);
	void		Update(void);
	void		ResetTime(void);
	f32			GetDeltaTime(void);
	f64			GetTotalTime(void);

protected:
	LARGE_INTEGER	m_OldCount;
	LARGE_INTEGER	m_PFreq;
	f32				m_DeltaTime;
	f64				m_totalTime;
	
};

/***************************** G L O B A L S *******************************/
/***************************** I N L I N E S *******************************/

////////////////////////////////////////////////////////////////////////////
//
inline CTimer::CTimer()
{
	m_DeltaTime		= 0.0f;
	m_totalTime		= 0.0f;
}

////////////////////////////////////////////////////////////////////////////
//
inline void	CTimer::Init(void)
{
	QueryPerformanceFrequency(&m_PFreq);
	QueryPerformanceCounter(&m_OldCount);
}

////////////////////////////////////////////////////////////////////////////
//
inline void	CTimer::Update(void)
{
	LARGE_INTEGER newCount;
	QueryPerformanceCounter(&newCount);
	m_DeltaTime = (f32)((f64)(newCount.QuadPart-m_OldCount.QuadPart)/(f64)m_PFreq.QuadPart);
	m_OldCount = newCount;

	m_totalTime += m_DeltaTime;

	if (m_DeltaTime > 1.0f/30.0f)
		m_DeltaTime = 1.0f/30.0f;
}

////////////////////////////////////////////////////////////////////////////
inline f32 CTimer::GetDeltaTime(void)
{
	return m_DeltaTime;
}

inline f64 CTimer::GetTotalTime(void)
{
	return m_totalTime;
}

inline void CTimer::ResetTime(void)
{
	m_totalTime = 0.0f;
}

