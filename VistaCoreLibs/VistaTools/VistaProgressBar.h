/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU Lesser General Public License as published  */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU Lesser General Public License for more details.                       */
/*                                                                            */
/*  You should have received a copy of the GNU Lesser General Public License  */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/


#if !defined(_VISTAPROGRESSBAR_H)
#define _VISTAPROGRESSBAR_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaToolsConfig.h"
#include <string>
#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaTimer;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTATOOLSAPI VistaProgressBar
{
public:
	// CONSTRUCTOR / DESTRUCTOR
	VistaProgressBar( double fCountMax = 100.0,
						double fCountInc = 1.0,
						std::ostream* pStream = &std::cout );
	virtual ~VistaProgressBar();

	//IMPLEMENTATION
	bool Start();
	bool Increment();
	bool Increment(double fIncrement);
	bool SetCounter(double fCounter);
	double GetCounter() const;
	bool Finish(bool bComplete = true);

	bool GetRunning() const;

	bool Reset(double fCountMax, double fCountInc);

	// should we display this thing at all?
	bool GetSilent() const;
	void SetSilent(bool bSilent);

	// control various display options
	std::string GetPrefixString() const;
	bool SetPrefixString(std::string strPrefix);

	bool GetDisplayPrefix() const;
	bool SetDisplayPrefix(bool bDisplayPrefix);

	bool GetDisplayBar() const;
	bool SetDisplayBar(bool bDisplayBar);

	bool GetDisplayPercentage() const;
	bool SetDisplayPercentage(bool bDisplayPercentage);

	int GetBarTicks() const;
	bool SetBarTicks(int iBarTicks);

	void SetOutstream( std::ostream* pStream );
	std::ostream* GetOutstream();

protected:
	void Draw();

	std::ostream* m_pStream;

	bool m_bRunning;
	bool m_bSilent;

	// internal progress counting
	double m_fCountCurrent, m_fCountMax, m_fCountIncrement;

	// various display options
	bool m_bDisplayPrefix, m_bDisplayBar, m_bDisplayPercentage;
	std::string m_strPrefix;
	int m_iBarTicks;

	// display progress
	int m_iCurrentTicks;
	int m_iCurrentPercentage;

	// time measurement
	VistaTimer *m_pTimer;
	double m_fLastTime;
};

/*============================================================================*/
/* INLINE METHODS                                                             */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetCounter                                                  */
/*                                                                            */
/*============================================================================*/
inline double VistaProgressBar::GetCounter() const
{
	return m_fCountCurrent;
}

inline bool VistaProgressBar::SetCounter(double fCounter)
{
	if (!m_bRunning)
	{
#ifdef DEBUG
		if (!m_bSilent)
			(*m_pStream) << " [VistaProgressBar] - WARNING!!! Unable to set count - bar is not running..." << std::endl;			
#endif
		return false;
	}

	if (fCounter < 0)
		fCounter = 0;

	if (fCounter > m_fCountMax)
		fCounter = m_fCountMax;

	m_fCountCurrent = fCounter;

	int iTicks = (int) ((m_fCountCurrent / m_fCountMax) * m_iBarTicks);
	int iPercentage = (int) ((m_fCountCurrent / m_fCountMax) * 100);

	if (iTicks != m_iCurrentTicks
		|| iPercentage != m_iCurrentPercentage)
	{
		m_iCurrentTicks = iTicks;
		m_iCurrentPercentage = iPercentage;

		Draw();
	}

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Increment                                                   */
/*                                                                            */
/*============================================================================*/
inline bool VistaProgressBar::Increment(double fIncrement)
{
	return SetCounter(m_fCountCurrent + fIncrement);
}

inline bool VistaProgressBar::Increment()
{
	return Increment(m_fCountIncrement);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetRunning                                                  */
/*                                                                            */
/*============================================================================*/
inline bool VistaProgressBar::GetRunning() const
{
	return m_bRunning;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetSilent                                                   */
/*                                                                            */
/*============================================================================*/
inline bool VistaProgressBar::GetSilent() const
{
	return m_bSilent;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetPrefixString                                             */
/*                                                                            */
/*============================================================================*/
inline std::string VistaProgressBar::GetPrefixString() const
{
	return m_strPrefix;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplayPrefix                                            */
/*                                                                            */
/*============================================================================*/
inline bool VistaProgressBar::GetDisplayPrefix() const
{
	return m_bDisplayPrefix;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplayBar                                               */
/*                                                                            */
/*============================================================================*/
inline bool VistaProgressBar::GetDisplayBar() const
{
	return m_bDisplayBar;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplayPercentage                                        */
/*                                                                            */
/*============================================================================*/
inline bool VistaProgressBar::GetDisplayPercentage() const
{
	return m_bDisplayPercentage;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetBarTicks                                                 */
/*                                                                            */
/*============================================================================*/
inline int VistaProgressBar::GetBarTicks() const
{
	return m_iBarTicks;
}

#endif //_VISTAPROGRESSBAR_H

