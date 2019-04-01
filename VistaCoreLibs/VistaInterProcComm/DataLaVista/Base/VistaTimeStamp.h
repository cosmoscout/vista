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


#if !defined(DLVISTATIMESTAMP_H)
#define DLVISTATIMESTAMP_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include "VistaDLVTypes.h"

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * An abstraction for a time-stamp value that is more accurate than a simple
 * 'double' or similiar. It does define a value taken from a high-resolution
 * clock in addition to a more or less low-resolution clock value.
 * The semantics for this a dependant on the type and quality of the clocks that we
 * have given by the operating system.
 */
class VISTAINTERPROCCOMMAPI DLVistaTimeStamp
{
private:
	/**
	 * a wide attribute for a high-resolution clock-value (e.g. cpu-clock-count).
	 * Note that this is supposed to be 64bit-wide clock counter which may not
	 * always be available
	 */
	DLV_INT64 m_i64MicroStamp;

	/**
	 * a narrow attribute for a "not-that-high" resolution clock value (e.g. system-time, seconds etc.)
	 * Note that the content is dependant on the availability on the operating system.
	 */
	DLV_INT32 m_i32MacroStamp;

	/**
	 * This is a special value that may indicate the number of clock-ticks that
	 * can differ from m_i64MicroStamp. This value is usually -1, indicating that
	 * the micro-stamp is precise.
	 * Note that this field is experimental.
	 */
	DLV_INT32 m_i32UnsafetyValue;

protected:
public:
	/**
	 * Clears this timestamp and inititalizes all values to -1, which
	 * indicates an invalid timestamp.
	 */
	DLVistaTimeStamp();

	/**
	 * Initializes the micro- and macro-stamp with the given values.
	 * Note that the unsafety-value remains -1
	 * @param i64MicroStamp the microStamp for this timestamp
	 * @param i32MacroStamp the macroStamp for this timestamp.
	 * @see m_i64MicroStamp()
	 * @see m_i32MacroStamp()
	 */
	DLVistaTimeStamp(DLV_INT64 i64MicroStamp, DLV_INT32 i32MacroStamp);

	/**
	 * Copies the values for this timestmp from rStamp
	 * @param rStamp the timestamp to copy the values from
	 */
	DLVistaTimeStamp(const DLVistaTimeStamp &rStamp);

	/**
	 * Destructor. Sets all values to -1 (invalid timestamp)
	 */
	virtual ~DLVistaTimeStamp();


	/**
	 * currently empty
	 * @todo define equality for timestamps!
	 */
	virtual bool Equals(const DLVistaTimeStamp &);

	/**
	 * Sets the given stamp-values for this timestamp.
	 * @param i64MicroStamp the VistaType::microstamp to set
	 * @param i32MacroStamp the macrostamp to set
	 */
	virtual void Stamp(DLV_INT64 i64MicroStamp, DLV_INT32 i32MacroStamp)
				{
					m_i64MicroStamp = i64MicroStamp;
					m_i32MacroStamp = i32MacroStamp;
				};

	/**
	 * Sets the unsafetyvalue for this timestamp. This is experimental.
	 * @todo define "what is unsafety"
	 */
	virtual void SetUnsafetyValue(DLV_INT32 i32UnsafetyValue)
	{
		m_i32UnsafetyValue = i32UnsafetyValue;
	};


	/**
	 * returns the Microstamp for this value.
	 * Note that this is supposed to be a 64bit-value (long long), and that
	 * you might run into trouble printing this via printf or similiar. Look
	 * for the corrent tranformation parameters!
	 * -1 indicates an invalid VistaType::microstamp.
	 * @return the value of m_i64MicroStamp
	 */
	DLV_INT64 GetMicroStamp() const { return m_i64MicroStamp; };

	/**
	 * returns the Macrostamp for this value.
	 * -1 indicates an invalid macrostamp.
	 * @return the value of m_i32MacroStamp
	 */
	DLV_INT32 GetMacroStamp() const { return m_i32MacroStamp; };

	/**
	 * Returns the unsafetyvalue for this timestamp.
	 * -1 indicates an invalid unsafety value.
	 * Note that this field is experimental.
	 */
	DLV_INT32 GetUnsafetyValue() const { return m_i32UnsafetyValue; };

	/**
	 * empty
	 * @todo define equality for timestamps
	 */
	bool operator==(const DLVistaTimeStamp &);

	/**
	 * emtpy
	 * @todo define an order for timestamps
	 */
	bool operator>(const DLVistaTimeStamp &);

	/**
	 * emtpy
	 * @todo define an order for timestamps
	 */
	bool operator<(const DLVistaTimeStamp &);

	/**
	 * empty
	 * @todo define an order for timestamps.
	 */
	bool operator<=(const DLVistaTimeStamp &);

	/**
	 * empty
	 * @todo define an order for timestamps
	 */
	bool operator>=(const DLVistaTimeStamp &);

	/**
	 * Copies micro-, macro- and unsafety value respectively.
	 */
	DLVistaTimeStamp &operator=(const DLVistaTimeStamp &);
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif // !defined(DLVISTATIMESTAMP_H)


