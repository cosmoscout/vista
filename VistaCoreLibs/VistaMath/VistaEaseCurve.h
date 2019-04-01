/*============================================================================*/
/*       1         2         3         4         5         6         7        */
/*3456789012345678901234567890123456789012345678901234567890123456789012345678*/
/*============================================================================*/
/*                                                                            */
/*                                               RRRR WW  WW   WTTTTTTHH  HH  */
/*                                               RR RR WW WWW  W  TT  HH  HH  */
/*      Header   :                               RRRR   WWWWWWWW  TT  HHHHHH  */
/*                                               RR RR   WWW WWW  TT  HH  HH  */
/*      Module   :  ViSTA                        RR  R    WW  WW  TT  HH  HH  */
/*                                                                            */
/*      Project  :  ViSTA                          Rheinisch-Westfaelische    */
/*                                               Technische Hochschule Aachen */
/*      Purpose  :  ...                                                       */
/*                                                                            */
/*                                                 Copyright (c)  1998-2016   */
/*                                                 by  RWTH-Aachen, Germany   */
/*                                                 All rights reserved.       */
/*                                                                            */
/*============================================================================*/
/*                                                                            */
/*    THIS SOFTWARE IS PROVIDED 'AS IS'. ANY WARRANTIES ARE DISCLAIMED. IN    */
/*    NO CASE SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DAMAGES.    */
/*    REDISTRIBUTION AND USE OF THE NON MODIFIED TOOLKIT IS PERMITTED. OWN    */
/*    DEVELOPMENTS BASED ON THIS TOOLKIT MUST BE CLEARLY DECLARED AS SUCH.    */
/*                                                                            */
/*============================================================================*/
/*                                                                            */
/*      CLASS DEFINITIONS:                                                    */
/*                                                                            */
/*        - VistaEaseCurve                                                    */
/*                                                                            */
/*============================================================================*/

#ifndef _VFL_EASE_CURVE_H_201109121643
#define _VFL_EASE_CURVE_H_201109121643

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "../VistaBase/VistaMathBasics.h"
#include "VistaMathConfig.h"

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * The VflEaseCurve provides different ease curves, i.e. functions that map the
 * range of double values [0.0 .. 1.0] to the range [0.0 .. 1.0] to control the
 * speed of the interpolation between these two values. Some basic ease curves
 * are already implemented within this class. If additional curves are needed,
 * they should not be implemented by inheriting this class, but should be
 * implemented here to make them available for other developers.
 *
 * Until now the following ease curves are implemented as in, out, in-out and
 * out-in variants:
 * - linear
 * - quadratic
 * - cubic
 * - sinus
 * - circular
 */

class VISTAMATHAPI VistaEaseCurve
{
public:
	enum eEaseCurveType
	{
		LINEAR,
		QUADRATIC_IN,
		QUADRATIC_OUT,
		QUADRATIC_IN_OUT,
		QUADRATIC_OUT_IN,
		CUBIC_IN,
		CUBIC_OUT,
		CUBIC_IN_OUT,
		CUBIC_OUT_IN,
		SIN_IN,
		SIN_OUT,
		SIN_IN_OUT,
		SIN_OUT_IN,
		CIRC_IN,
		CIRC_OUT,
		CIRC_IN_OUT,
		CIRC_OUT_IN
	};

	VistaEaseCurve();

	virtual ~VistaEaseCurve();

	/**
	 * Returns a transformed value in the range of [0..1] for a given value that
	 * also lies in the range of [0..1]. The function used to transform the
	 * value is chosen based on the ease curve type, specified by SetType(...)
	 * @return	double
	 * @param	double t
	 */
	virtual double GetValue(double t) const;

	void SetType(VistaEaseCurve::eEaseCurveType val);

	VistaEaseCurve::eEaseCurveType GetType() const;

protected:
	typedef double(*ValueFn)(double);

	static inline double GetInValue(double t, ValueFn pValueFn);

	static inline double GetOutValue(double t, ValueFn pValueFn);

	static inline double GetInOutValue(double t, ValueFn pValueFn);

	static inline double GetOutInValue(double t, ValueFn pValueFn);

	static inline double GetQuadraticValue(double t);

	static inline double GetCubicValue(double t);

	static inline double GetSinValue(double t);

	static inline double GetCircValue(double t);

private:
	eEaseCurveType	m_eType;
};


#endif // _VFL_EASE_CURVE_H_201109121643
