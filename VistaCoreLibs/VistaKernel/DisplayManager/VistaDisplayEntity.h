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


#if !defined _VISTADISPLAYENTITY_H
#define _VISTADISPLAYENTITY_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaAspects/VistaNameable.h>
#include <iostream>

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaAspects/VistaReflectionable.h>

/*============================================================================*/
/* DEFINITIONS                                                                */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaDisplayBridge;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI IVistaDisplayEntityData
{
public:
	virtual ~IVistaDisplayEntityData() {};
};

/**
 * VistaDisplayEntity is ...
 */
//class VISTAKERNELAPI VistaDisplayEntity : public IVistaPropertyAwareable, public IVistaNameable
class VISTAKERNELAPI VistaDisplayEntity : public IVistaNameable

{
public:
	enum EPropertyStatus
	{
		PROP_OK,
		PROP_NOT_FOUND,
		PROP_INVALID_VALUE,
		PROP_READ_ONLY
	};

	virtual ~VistaDisplayEntity();

	IVistaDisplayEntityData     *GetData() const;

	virtual void InitProperties();

	virtual void Debug ( std::ostream & out ) const;

	/**
	 * Interface from IVistaNameable
	 */
	virtual std::string GetNameForNameable() const;
	virtual void        SetNameForNameable(const std::string &strNewName);

	/**
	 * Some additions to that
	 */
	std::string GetNameableIdAsString() const;

	class VISTAKERNELAPI IVistaDisplayEntityProperties : public IVistaReflectionable
	{

	public:
		std::string GetName() const;
		virtual bool SetName(const std::string &sName);

		VistaDisplayEntity *GetParent() const;
		IVistaDisplayBridge *GetDisplayBridge() const;

		virtual std::string GetReflectionableType() const;

	protected:
		virtual ~IVistaDisplayEntityProperties();
		IVistaDisplayEntityProperties (VistaDisplayEntity *, IVistaDisplayBridge *);

		virtual int AddToBaseTypeList(std::list<std::string> &rBtList) const;

	private:
		// make uncopyable
		IVistaDisplayEntityProperties &operator=(const IVistaDisplayEntityProperties &);
		IVistaDisplayEntityProperties(const IVistaDisplayEntityProperties  &);

		friend class VistaDisplayEntity; // needed to call destructor

		VistaDisplayEntity *m_pParent;
		IVistaDisplayBridge *m_pBridge;
	};

	IVistaDisplayEntityProperties *GetProperties() const;

	IVistaDisplayBridge *GetDisplayBridge() const;
protected:
	virtual IVistaDisplayEntityProperties *CreateProperties() = 0;
	virtual bool DeleteProperties();	

	VistaDisplayEntity(IVistaDisplayEntityData *pData, IVistaDisplayBridge *pBridge);

private:
	IVistaDisplayEntityData       *m_pData;
	IVistaDisplayBridge           *m_pBridge;
	IVistaDisplayEntityProperties *m_pProperties;
	std::string                    m_strName;
};

VISTAKERNELAPI std::ostream & operator<< ( std::ostream &, const VistaDisplayEntity & );


/**
* TVistaDisplayEntityParentPropertyGet implements a templated getter functor for a
* IVistaReflectionable object. This is a specific getter for VistaDisplayEntity's
* for the case when a getter of the parent object is to be called.
* The meaning of the template arguments is as follows:
*
* R    return type of getter method
* P    type of the Parent (DisplayEntity) whose function is actually invoked
*
*/
template<class R, class P, VistaProperty::ePropType nPropType = VistaProperty::PROPT_STRING>
class TVistaDisplayEntityParentPropertyGet
 : public IVistaPropertyGetFunctor
{
public:
	/**
	* This constructor takes all the necessary parameters for creating a
	* functor which gets a single property from a IVistaReflectionable
	*
	* @param	sPropName	string encoding of property's name (i.e. key)
	* @param	sClassType	reflectionable's type name as encoded in the base type list
	* @param	pfm			pointer to the reflectionable's get method to be used
	*/
	TVistaDisplayEntityParentPropertyGet(const std::string &sPropName,
										 const std::string &sClassType,
										 R (P::*pfm)() const,
										 const std::string &sDescription = "")
	: IVistaPropertyGetFunctor(sPropName, sClassType, sDescription),
	m_pfn(pfm)
	{

	}
	~TVistaDisplayEntityParentPropertyGet()
	{
	}

	virtual bool operator()(const IVistaPropertyAwareable &rObj,
							VistaProperty &rProp) const
	{
		P *parent = static_cast<P*>(
			static_cast<const VistaDisplayEntity::IVistaDisplayEntityProperties&>(rObj).GetParent() );
		R rTmp = (parent->*m_pfn)();
		rProp.SetValue( VistaConversion::ToString( rTmp ));
		rProp.SetPropertyType(nPropType);
		return !rProp.GetIsNilProperty();
	}

	protected:
	R (P::*m_pfn)() const;
	private:
	TVistaDisplayEntityParentPropertyGet() {}
	TVistaDisplayEntityParentPropertyGet(const std::string &sPropName,
										 const std::string &sClassType) {}
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // !defined(_VISTADISPLAYENTITY_H)
