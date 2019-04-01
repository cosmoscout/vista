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


#if !defined(_VISTAPROJECTION_H)
#define _VISTAPROJECTION_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>


#include "VistaDisplayEntity.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaViewport;
class IVistaDisplayBridge;
class VistaVector3D;
class VistaQuaternion;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * VistaProjection is an abstraction for a projection. (Hm, that was easy, right?!)
 */
class VISTAKERNELAPI VistaProjection  : public VistaDisplayEntity
{
	friend class IVistaDisplayBridge;

public:
	virtual ~VistaProjection();

	VistaViewport *GetViewport() const;
	std::string GetViewportName() const;

	virtual void Debug ( std::ostream & out ) const;

	/**
	 * Set/get projection properties. The following keys are understood:
	 *
	 *  PROJ_PLANE_MIDPOINT     -   [list of floats, 3 items]
	 *  PROJ_PLANE_NORMAL       -   [list of floats, 3 items]
	 *  PROJ_PLANE_UP           -   [list of floats, 3 items]
	 *  PROJ_PLANE_EXTENTS      -   [list of doubles, 4 items]
	 *  CLIPPING_RANGE          -   [list of doubles, 2 items]
	 *  STEREO_MODE             -   ["MONO"|"LEFT_EYE"|"RIGHT_EYE"|"FULL_STEREO"]
	 *  VIEWPORT_NAME           -   [string] (read only)
	 */

	class VISTAKERNELAPI VistaProjectionProperties : public IVistaDisplayEntityProperties
	{
	public:

		enum {
			MSG_PROJ_PLANE_MIDPOINT_CHANGE = IVistaDisplayEntityProperties::MSG_LAST,
			MSG_PROJ_PLANE_NORMAL_CHANGE,
			MSG_PROJ_PLANE_UP_CHANGE,
			MSG_PROJ_PLANE_EXTENTS_CHANGE,
			MSG_CLIPPING_RANGE_CHANGE,
			MSG_STEREO_MODE_CHANGE,
			MSG_LAST
		};

		enum EStereoMode
		{
			SM_MONO,
			SM_LEFT_EYE,
			SM_RIGHT_EYE,
			SM_FULL_STEREO
		};

		bool SetName(const std::string &sName);

		bool GetProjPlaneMidpoint(float &x, float &y, float &z) const;
		bool SetProjPlaneMidpoint(const float x, const float y, const float z);

		bool GetProjPlaneNormal(float &x, float &y, float &z) const;
		bool SetProjPlaneNormal(const float x, const float y, const float z);

		bool GetProjPlaneUp(float &x, float &y, float &z) const;
		bool SetProjPlaneUp(const float x, const float y, const float z);

		bool GetProjPlaneExtents(double &dLeft, double &dRight, double &dBottom, double &dTop) const;
		bool SetProjPlaneExtents(const double dLeft, const double dRight, const double dBottom, const double dTop);

		bool GetClippingRange(double &dNear, double &dFar) const;
		bool SetClippingRange(const double dNear, const double dFar);

		int  GetStereoMode() const;
		bool SetStereoMode(int iMode);
		std::string GetStereoModeString() const;
		bool        SetStereoModeString(const std::string &strMode);

		/* convenience API begin */
		/**
		 * Set/get the projection plane.
		 */
		bool SetProjectionPlane(const VistaVector3D &v3MidPoint,
								const VistaVector3D &v3NormalVector,
								const VistaVector3D &v3UpVector);

		void GetProjectionPlane(VistaVector3D &v3MidPoint,
								VistaVector3D &v3NormalVector,
								VistaVector3D &v3UpVector) const;
		/* convenience API end */

		virtual std::string GetReflectionableType() const;

	protected:
		virtual int AddToBaseTypeList(std::list<std::string> &rBtList) const;
		virtual ~VistaProjectionProperties();

	private:
		VistaProjectionProperties(VistaProjection *,
								   IVistaDisplayBridge *);
		friend class VistaProjection;
	};

	VistaProjectionProperties *GetProjectionProperties() const;

protected:
	virtual IVistaDisplayEntityProperties *CreateProperties();
	VistaProjection    (VistaViewport *pViewport,
						 IVistaDisplayEntityData *pData,
						 IVistaDisplayBridge *pBridge);
	VistaViewport      *m_pViewport;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // !defined(_VISTAPROJECTION_H)
