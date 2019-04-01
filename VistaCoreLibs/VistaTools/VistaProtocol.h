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


#if !defined(_VISTAPROTOCOL_H)
#define _VISTAPROTOCOL_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaToolsConfig.h"

#include <map>
#include <string>

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

class VISTATOOLSAPI VistaProtocol
{
public:
	class TimeInterval
	{
	public:
		double dBegin;
		double dEnd;
		TimeInterval(const double &tBegin = 0.0,
					 const double &tEnd = 0.0)
		: dBegin(tBegin), dEnd(tEnd)
		{};

		double interval(double dSysTime) const
		{
			if(dEnd == 0)
				return (dSysTime - dBegin);
			else
				return (dEnd - dBegin);
		}
	};

	// CONSTRUCTOR / DESTRUCTOR
	// Initializing class variables
	VistaProtocol();
	virtual ~VistaProtocol();

	//IMPLEMENTATION

	//creates new time interval in interval list and start a interval by its name
	void StartMeasureByName(const std::string &sName);

	void StartMeasureByKey(int);
	void StopMeasureByKey(int);

	//stop a interval by its name
	void StopMeasureByName(const std::string &sName);


	//get the interval or actual runtime of interval by its name
	//name is not existing, if return value -1 is
	double GetIntervalByName(const std::string &sName);

	//set output filename
	void SetFileName(const std::string &sFileName);

	//set text for output of interval
	void SetUserText(const std::string &sText);

	void SetSeparator(char cSep);
	char GetSeparator() const;

	//writes output file
	bool WriteProtocol();

	//set file openmode: true - for overwrite, false - for append
	void SetOverwrite(bool bOverwrite);

	bool GetOverwrite() const;

	void SetPrintHeader(bool bPrintHeader);
	bool GetPrintHeader() const;

	void SetWriteKey(bool bWriteKey);
	bool GetWriteKey() const;

	void SetWriteColumnHeaders(bool bWriteColHeads);
	bool GetWriteColumnHeaders() const;

private:

	/**
	* map of time measurements.
	* We need this for faster lookup of intervals.
	*/
	std::map<std::string, TimeInterval> m_mpMeasureMap;

	std::map<int, TimeInterval> m_mpMsrMap;
	/**
	* name of outputfile.
	*/
	std::string m_sFileName;

	/**
	* remarks from user, will be printed in header
	*/
	std::string m_sUserText;

	/**
	* represents a date, needed for header
	*/
	std::string m_sDateOfMeasure;


	/**
	* We need this for time functions
	*/
	VistaTimer * m_Timer;


	/**
	* file openmode: true - for overwrite, false - for append,
	* default: true -> overwrite
	*/
	bool m_bOverwrite;

	/**
	* skip header with system information, default: true -> print header
	*/
	bool m_bWriteHeader;

	bool m_bWriteKey;

	bool m_bWriteColumnHeaders;

	char m_cSeparator;
};


class VistaProtocolMeasure
{
private:
	std::string m_sMeasureName;
	int m_iKey;
	bool m_bNamedMeasure;
	VistaProtocol &m_rProtocol;
public:
	VistaProtocolMeasure(VistaProtocol &rProtocol, int iKey)
		: m_rProtocol(rProtocol), m_iKey(iKey), m_bNamedMeasure(false)
	{
		m_rProtocol.StartMeasureByKey(iKey);
	};

	VistaProtocolMeasure(VistaProtocol &rProtocol, const std::string &sName)
		: m_rProtocol(rProtocol), m_sMeasureName(sName), m_iKey(0), m_bNamedMeasure(true)
	{
		m_rProtocol.StartMeasureByName(sName);
	};

	virtual ~VistaProtocolMeasure()
	{
		if(m_bNamedMeasure)
			m_rProtocol.StopMeasureByName(m_sMeasureName);
		else
			m_rProtocol.StopMeasureByKey(m_iKey);
	};
};
#endif //_VISTAPROTOCOL_H
