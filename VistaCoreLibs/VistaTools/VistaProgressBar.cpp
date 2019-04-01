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


#include "VistaProgressBar.h"

#include <VistaBase/VistaStreamUtils.h>

#include <VistaBase/VistaTimer.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
using namespace std;

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaProgressBar::VistaProgressBar(double fCountMax,
									double fCountInc,
									std::ostream* pStream)
: m_bRunning(false),
  m_bSilent(false),
  m_fCountCurrent(0),
  m_fCountMax(fCountMax),
  m_fCountIncrement(fCountInc),
  m_bDisplayPrefix(false),
  m_bDisplayBar(true),
  m_bDisplayPercentage(true),
  m_strPrefix(""),
  m_iBarTicks(40),
  m_iCurrentTicks(0),
  m_iCurrentPercentage(0),
  m_pTimer(NULL),
  m_fLastTime(0),
  m_pStream( pStream )
{
}


VistaProgressBar::~VistaProgressBar()
{
	if (m_bRunning)
		Finish(false);
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Start                                                       */
/*                                                                            */
/*============================================================================*/
bool VistaProgressBar::Start()
{
	if (m_bRunning)
	{
		vstr::warnp() << "[VistaProgressBar]: Unable to start progress bar - it's already running..." << endl;		
		return false;
	}

	// reset counters
	m_fCountCurrent = 0;
	m_iCurrentTicks = 0;
	m_iCurrentPercentage = 0;

	// make sure, we're starting in a clean line...
//	if (!m_bSilent)
//		cout << std::endl;

	// and draw...
	Draw();

	m_pTimer = new VistaTimer();

	m_bRunning = true;

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Finish                                                      */
/*                                                                            */
/*============================================================================*/
bool VistaProgressBar::Finish(bool bComplete)
{
	if (!m_bRunning)
	{
		vstr::warnp() << "[VistaProgressBar]: Unable to finish progress bar - it's not running..." << endl;
		return false;
	}

	// memorize time needed
	m_fLastTime = m_pTimer->GetLifeTime();
	delete m_pTimer;
	m_pTimer = NULL;

	// are we forced to display a completed result...?
	if (bComplete)
	{
		m_iCurrentTicks = m_iBarTicks;
		m_iCurrentPercentage = 100;
		m_fCountCurrent = m_fCountMax;
	}

	// if we have a completed result, draw it...
	if (m_iCurrentPercentage == 100)
		Draw();

	// advance a line
	if (!m_bSilent)
		vstr::out() << std::endl;

	// that's it...
	m_bRunning = false;

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Reset                                                       */
/*                                                                            */
/*============================================================================*/
bool VistaProgressBar::Reset(double fCountMax, double fCountInc)
{
	if (m_bRunning)
	{
		vstr::warnp() << "[VistaProgressBar]: Unable to reset bar while it's running..." << endl;
		return false;
	}

	if (fCountMax <= 0 || fCountInc <= 0)
	{
		vstr::warnp() << "[VistaProgressBar]: Unable to set count maximum and/or increment to negative values..." << endl;
		return false;
	}

	m_fCountMax = fCountMax;
	m_fCountIncrement = fCountInc;

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetSilent                                                   */
/*                                                                            */
/*============================================================================*/
void VistaProgressBar::SetSilent(bool bSilent)
{
	m_bSilent = bSilent;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetPrefixString                                             */
/*                                                                            */
/*============================================================================*/
bool VistaProgressBar::SetPrefixString(std::string strPrefix)
{
	if (m_bRunning)
	{
		vstr::warnp() << "[VistaProgressBar]: Unable to set prefix string while bar is running..." << endl;
		return false;
	}

	m_strPrefix = strPrefix;

	if (!m_strPrefix.empty())
	{
		// assume, that this prefix is to be displayed...
		m_bDisplayPrefix = true;
	}

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetDisplayPrefix                                            */
/*                                                                            */
/*============================================================================*/
bool VistaProgressBar::SetDisplayPrefix(bool bDisplayPrefix)
{
	if (m_bRunning)
	{
		vstr::warnp() << "[VistaProgressBar]: Unable to set prefix display while bar is running..." << endl;
		return false;
	}

	m_bDisplayPrefix = bDisplayPrefix;

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetDisplayBar                                               */
/*                                                                            */
/*============================================================================*/
bool VistaProgressBar::SetDisplayBar(bool bDisplayBar)
{
	if (m_bRunning)
	{
		vstr::warnp() << "[VistaProgressBar]: Unable to set bar display while bar is running..." << endl;
		return false;
	}

	m_bDisplayBar = bDisplayBar;

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetDisplayPercentage                                        */
/*                                                                            */
/*============================================================================*/
bool VistaProgressBar::SetDisplayPercentage(bool bDisplayPercentage)
{
	if (m_bRunning)
	{
		vstr::warnp() << "[VistaProgressBar]: Unable to set percentage display while bar is running..." << endl;
		return false;
	}

	m_bDisplayPercentage = bDisplayPercentage;

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetBarTicks                                                 */
/*                                                                            */
/*============================================================================*/
bool VistaProgressBar::SetBarTicks(int iBarTicks)
{
	if (m_bRunning)
	{
		vstr::warnp() << "[VistaProgressBar]: Unable to set bar ticks while bar is running..." << endl;
		return false;
	}

	m_iBarTicks = iBarTicks;

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Draw                                                        */
/*                                                                            */
/*============================================================================*/
void VistaProgressBar::Draw()
{
	if (m_bSilent)
		return;

	(*m_pStream) << "\r";

	if (m_bDisplayPrefix)
		(*m_pStream) << m_strPrefix;

	if (m_bDisplayBar)
	{
		(*m_pStream) << "[";

		int i=0;
		for (i=0; i<m_iCurrentTicks; ++i)
		{
			(*m_pStream) << "*";
		}
		for (; i<m_iBarTicks; ++i)
		{
			(*m_pStream) << "-";
		}

		(*m_pStream) << "] ";
	}

	if (m_bDisplayPercentage)
	{
		if (m_pTimer)
		{
			// we're still running and timing
			(*m_pStream) << "[" << m_iCurrentPercentage << "% complete]  ";
		}
		else
		{
			(*m_pStream) << "[" << m_fLastTime << " seconds]    ";
		}
	}

	(*m_pStream).flush();
}

void VistaProgressBar::SetOutstream( std::ostream* pStream )
{
	m_pStream = pStream;
}

std::ostream* VistaProgressBar::GetOutstream()
{
	return m_pStream;
}

