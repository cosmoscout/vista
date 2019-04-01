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


/**
 *
 *      CLASS DEFINITIONS:
 *
 *        - VistaFilter			   : virtual base class for all Filters
 *
 *        - VistaBuffer               : fifo buffer of specified length
 *							             (zero = pass through)
 *
 *        - VistaMFilter              : moving average
 *                                       (averages last two values)
 *
 *        - VistaSecondOrderFilter    : generic FIR or IIR
 *                                       Filter of second order
 *
 *        - VistaButterworthFilter    : butterworth lowpass build
 *                                       of a SecondOrderFilter
 *
 *        - VistaCriticalDampedFilter : critical damped lowpass build
 *                                       of a SecondOrderFilter
 *
 */

#ifndef _VISTAFILTER_H
#define _VISTAFILTER_H

/*============================================================================*/
/*  INCLUDES                                                                  */
/*============================================================================*/
#include <cmath>    // sqrt (), tan ()
// #include <VistaMath\VistaVectorMath.h>

/**
 *	This is a pure virtual baseclass for all filters needed by VISTA
 *	(especially lowpass-filters).
 *	Derive if you need more filter types.
 */
template <class ClassType>
class Filter
{
public:

	// Get the last filtered value
	// Override this member to implement your functionality.
	virtual void get( ClassType & outValue ) const = 0;

	// Put a new value into the filter
	// Override this member to implement your functionality.
	virtual void push( const ClassType & inValue ) = 0;

	//Do get and set in one cycle
	inline void apply( ClassType & InoutValue )
	{
		push ( InoutValue );
		get  ( InoutValue );
	}
};

/**
 * Buffers have a value of Length+1.
 * Use Length=0 for a dummy filter that does nothing.
 */
template <class ClassType, int Length = 0>
class Buffer : public Filter<ClassType>
{
public:

	//	Get the last buffered value
	virtual void get( ClassType & outValue ) const
	{
		outValue = myBuffer[position];
	}

	// Put a new value into the buffer
	virtual void push( const ClassType & inValue )
	{
		myBuffer[position] = inValue;       // write value
		position = ++position % (Length+1);	// step ahead
	}

private:

	// buffer array
	ClassType	myBuffer[Length+1];

	//position counter
	int position;
	};

/**
 * M-Filter (moving avarage)
 */
template <class ClassType>
class MFilter : public Filter<ClassType>
{
public:

	// Get the last filtered value
	void get( ClassType & outValue ) const
	{
		outValue = original[0] + original[1];
		outValue *= 0.5;
	}


	// Put a new value into the filter (forget the oldest) and recalc
	void push( const ClassType & inValue )
	{
		original[1] = original[0];
		original[0] = inValue;
	}

	private:

	// filter queue
	ClassType	original[2];
};

/**
 * A second order filter - ready to use
 */
template <class ClassType>
class SecondOrderFilter : public Filter<ClassType>
{
public:

	// Build a 2nd order filter
	inline SecondOrderFilter( )
		: a0 ( 1 ), a1 ( 0 ), a2 ( 0 ), b1 ( 0 ), b2 ( 0 ) {}


	// Build a 2nd order filter of these coefficients
	inline SecondOrderFilter
		( const double inA0, const double inA1, const double inA2,
		  const double inB1, const double inB2 )
		: a0( inA0 ), a1( inA1 ), a2( inA2 ), b1( inB1 ), b2( inB2 ) {}


	// Copy constructor
	inline SecondOrderFilter ( const SecondOrderFilter & Other )
	{
		// setup coefficients
		a0 = Other.a0;
		a1 = Other.a1;
		a2 = Other.a2;
		b1 = Other.b1;
		b2 = Other.b2;
	}

	// Get the last filtered value
	void get( ClassType & outValue ) const
	{
		outValue = filtered[0];
	};

	// Get one of the last filtered values (0 = last)
	void get( ClassType & outValue, const int inIndex ) const
	{
		outValue = (inIndex > 0 && inIndex < 3) ? filtered[ inIndex ] : filtered[0];
	};

	// Put a new value into the filter (forget the oldest) and recalc
	void push( const ClassType &  inValue )
	{
		// shift the arrays up
		original[2] = original[1];
		original[1] = original[0];
		original[0] = inValue;

		filtered[2] = filtered[1];
		filtered[1] = filtered[0];

		// calulate filter
		filtered[0] = original[0] * a0 +
					  original[1] * a1 +
					  original[2] * a2 +
					  filtered[1] * b1 +
					  filtered[2] * b2;
	}

	// Assignment operator (copies filter coefficients)
	const SecondOrderFilter & operator= ( const SecondOrderFilter & Original )
	{
		a0 = Original.a0;
		a1 = Original.a1;
		a2 = Original.a2;
		b1 = Original.b1;
		b2 = Original.b2;

		return Original;
		}

	// Comparison operator (compares filter coefficients)
	bool operator== ( const SecondOrderFilter & Other ) const
	{
		return ( ( a0 == Other.a0 ) &&
				 ( a1 == Other.a1 ) &&
				 ( a2 == Other.a2 ) &&
				 ( b1 == Other.b1 ) &&
				 ( b2 == Other.b2 ) );
	}

protected:

	// filter coefficients
	double a0, a1, a2, b1, b2;

	// filter queues
	ClassType	original[3], filtered[3];
};

/**
 * Butterworth lowpass
 */
template <class ClassType>
class ButterworthFilter : public SecondOrderFilter<ClassType>
{
public:

	// Build Vista Butterworth lowpass of 2nd order with this cut off frequency
	// default sample frequency 1kHz
	inline ButterworthFilter ( const double inCutOff, const double inSample = 1000.0 )
	{
		const float PI = 3.14159265358979323846f;
		double omega, k1, k2, k3;

		// initialize filter coefficients
		omega = tan ( PI * inCutOff / inSample );

		k1 = sqrt(2) * omega;
		k2 = omega * omega;

		a0 = k2 / ( 1 + k1 + k2 );
		a1 = 2 * a0;
		a2 = a0;
		k3 = 2 * a0 / k2;
		b1 = k3 - 2 * a0;
		b2 =  1 - 2 * a0 - k3;
	}

};

/**
 * critical damped lowpass
 */
template <class ClassType>
class CriticalDampedFilter : public SecondOrderFilter<ClassType>
{
public:

	// Build a critical damped lowpass of 2nd order with this cut off frequency
	// default sample frequency 1kHz
	inline CriticalDampedFilter ( const double inCutOff, const double inSample = 1000.0 )
	{
		const float PI = 3.14159265358979323846f;
		double omega, k1, k2, k3;

		// initialize filter coefficients
		omega = tan( PI * inCutOff / inSample);

		k1 = 2 * omega;
		k2 = omega * omega;

		a0 = k2 / ( 1 + k1 + k2 );
		a1 = 2 * a0;
		a2 = a0;
		k3 = 2 * a0 / k2;
		b1 = k3 - 2 * a0;
		b2 =  1 - 2 * a0 - k3;
	}
};

/*============================================================================*/

#endif  //_VISTAFILTER_H
