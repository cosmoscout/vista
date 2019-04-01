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


#include "VistaShallowDriver.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

class VistaShallowDriverCreation : public IVistaDriverCreationMethod
{
public:
	VistaShallowDriverCreation()
	: IVistaDriverCreationMethod(NULL)
	{

	}

	virtual IVistaDeviceDriver *CreateDriver()
	{
		return new VistaShallowDriver(this);
	}
};


namespace
{
	VistaShallowDriverCreation *SpFactory = NULL;
}

IVistaDriverCreationMethod *VistaShallowDriver::GetDriverFactoryMethod()
{
	if(SpFactory == NULL)
		SpFactory = new VistaShallowDriverCreation;
	return SpFactory;
}


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaShallowDriver::VistaShallowDriver(IVistaDriverCreationMethod *crm)
: IVistaDeviceDriver(crm)
{
	SetUpdateType( IVistaDeviceDriver::UPDATE_EXPLICIT_POLL );
}

VistaShallowDriver::~VistaShallowDriver()
{
}

bool VistaShallowDriver::DoSensorUpdate(VistaType::microtime dTs)
{
	return true;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


