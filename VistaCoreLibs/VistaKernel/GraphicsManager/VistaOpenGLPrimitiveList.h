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


#ifndef _VISTAOPENGLPRIMITIVELIST_H
#define _VISTAOPENGLPRIMITIVELIST_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>
#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaMath/VistaBoundingBox.h>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaNode;
class VistaGroupNode;
class VistaSceneGraph;
class VistaOpenGLNode;
class IVistaNode;
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaOpenGLPrimitiveList
{
public:
	class VISTAKERNELAPI COpenGLPrimitiveList : public IVistaOpenGLDraw
	{
	public:
		COpenGLPrimitiveList();
		virtual ~COpenGLPrimitiveList();

		// IVistaOpenGLDraw interface
		virtual bool Do();
		virtual bool GetBoundingBox(VistaBoundingBox &bb);

		void SetVertices(const std::vector<float> &vecPoints);
		bool GetVertices(std::vector<float> &vecPoints) const;

		bool GetIsUsingColorVector() const { return m_bIsUsingColorVector; }
		void SetIsUsingColorVector(bool val) { m_bIsUsingColorVector = val; }

		std::vector<VistaColor>& GetVertexColors() {return m_vecColors;};

		unsigned int GetPrimitiveType() const;
		void  SetPrimitiveType(unsigned int eMd);

		VistaColor GetColor() const;
		void           SetColor(const VistaColor &mat);

		bool GetUseLighting() const;
		void SetUseLighting(bool val);

		float GetGlPrimitiveSize() const { return m_fGlPrimitiveSize; }
		void SetGlPrimitiveSize(float val) { m_fGlPrimitiveSize = val; }

	protected:
		bool UpdateDisplayList();
		void CalculateNormal( float normal[3], unsigned int i, int param2, int param3 );
	private:
		int m_iDispId;
		bool m_bDlistDirty;

		bool m_bIsUsingColorVector;
		std::vector<float> m_vecPoints;
		std::vector<VistaColor> m_vecColors;
		VistaBoundingBox   m_oBBox;
		unsigned int m_eGLPrimitiveType;
		VistaColor     m_oColor;
		bool			m_bUseLighting;
		float m_fGlPrimitiveSize;
	};
public:

	enum ePrimitiveType
	{
		POINTS=0,
		LINES,
		LINE_LOOP,
		LINE_STRIP,
		TRIANGLES,
		TRIANGLE_STRIP,
		TRIANGLE_FAN,
		QUADS,
		QUAD_STRIP,
		POLYGON
	};

	VistaOpenGLPrimitiveList(VistaSceneGraph* pVistaSceneGraph, VistaGroupNode* pParent);

	virtual ~VistaOpenGLPrimitiveList();

	IVistaNode* GetVistaNode() const;

	void SetVertices(const std::vector<float> &vecPoints);
	bool GetVertices(std::vector<float> &vecPoints) const;

	bool GetIsUsingColorVector() const { return m_pDrawInterface->GetIsUsingColorVector (); }
	void SetIsUsingColorVector(bool val) { m_pDrawInterface->SetIsUsingColorVector (val); }

	std::vector<VistaColor>& GetVertexColors() {return m_pDrawInterface->GetVertexColors();};

	bool GetRemoveFromSGOnDelete() const;
	void SetRemoveFromSGOnDelete(bool bRemove);

	bool SetUseLighting(bool bUseLighting);
	bool SetColor(const VistaColor &oMat) const;
	bool SetColor (const VistaColor  & color);
	bool SetPrimitiveType(const ePrimitiveType & primitiveType);

	float GetGlPrimitiveSize() const { return m_pDrawInterface->GetGlPrimitiveSize (); }
	void SetGlPrimitiveSize(float val) { m_pDrawInterface->SetGlPrimitiveSize (val); }
protected:
	VistaOpenGLNode		*m_pOglNode;
	COpenGLPrimitiveList		*m_pDrawInterface;
private:	
	/** default is true */
	bool					m_bRemoveFromSGOnDelete;
	ePrimitiveType			m_ePrimitiveType;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTALINE_H