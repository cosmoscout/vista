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


#ifndef _VISTADRIVERCONNECTIONASPECT_H
#define _VISTADRIVERCONNECTIONASPECT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriversConfig.h>

#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <vector>
#include <list>
#include <string>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaConnection;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Some drivers use connections for their communication with the device.
 * If so, they can export a connection aspect that defines the number of
 * connections needed, and the role of each connection for the driver.
 * Valid roles are driver specific, but can be queried through the aspect
 * interface.
 * Connections are organized by assigning a driver role to an index. The
 * driver role is a human readable string that can be assigned dynamically and
 * is driver specific. Connections have attach and detach sequences which
 * are calls right after initialization and when the connection is about to
 * be closed respectively. They may be set on a per-connection level. This
 * may be useful, i.e., when some driver needs a token sent after the connection
 * is opened (e.g. an initialization command) or something similar in order
 * to really shut down (e.g. an explicit quit command).
 * The connection aspect also defines some macro like methods that may be used
 * to simplify the implementation of a driver (such as SendCommand() or
 * GetCommand().
 */
class VISTADEVICEDRIVERSAPI VistaDriverConnectionAspect : public IVistaDeviceDriver::IVistaDeviceDriverAspect
{
public:
	/**
	 * A connection sequence is called on the instance of a connection :)
	 * shortly said: it is a callback that is called on behalf of the
	 * management code that can be customized to user-specific demands.
	 * For example, if you need to set the blocking behavior, block size
	 * or have to check whether a device has the correct version...
	 * this is a possibility.
	 */
	class VISTADEVICEDRIVERSAPI IVistaConnectionSequence
	{
	public:
		virtual ~IVistaConnectionSequence() {}
		/**
		 * is called with the connection to process.
		 * @return false if the connection is to be closed again.
		 */
		virtual bool operator()(VistaConnection *) = 0;
	};


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// CONSTRUCTION/DESTRUCTION
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	VistaDriverConnectionAspect();
	virtual ~VistaDriverConnectionAspect();

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// CONNECTION MANAGEMENT
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/**
	 * retrieve a connection by index.
	 * @param the index to query.
	 * @return NULL when no connection fills the entry
	 * @see SetConnection()
	 */
	VistaConnection *GetConnection(unsigned int nIndex = 0) const;

	/**
	 * set a connection for an index. This API is twofold
	   - you can declare an index, but do not have a connection.
	     In that case, set nIndex >= 0 and pConnection == NULL,
	     bCanBeReplaced should be set to true and bForce to false
	   - you can predefine a connection for a given index, in that
	     case you set nIndex >= 0, pConnection != NULL.
	     Setting bCanBeReplaced to true means that that user code
	     can still decide and override the connection.
	 * @param nIndex the index to set
	 * @param pConnection the connection to be set (can be NULL,
	          in which case an index is reserved but not assigned)
	 * @param bCanBeReplaced flag to indicate whether a user can
	          call to set a connection and replace the old one,
	          set it to false for connections that you do not want
	          to be replaced by user code
	 * @param bForce if a slot is already set, SetConnection() will
	          try to overwrite the value if bForce is set to true.
	          It still observes the bCanBeReplaced. So forcing of
	          non-replacable connections will not work.
	 */
	enum ReplaceBehavior
	{
		ALLOW_CONNECTION_REPLACEMENT, /**< allows to override / replace a connection that was given with SetConnection */
		FORBID_CONNECTION_REPLACEMENT /**< do not allow attempts to replace a connection if it was once set */
	};

	enum ForceAlreadySetBehavior
	{
		REPLACE_ALSO_WHEN_ALREADY_SET, /**< second level switch: if connection replacement is allowed, replace even when it was already set */
		REPLACE_ONLY_WHEN_NOT_SET /**< allow a replacement only when a connection slot was nullptr before */
	};

	enum ConnectionCollectBehavior
	{
		COLLECT_ON_ASPECT_DELETE, /**< aspect will call delete on the registered connections when itself is deleted */
		DO_NOT_COLLECT_ON_ASPECT_DELETE /**< aspect will just forget all connection pointers and not try to claim them */
	};



	void              SetConnection(unsigned int nIndex,
									VistaConnection *pConnection,
									const std::string &sRoleName,
									ReplaceBehavior bCanBeReplaced,
									ForceAlreadySetBehavior bForce = REPLACE_ONLY_WHEN_NOT_SET,
									ConnectionCollectBehavior bCollectOnAspectDelete = COLLECT_ON_ASPECT_DELETE );

	/**
	 * is called upon detach. will eventually lead to calling the detach
	 * sequence (which can be user definable)
	 * @see IVistaConnectionSequence()
	 * @see SetDetachSequence()
	 * @param nIndex the connection index to be detached
	 */
	void         DetachConnection(unsigned int nIndex);

	/**
	 * Index slots can have a user defined ROLE (for example 'INCOMING' or
	 * 'DATACHANNEL'). This method tries to evaluate to the index for a
	 * specific role.
	 * @return the index to query for the connection of role sToken or -1
	 * @param sToken the user symbol to query the index for
	 */
	unsigned int GetIndexForRole(const std::string &sToken) const;

	/**
	 * retrieve all registered roles.
	 * @return the number of entries in liTokens after the call
	 * @param liTokens a list of strings to store the roles to.
	 */
	unsigned int EnumerateRoles(std::list<std::string> &liTokens) const;


	/**
	 * ok... one can call this a quick hack ;)
	 * The update connection is usually unique and has a specific index, set
	 * by the driver. This method will try to return the slot to talk to.
	 * The update connection index is used by the connection updater to
	 * wait for new and incoming data. If there is none, the device will
	 * not be polled
	 * @return the update connection index, or -1 for none available
	 * @see SetUpdateConnectionIndex()
	 * @todo check if this is still meaningful, resp. clearify on the side-effect
	 */
	unsigned int GetUpdateConnectionIndex() const;

	/**
	 * Usually called by the driver during constructio/connect: which index is
	 * responsible to get new data.
	 */
	void         SetUpdateConnectionIndex(unsigned int nIndex);


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ATTACH/DETACH SEMANTICS
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/**
	 * Sets an attach sequence for a connection at a given index.
	 * Defines a user-determined callback upon a successful attachment
	 * of a connection. If the nConIdx was not set with a connection, the index
	 * is internally created and set to NULL (reserving an index).
	 * @return true iff the sequence was set, false else
	 * @see SetDetachSequence()
	 */
	bool SetAttachSequence(unsigned int nConIdx,
						   IVistaConnectionSequence *);

	/**
	 * return an attach sequence for a given index.
	 * @return NULL iff no sequence was set or the index is non-existent
	 */
	IVistaConnectionSequence *GetAttachSequence(unsigned int nConIdx) const;

	/**
	 * @see SetAttachSequence()
	 */
	bool SetDetachSequence(unsigned int nConIdx,
						   IVistaConnectionSequence *);
	/**
	 * @return NULL iff no sequence was set or the index is non-existent
	 */
	IVistaConnectionSequence *GetDetachSequence(unsigned int nConIdx) const;


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// CONVINIENCE API: reading and writing to connections
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/**
	 * send a number of bytes over a connection at a given index. Once the send
	 * was done, wait a specific amount of time in msecs.
	 * The method does not assume anything about the blocking state of the connection,
	 * so for non-blocking connections, the method might return false. In case you
	 * do not want that: take care of the blocking state before calling this method,
	 * or use the connection directly. The post-send delay is handy for some slow devices,
	 * for example old electro magnetic trackers which need some msecs to
	 * process the incoming requests.
	 * @param nIndex the connection slot to use
	 * @param pcCmd the byte-buffer to be written
	 * @param unLength the number of bytes to write
	 * @param nPostSendDelay after successful send: wait in msecs
	 * @return false when
	          - slot was empty
	          - unLength was illegal
	          - the connection broke during send
	          - something was send, but less than unLength
	 */
	bool SendCommand(unsigned int nIndex,
					 const void *pcCmd,
					 unsigned int unLength,
					 unsigned int nPostSendDelay = 0);

	/**
	 * retrieve a number of bytes from a connection. The method does not assume
	 * anything about the blocking state, but is able to identify a timeout on get.
	 * The caller is responsible for a buffer size that is big enough.
	 * For timeouts to work, the connection should be blocking.
	 * @param nIndex the connection slot index
	 * @param pcBuffer the memory to write to, should be nMaxLength in size
	 * @param nMaxLength the maximum number of bytes to receive, ~0 for as many
	          bytes as come along (or until timeout)
	 * @param nTimeout the number of msecs to wait before return, 0 for polling or
	          blocking (depending on the blocking scheme of the connection)
	 * @param bTimeoutMark in case the read is timed out, the bool bTimeoutMark
	          is set to true; this is a pointer, as it can be optional, but
	          please to not use a new to allocate it ;)
	 * @return the number of bytes read or -1 in case of failure.
	 */
	int  GetCommand(unsigned int nIndex,
					void *pcBuffer,
					unsigned int nMaxLength = ~0,
					int nTimeout = 0,
					bool *bTimeoutMark = NULL);

	/**
	 * retrieve an end-mark terminated command from the connection, for example a
	 * string with a newline at its end.
	 * Users should take care of proper buffer sizes.
	 * @param nIndex the connection slot index
	 * @param pcBuffer the memory to write to
	 * @param nMaxLength the max. number of bytes to read, ~0 for as many as come along.
	 * @param cMark the endline mark to look for
	 * @param nTimeout how long to wait (msecs) until a timeout comes along
	 * @param bTimeoutMark in case the blocking connection times out, this flag is
	          written to true, false else.
	 * @return the number of bytes read or -1 in case of failure
	 */
	int  GetTerminatedCommand(unsigned int nIndex,
							  void *pcBuffer,
							  unsigned int nMaxLength = ~0,
							  char cMark = '\n',
							  int nTimeout = 0,
							  bool *bTimeoutMark = NULL);

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// STATE MANAGEMENT -- convenience api
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/**
	 * get whether a connection is set to blocking.
	 * Connection slots that are not set are not blocked.
	 * @return the blocking state of the connection at index nIndex
	 */
	bool GetIsBlocking(unsigned int nIndex) const;

	/**
	 * set the blocking state for a connection at a given index.
	 * @return true iff the connection at nIndex has the blocking state bBlocking,
	           false else
	 */
	bool SetIsBlocking(unsigned int nIndex, bool bBlocking);

	// #########################################
	// OVERWRITE IN SUBCLASSES
	// #########################################
	static int  GetAspectId();
	static void SetAspectId(int);

protected:
	friend class ConnectionHlp;
	bool AttachConnection(VistaConnection *);
	bool DetachConnection(VistaConnection *);
private:
	static int m_nAspectId;

	class ConnectionHlp
	{
	public:
		ConnectionHlp(
			VistaDriverConnectionAspect *pParent,
			VistaConnection *pCon,
			IVistaConnectionSequence *pAttach = NULL,
			IVistaConnectionSequence *pDetach = NULL,
			ReplaceBehavior bCanBeReplaced = ALLOW_CONNECTION_REPLACEMENT,
			ConnectionCollectBehavior bCollect = COLLECT_ON_ASPECT_DELETE );

		~ConnectionHlp();

		VistaConnection *m_pConnection;
		IVistaConnectionSequence *m_pAttachSequence,
								 *m_pDetachSequence;
		VistaDriverConnectionAspect *m_pParent;
		std::string m_strName;
		ReplaceBehavior m_bCanBeReplaced;
		ConnectionCollectBehavior m_bCollect;
	};

	typedef std::vector<ConnectionHlp*> CONVEC;
	CONVEC m_vecConnections;
	unsigned int m_nUpdConnectionIndex;

	ConnectionHlp *GetHlpByConnection(VistaConnection *pCon) const;


};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H
