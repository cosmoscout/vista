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


#ifndef _VISTASTREAMMANAGER_H
#define _VISTASTREAMMANAGER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaBaseConfig.h"
#include "VistaBaseTypes.h"

#include <VistaAspects/VistaUncopyable.h>

#include <string>
#include <map>
#include <vector>
#include <ostream>
#include <iostream>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaTimer;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


class VISTABASEAPI VistaStreamManager
{
	VISTA_UNCOPYABLE( VistaStreamManager );
public:	
	VistaStreamManager();
	virtual ~VistaStreamManager();

	/**
	 * Interface for the StreamManager to access specific data from the system
	 * Needs to be Implemented and set in order to make some functions available
	 */
	class VISTABASEAPI IInfoInterface
	{
	protected:
		IInfoInterface() {}
	public:
		virtual ~IInfoInterface() {}

		virtual VistaType::systemtime	GetFrameClock() const	= 0;
		virtual std::string				GetNodeName() const		= 0;
		virtual unsigned int			GetFrameCount() const	= 0;
		virtual float					GetFrameRate() const	= 0;
	};
	void SetInfoInterface( IInfoInterface* pInfoInterface );
	IInfoInterface* GetInfoInterface() const;


	/**
	 * Adds a new Stream with the given name
	 * If the name is already used, it may be replaced with the new stream by
	 * setting bReplaceExistingStream to true.
	 * If bManageDeletion is true, the stream will be closed/deleted with the
	 * StreamManager instance
	 * @return true if stream was successfully added
	 */
	virtual bool AddStream( const std::string& sName,
					std::ostream& oStream,
					bool bManageDeletion = false,
					bool bReplaceExistingStream = false );
	/**
	 * Removes the stream with the specified name, and deletes it if
	 * bAlwaysDelete == true OR ManageDeletion was set for the stream
	 * @return true if Stream was removed, false if no stream of this name exists.
	 */
	virtual bool RemoveStream( const std::string& sName,
							const bool bAlwaysDelete = false );
	/** 
	 * returns the stream with the given name. 
	 * If it does not exist, cout is returned instead
	 */
	std::ostream& GetStream( const std::string& sName );

	bool GetHasStream( const std::string& sName );

	/**
	 * Creates a new filestream.
	 * The name consists of the specified filename (or the streamname,
	 * if the filename is ""), the current date (if bAddTimeStamp == true),
	 * the name of the Vista ClusterNode (if bAddNodeName == true and the
	 * InfoInterface has been set), as well as the FileExtension
	 * The stream is managed by the StreamManager, and 
	 */
	std::ostream* CreateNewLogFileStream( const std::string& sFileName,
						const std::string& sFileExtension = "log",
						const bool bManageDeletion = false,
						const bool bAddNodeName = true,
						const bool bAddTimeStamp = true,
						const bool bAppendToExistingFile = false );
	/**
	 * Creates a new filestream, and automatically adds it.
	 * Also manages deletion of the stream
	 */
	bool AddNewLogFileStream( const std::string& sStreamName,
						const std::string& sFileName = "",
						const std::string& sFileExtension = "log",
						const bool bAddNodeName = true,
						const bool bAddTimeStamp = true,
						const bool bAppendToExistingFile = false);
					
	
	enum INFO_LAYOUT_ELEMENT
	{
		LE_FRAMECLOCK = 0,
		LE_SYSTEMTIME,
		LE_RELATIVETIME,
		LE_DATE,
		LE_NODENAME,
		LE_FRAMECOUNT,
		LE_FRAMERATE,
		LE_COLON,
		LE_DASH,
		LE_PIPE,
		LE_SPACE,
		LE_TAB,
		LE_LINEBREAK,
	};
	/**
	 * Specifies the information that is printed by the info stream manipulator.
	 * @param vecLayout A vector containing the elements to be displayed, which can
	 *        be any number of INFO_LAYOUT_ELEMENT
	 */
	bool SetInfoLayout( const std::vector<INFO_LAYOUT_ELEMENT>& vecLayout );
	bool GetInfoLayout( std::vector<INFO_LAYOUT_ELEMENT>& vecLayout );
	std::vector<INFO_LAYOUT_ELEMENT>& GetInfoLayoutRef();
	void PrintInfo( std::ostream& oStream ) const;

	// some state info
	VistaType::systemtime GetSystemTime() const;
	VistaType::systemtime GetMicroTime() const;
	std::string GetDateString() const;
	//special state info only a available if own InfoInterface is set
	// return 0/"" if no InfoInterface is set
	VistaType::systemtime GetFrameClock() const;	
	std::string GetNodeName() const;
	unsigned int GetFrameCount() const;
	float GetFrameRate() const;

	/**
	 * Gets/Sets the default prefix to be printed before warning or error messages
	 */
	std::string GetErrorPrefix() const;
	void SetErrorPrefix( const std::string& sPrefix );
	std::string GetWarningPrefix() const;
	void SetWarningPrefix( const std::string& sPrefix );

	/**
	 * Interface for consistent indentation
	 */
	int GetIndentationLevel() const;
	void SetIndentationLevel( const int iLevel );
	void AddToIndentationLevel( const int iAdd = +1 );
	
	const std::string& GetIndentationPrefixString() const;
	void SetIndentationPrefixString( const std::string& sString );
	const std::string& GetIndentationPostfixString() const;
	void SetIndentationPostfixString( const std::string& sString );
	const std::string& GetIndentationLevelString() const;
	void SetIndentationLevelString( const std::string& sString );

	const std::string& GetIndentation() const;

protected:
	/** 
	 * This function is called if a stream is requested by GetStream(), but does not
	 * exist yet. Default implementation just returns the null stream
	 */
	virtual std::ostream& GetDefaultStream( const std::string& sName );
	void RebuildIndentationString();
	
protected:
	VistaTimer*								m_pTimer;
	std::map<std::string, std::ostream*>	m_mapStreams;
	std::vector<std::ostream*>				m_vecOwnStreams;
	std::vector<INFO_LAYOUT_ELEMENT>		m_vecInfoLayout;
	IInfoInterface*							m_pInfo;

	std::string								m_sErrorPrefix;
	std::string								m_sWarningPrefix;
	int										m_iIndentationLevel;
	std::string								m_sIndentationLevelString;
	std::string								m_sIndentationPrefixString;
	std::string								m_sIndentationPostfixString;
	std::string								m_sIndentation;
};

#endif // _VISTASTREAMMANAGER_H

