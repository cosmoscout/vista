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


#ifndef _VISTAREFERENCEFRAME_H
#define _VISTAREFERENCEFRAME_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaBaseConfig.h"

#include "VistaMathBasics.h"
#include "VistaVector3D.h"
#include "VistaQuaternion.h"
#include "VistaTransformMatrix.h"

#include <cmath>
#include <iostream>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * Represents a local coordinate system by storing the translation,
 * rotation and (uniform) scale. The resulting transformation matrix
 * can be retrieved and is calculated only when needed. The class also
 * offers convenience functions to transform positions, orientations
 * or whole transformation matrices from/to this local coordinate
 * system.
 */
class VISTABASEAPI VistaReferenceFrame
{
public:
	VistaReferenceFrame();
	VistaReferenceFrame( const VistaVector3D& v3RefTranslation,
						  const VistaQuaternion& qRefRotation,
						  const float fScale = 1.0f );
	virtual ~VistaReferenceFrame();

	virtual bool SetTranslation( const VistaVector3D &v3RefTranslation );
	bool GetTranslation( VistaVector3D &v3RefTranslation ) const;
	VistaVector3D GetTranslation() const;
	const VistaVector3D& GetTranslationConstRef() const;

	virtual bool SetRotation(const VistaQuaternion &refRotation);
	bool GetRotation(VistaQuaternion &refRotation) const;
	VistaQuaternion GetRotation() const;
	const VistaQuaternion& GetRotationConstRef() const;

	virtual bool SetScale(float fScale);
	float GetScale() const;

	/**
	 * Transform positions and orientations from this coordinate frame to its
	 * parent system.
	 */
	VistaVector3D TransformPositionFromFrame( const VistaVector3D& v3Position ) const;
	VistaQuaternion TransformOrientationFromFrame( const VistaQuaternion& qOrientation ) const;
	bool TransformFromFrame(VistaVector3D& v3Position,
							VistaQuaternion& qOrientation) const;
	VistaTransformMatrix TransformMatrixFromFrame( const VistaTransformMatrix& matMatrix );

	/**
	 * Transform positions and orientations to this coordinate frame.
	 */
	VistaVector3D TransformPositionToFrame( const VistaVector3D& v3Position ) const;
	VistaQuaternion TransformOrientationToFrame( const VistaQuaternion& qOrientation ) const;
	bool TransformToFrame( VistaVector3D& v3Position,
							VistaQuaternion& qOrientation ) const;
	VistaTransformMatrix TransformMatrixToFrame(const VistaTransformMatrix& matMatrix );

	/**
	 * Get the transformations to and from the reference frame as matrices.
	 * As the results are cached, the following methods are not defined "const".
	 */
	inline bool GetMatrix( VistaTransformMatrix& matTarget );
	inline VistaTransformMatrix GetMatrix();
	inline const VistaTransformMatrix& GetMatrixConstRef();

	inline bool GetMatrixInverse( VistaTransformMatrix& matTarget );
	inline VistaTransformMatrix GetMatrixInverse();
	inline const VistaTransformMatrix& GetMatrixInverseConstRef();


protected:
	void RefreshXformMatrix();
	void RefreshXformMatrixInverse();

	VistaVector3D			m_v3Translation;
	VistaQuaternion			m_qRotation;
	float					m_fScale;

	VistaTransformMatrix	m_matXform;
	VistaTransformMatrix	m_matformInverse;
	bool                    m_bRefreshXform;
	bool                    m_bRefreshXformInv;
};

inline std::ostream& operator<<( std::ostream& oStream, const VistaReferenceFrame& matTransform );

/*============================================================================*/
/* INLINE IMPLEMENTATIONS */
/*============================================================================*/


inline VistaReferenceFrame::VistaReferenceFrame()
: m_fScale(1.0f),
  m_bRefreshXform(true),
  m_bRefreshXformInv(true)
{
}

inline VistaReferenceFrame::VistaReferenceFrame(const VistaVector3D &refTranslation,
										   const VistaQuaternion &refRotation,
										   float fScale)
: m_v3Translation(refTranslation),
  m_qRotation(refRotation),
  m_fScale(fScale),
  m_bRefreshXform(true),
  m_bRefreshXformInv(true)
{
}

inline VistaReferenceFrame::~VistaReferenceFrame()
{
}

inline void VistaReferenceFrame::RefreshXformMatrix()
{
	m_matXform = VistaTransformMatrix(m_qRotation) * VistaTransformMatrix(m_fScale);

	// avoid creating an additional, translational matrix by just setting
	// the last column to the desired translation
	m_matXform.SetTranslation( m_v3Translation );

	// well, you never know...
	m_matXform[3][3] = 1.0f;

	m_bRefreshXform = false;
}

inline void VistaReferenceFrame::RefreshXformMatrixInverse()
{
	if (m_bRefreshXform)
		RefreshXformMatrix();

	m_matXform.GetInverted(m_matformInverse);

	m_bRefreshXformInv = false;
}


inline bool VistaReferenceFrame::SetRotation(const VistaQuaternion &refRotation)
{
	m_qRotation = refRotation;
	m_bRefreshXform = m_bRefreshXformInv = true;
	return true;
}

