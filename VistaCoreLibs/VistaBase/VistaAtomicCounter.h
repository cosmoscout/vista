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


#ifndef _VISTAATOMICCOUNTER_H_
#define _VISTAATOMICCOUNTER_H_

#include "VistaBaseConfig.h"
#include "VistaBaseTypes.h"


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

template< class BaseType >
class TVistaAtomicCounter
{
public:
	virtual ~TVistaAtomicCounter() {}

	operator BaseType() const { return m_nValue; }
	/**
	 * @brief Setter method for the non-parallel access case.
	 *
	 * To be used in case you have to re-adjust the initial value, but you are sure that
	 * there is no concurrent access at the same time.
	 */
	virtual void Set( BaseType nValue ) { m_nValue = nValue; }

	/**
	 * @brief Getter method for the value.
	 * 
	 * Should be safe to use a simple read here, as atomics imply memory barriers.
	 * For specific cases, this method is marked as virtual to allow override.
	 */
	virtual BaseType Get() const { return m_nValue; }
	virtual void Add( BaseType nValue)  = 0;
	virtual void Sub( BaseType nValue ) = 0;
	virtual void Inc() = 0;
	virtual void Dec() = 0;
	virtual bool DecAndTestNull() = 0;
	virtual bool AddAndTestNegative( BaseType nValue ) = 0;
	virtual BaseType ExchangeAndAdd( BaseType nValue ) = 0;

	void operator= ( BaseType nValue ) { Set(nValue); }
	void operator+=( BaseType nValue ) { Add(nValue); }
	void operator-=( BaseType nValue ) { Sub(nValue); }


	void operator++()
	{
		Inc();
	} // prefix operator

	TVistaAtomicCounter &operator++( int )
	{
		Inc();
		return *this;
	} // postfix operator

	void operator--()
	{
		Dec();
	} // prefix operator

	void operator--( int )
	{
		Dec();
	} // postfix operator

	BaseType operator==(const BaseType &other) const { return Get() == other; }
protected:
	TVistaAtomicCounter( BaseType initialValue = 0 )
	: m_nValue( initialValue )
	{

	}

	volatile BaseType m_nValue;
};

class VISTABASEAPI VistaSigned32Atomic : public TVistaAtomicCounter<VistaType::sint32>
{
public:
	VistaSigned32Atomic( VistaType::sint32 initialValue = 0 );

	virtual void Add( VistaType::sint32 nValue);
	virtual void Sub( VistaType::sint32 nValue );
	virtual void Inc();
	virtual void Dec();
	virtual bool DecAndTestNull();
	virtual bool AddAndTestNegative( VistaType::sint32 nValue );
	virtual VistaType::sint32 ExchangeAndAdd( VistaType::sint32 nValue );

        struct Private 
        { 
                Private() : m_pPrivate(0L) {}
                void *m_pPrivate; 
        } m_pPrivate;
};

class VISTABASEAPI VistaSigned64Atomic : public TVistaAtomicCounter<VistaType::sint64>
{
public:
	VistaSigned64Atomic( VistaType::sint64 initialValue = 0 );
	virtual ~VistaSigned64Atomic();

	virtual VistaType::sint64 Get() const;

	virtual void Add( VistaType::sint64 nValue);
	virtual void Sub( VistaType::sint64 nValue );
	virtual void Inc();
	virtual void Dec();
	virtual bool DecAndTestNull();
	virtual bool AddAndTestNegative( VistaType::sint64 nValue );
	virtual VistaType::sint64 ExchangeAndAdd( VistaType::sint64 nValue );
	struct Private 
	{ 
		Private() : m_pPrivate(0L) {}
		void *m_pPrivate; 
	} m_pPrivate;
};


#endif /* _VISTAATOMICCOUNTER_H_ */


/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/


