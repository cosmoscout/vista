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
// $Id: VistaSerializable.h 29136 2012-02-08 09:35:49Z dr165799 $

#ifndef _VISTASHAREDCOREOWNER_H
#define _VISTASHAREDCOREOWNER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaAspectsConfig.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaSharedCore;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * 
 */
class VISTAASPECTSAPI IVistaSharedCoreOwner
{
public:
	enum DataHandling
	{
		DH_ALWAYS_COPY,
		DH_ALWAYS_SHARE,
		DH_COPY_ON_WRITE,
	};
	DataHandling GetDataHandlingMode() const;
	void SetDataHandlingMode( const DataHandling eMode );

	const IVistaSharedCore* GetCore() const;
	IVistaSharedCore* GetCoreForWriting();

protected:
	void PrepareCoreForWriting();
	void MakeCoreUniquelyUsed();

	IVistaSharedCoreOwner& operator= ( const IVistaSharedCoreOwner& oOther );

protected:
	IVistaSharedCoreOwner( IVistaSharedCore* pCore,
							const DataHandling eDataHandlingMode );
	IVistaSharedCoreOwner( const IVistaSharedCoreOwner& oCopy );
	virtual ~IVistaSharedCoreOwner();
protected: // @IMGTODO: temporarily un-privated for swattextures
	DataHandling m_eDataHandlingMode;
	IVistaSharedCore* m_pCore;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASHAREDCOREOWNER_H

