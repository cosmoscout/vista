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


#ifndef _VISTAPROPERTYFUNCTOR_H
#define _VISTAPROPERTYFUNCTOR_H

#include "VistaAspectsConfig.h"

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaNameable.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaPropertyAwareable;
class VistaProperty;
class VistaPropertyFunctorRegistry;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAASPECTSAPI IVistaPropertyGetFunctor : public IVistaNameable
{
public:
	virtual ~IVistaPropertyGetFunctor();

	virtual bool operator()(const IVistaPropertyAwareable &,
							VistaProperty &) const = 0;

	void   SetNameForNameable(const std::string &sFunctorName);
	std::string GetNameForNameable() const;

	std::string GetPropertyDescription() const;

	std::string GetPropertyClassName() const;
protected:
	IVistaPropertyGetFunctor(const std::string &sPropname,
							const std::string &sClassName,
							const std::string &sDescription = "<none>");

private:
	IVistaPropertyGetFunctor(const IVistaPropertyGetFunctor &);
	IVistaPropertyGetFunctor &operator=(const IVistaPropertyGetFunctor &);

	VistaPropertyFunctorRegistry *m_pRegistry;
	std::string m_sFunctorName;
	std::string m_sFunctorClass;
	std::string m_sDescription;
};


class VISTAASPECTSAPI IVistaPropertySetFunctor : public IVistaNameable
{
public:
	virtual ~IVistaPropertySetFunctor();

	virtual bool operator()(IVistaPropertyAwareable &, const VistaProperty &) = 0;

	void SetNameForNameable(const std::string &sFunctorName);
	std::string GetNameForNameable() const;

	std::string GetPropertyDescription() const;
	std::string GetPropertyClassName() const;
protected:
	IVistaPropertySetFunctor(const std::string &sPropname,
							const std::string &sClassName,
							const std::string &sDescription = "<none>");
private:
	IVistaPropertySetFunctor(const IVistaPropertySetFunctor &);
	IVistaPropertySetFunctor &operator=(const IVistaPropertySetFunctor &);

	VistaPropertyFunctorRegistry *m_pRegistry;
	std::string m_sFunctorName;
	std::string m_sFunctorClass;
	std::string m_sDescription;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAPROPERTYFUNCTOR_H

