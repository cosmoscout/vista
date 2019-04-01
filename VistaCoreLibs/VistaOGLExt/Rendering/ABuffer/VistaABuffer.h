/*============================================================================*/
/*                                 VistaFlowLib                               */
/*               Copyright (c) 1998-2016 RWTH Aachen University               */
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


#ifndef VISTA_A_BUFFER_H
#define VISTA_A_BUFFER_H
/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "../../VistaOGLExtConfig.h"

#include <GL/glew.h>
#include <string>
#include <vector>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaGLSLShader;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAOGLEXTAPI IVistaABuffer
{
public: 
	/**************************************************************************/
	/* CONSTRUCTORS / DESTRUCTOR                                              */
	/**************************************************************************/
	IVistaABuffer();
	virtual ~IVistaABuffer();

	/**************************************************************************/
	/* PUBLIC INTERFACE                                                       */
	/**************************************************************************/
	/**
	 * This method will initializes the A-Buffer.
	 */
	virtual bool Init();

	/**
	 * This method will clear the A-Buffer. All data that is currently stored 
	 * in the A-Buffer will be deleted, to make space for new data.
	 */
	virtual bool ClearABuffer();

	/**
	 * 
	 */
	virtual void Barrier();

	virtual bool SetABufferSize( unsigned int uiWidth, unsigned int uiHeight );
	virtual bool SetFragmenstPerPixel( unsigned int uiMaxNumFrag );

	/**
	 * Adds a new data field to the A-Buffer.
	 * A data field contains the date that is associated with a fragment.
	 * For example: a data field can be used to store the color of a fragment.
	 *
	 * nSizeInByte specifies the size in Byte for one element of the data field.
	 * For example: if you want to store the color of a fragment as a vector of 
	 * four floats, nSizeInByte should be 4*sizeof(float).
	 * If you want to store the color of a fragment as a single integer,
	 * nSizeInByte should be sizeof(int).
	 *
	 * strName specifies the name of the uniform variable that is used to access
	 * the data field.
	 */
	virtual bool AddDataField( unsigned int nSizeInByte, const std::string& strName );

	/**
	 * returns a new VistaGLSLShader that is already partially initialized.
	 * the Shader returned by this method will already provide function to 
	 * access the A-Buffer.
	 * This method will not take ownership of the Shaders created by this method,
	 * therefor you have to delete the Shaders create by this method by your shelve.
	 */
	virtual VistaGLSLShader* CreateShaderPrototype() const = 0;

	/**
	 * All shader that access the A-Buffer must be registered using this method.
	 * Otherwise the required uniform variables will not be set correct.
	 * Shaders should be registered AFTER, they have successfully been compiled and linked.
	 */
	virtual bool RegisterShader( VistaGLSLShader* pShader );
	virtual bool DeregisterShader( VistaGLSLShader* pShader );

protected:
	struct DataField
	{
		unsigned int	m_nSizeInByte;
		std::string 	m_strName;

		GLuint		m_uiBuffer;
		GLuint64EXT	m_addrBuffer;
	};

	virtual bool InitShader() = 0;
	virtual bool InitBuffer() = 0;
	virtual void ResizePerPixelBuffer() = 0;

	virtual void ResizeDataFileds();
	virtual void ResizeBuffer(	GLuint		 uiBufferID, 
								GLuint64EXT& uiBufferAdress, 
								unsigned int iSize );

	virtual void UpdateUniforms();
	virtual void AssignUniforms( VistaGLSLShader* pShader ) = 0;

protected:
	VistaGLSLShader*	m_pClearShader;

	std::vector<DataField>			m_vecDataFileds;
	std::vector<VistaGLSLShader*>	m_vecShader;

	unsigned int	m_uiWidth;
	unsigned int	m_uiHeight;
	unsigned int	m_uiFragmenstPerPixel;
	unsigned int	m_uiBufferSize;

	bool m_bIsInitialized;
};
#endif // Include guard.
/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