inline bool VistaReferenceFrame::SetTranslation(const VistaVector3D &refTranslation)
{
	m_v3Translation = refTranslation;
	m_bRefreshXform = m_bRefreshXformInv = true;
	return true;
}

inline bool VistaReferenceFrame::SetScale(float fScale)
{
	if (fScale > 0.0f)
	{
		m_fScale = fScale;
		m_bRefreshXform = m_bRefreshXformInv = true;
		return true;
	}
	else
	{
		return false;
	}
}

inline bool VistaReferenceFrame::GetTranslation(VistaVector3D& v3Translation) const
{
	v3Translation = m_v3Translation;
	return true;
}

inline VistaVector3D VistaReferenceFrame::GetTranslation() const
{
	return m_v3Translation;
}

inline const VistaVector3D& VistaReferenceFrame::GetTranslationConstRef() const
{
	return m_v3Translation;
}

inline bool VistaReferenceFrame::GetRotation(VistaQuaternion& qRotation) const
{
	qRotation = m_qRotation;
	return true;
}

inline VistaQuaternion VistaReferenceFrame::GetRotation() const
{
	return m_qRotation;
}

inline const VistaQuaternion& VistaReferenceFrame::GetRotationConstRef() const
{
	return m_qRotation;
}


inline float VistaReferenceFrame::GetScale() const
{
	return m_fScale;
}

inline VistaVector3D VistaReferenceFrame::TransformPositionFromFrame(const VistaVector3D& v3Position) const
{
	VistaVector3D v3Result = v3Position * m_fScale;
	v3Result = m_qRotation.Rotate(v3Result);
	v3Result += m_v3Translation;
	return v3Result;
}

inline VistaQuaternion VistaReferenceFrame::TransformOrientationFromFrame(const VistaQuaternion& qOrientation) const
{
	return m_qRotation * qOrientation;
}

inline bool VistaReferenceFrame::TransformFromFrame(VistaVector3D& v3Position,
											   VistaQuaternion& qOrientation) const
{
	v3Position = TransformPositionFromFrame(v3Position);
	qOrientation = TransformOrientationFromFrame(qOrientation);
	return true;
}

inline VistaTransformMatrix VistaReferenceFrame::TransformMatrixFromFrame(const VistaTransformMatrix& matTransform)
{
	if (m_bRefreshXform)
	{
		RefreshXformMatrix();
	}

	return m_matXform * matTransform;
}

inline VistaVector3D VistaReferenceFrame::TransformPositionToFrame(const VistaVector3D& v3Position) const
{
	VistaVector3D v3Result = v3Position - m_v3Translation;
	v3Result = m_qRotation.GetComplexConjugated().Rotate(v3Result);
	v3Result *= 1.0f/m_fScale;
	return v3Result;
}

inline VistaQuaternion VistaReferenceFrame::TransformOrientationToFrame(const VistaQuaternion& qOrientation) const
{
	return m_qRotation.GetComplexConjugated() * qOrientation;
}

inline bool VistaReferenceFrame::TransformToFrame(VistaVector3D &refPosition,
													VistaQuaternion &refOrientation) const
{
	refPosition = TransformPositionToFrame(refPosition);
	refOrientation = TransformOrientationToFrame(refOrientation);
	return true;
}

inline VistaTransformMatrix VistaReferenceFrame::TransformMatrixToFrame(const VistaTransformMatrix& matTransform)
{
	if (m_bRefreshXformInv)
	{
		RefreshXformMatrixInverse();
	}

	return m_matformInverse * matTransform;
}

inline VistaTransformMatrix VistaReferenceFrame::GetMatrix()
{
	if (m_bRefreshXform)
		RefreshXformMatrix();
	return m_matXform;
}

inline const VistaTransformMatrix& VistaReferenceFrame::GetMatrixConstRef()
{
	if (m_bRefreshXform)
		RefreshXformMatrix();
	return m_matXform;
}

inline bool VistaReferenceFrame::GetMatrix(VistaTransformMatrix& matTarget)
{
	if (m_bRefreshXform)
		RefreshXformMatrix();
	matTarget = m_matXform;
	return true;
}

inline VistaTransformMatrix VistaReferenceFrame::GetMatrixInverse()
{
	if (m_bRefreshXformInv)
		RefreshXformMatrixInverse();
	return m_matformInverse;
}

inline const VistaTransformMatrix& VistaReferenceFrame::GetMatrixInverseConstRef()
{
	if (m_bRefreshXformInv)
		RefreshXformMatrixInverse();
	return m_matformInverse;
}

inline bool VistaReferenceFrame::GetMatrixInverse(VistaTransformMatrix& matTraget )
{
	if (m_bRefreshXformInv)
		RefreshXformMatrixInverse();
	matTraget = m_matformInverse;
	return true;
}


inline std::ostream& operator<<( std::ostream& oStream, const VistaReferenceFrame& oRefFrame )
{
	oStream << "Translation: " << oRefFrame.GetTranslation() << std::endl;
	oStream << "Rotation:    " << oRefFrame.GetRotation() << std::endl;
	oStream << "Scale:       " << oRefFrame.GetScale() << std::endl;

	return oStream;
}


#endif //_VISTAREFERENCEFRAME_H
