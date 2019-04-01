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


#ifndef _VISTADATATUNNEL_H
#define _VISTADATATUNNEL_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IDLVistaDataPacket;
class IDLVistaPipeComponent;
class IDLVistaDataProducer;
class IDLVistaDataConsumer;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
* IVistaDataTunnel defines the generic base behavior for a 'tunnel-like' data channel. The tunnel basically consists
* of two one directional tubes, which in turn are implemented as DataLaViSTA-pipelines.
* The <b>IDEA</b> behind the data tunnel is this: Suppose you want to transmit data between different endpoints (say a VR-system
* and a remote computer generating data). Now assume your VR-system is a cluster architecture and the incoming data needs
* to be received at all nodes. The task of IVistaDataTunnel is to solve just this problem transparently, without having
* you to bother about the actual implementation details. In particular the VistaDataTunnel is meant to even hide these details
* from its user completely, i.e. an application using the tunnel doesn't need to know whether it's running in a cluster or not.
* All you need to know is that you push "something" inside on the one end and you will eventually get "something" in return.
* By "something" we mean anything which is wrapped into an IDLVistaDataPacket-derivative of your choice.
*
* For all this to work we need some ASSUMPTION and DEFINITIONS:
* -	IVistaDataTunnel internally provides two separate, one-directional channels (like the two tubes of a tunnel).
*	One is called UPSTREAM, the other one DOWNSTREAM.
* -	The UPSTREAM is used to post any kind of REQUEST packets. These packets are passed through the pipe and eventually
*	they will be translated into a request of any application-specific kind. NOTE that these packets can contain anything,
*	from simple parameter data to ...
* -	The DOWNSTREAM is used to handle the answer to a previously postet request. It transports the data to where it
*	can be processed meaningfully.
* - In a STANDALONE environment the setup is just two tubes, forwarding the packets given to them.
* - In a cluster environment there are two special roles
*	>	The MASTER is allowed to post requests normally. These will be processed by the master's upstream consumer.
*		The feedback will be handled by the master's downstream producer which sends them downstream. On their way
*		downstream, the packets will be mirrored to the clients.
*	>	All SLAVE request will be lost deliberately.
*		the very same request due to the "mirrored application concept"
*	>	The MASTER mirrors any downstream packets to the clients which in turn can then issue them to their very own
*		downstream pipe. Thus for the client it looks like the answer to his request just came directly to him
*	>	Any downstream packets produced by the SLAVE's downstream producer will never see the light of day again,
*		since they will end up in a dead end. The downstream packets which are consumed by the client's downstream
*		consumer come from the data collector which in turn receives them from the master.
*	>>>	As you can deduce from the above statements, it is generally a good idea to use the downstream pipe in
*		PULL MODE ONLY, since otherwise you will eventually run into trouble...
*
* -	In order for data to be processable by the pipeline, it must be stowed in a IDLVistaDataPacket (...or concrete subclass).
*	Note that the data tunnel doesn't know a thing about the content of the packet. It just operates on the given interface.
* - The data tunnel's upstream is always realized as a passive component, i.e. you have to provide control flow to it.
*   In constrast the downstream is active for Client and Master implementation), meaning that there is a thread in each
*   one, actively pushing data through the tunnel. You won't notice this though since the tunnel's downstream output is
*   connected to a queue in which all packets are stowed until YOU retrieve them.
*   NOTE: When you don't want your application to know about its role in the cluster, it might
*	be a good idea to settle with a passive UpstreamOutput as well as passive DownstreamInput pipe, since
*	these elements will not have any function on a cluster client. Thus the threads used for their implementation if
*	they were active would just eat up cpu time doing nothing in effect.
*
* Please remember, that your application will not notice any of the internal handling. It just connects to the
* up/downstream of the data tunnel. Thus, the 'tunnel metaphor' holds, i.e. you won't have a clue
* (...and don't need to have), to what happens 'in there'.
*
* USAGE:
* In order to use the DataTunnel you'll have to implement the following classes:
* - An UpstreamProducer. This is the one that fills request packets and sends them down the upstream pipe.
* -	An UpstreamConsumer. This one handles incoming packets, unwraps them and translates them to any request/call
*	which might be needed.
* - A DownstreamProducer. It handles the incoming data. You just need to wrap your data in some implementation of
*	IDLVistaDataPacket.
* - A DownstreamConsumer, which will provide the data to your application. The consumer typically unwraps the data
*	and
* NOTE: There is no direct access to the 'tunnel tubes'. You'll have to handle this via DLV-components. This might seem
*		to be an inhibition but a DLV-pipe is a closed system itself and thus there is no 'easy' or 'nice' way to pass data
*		to the user.
*
* ADD. NOTES:
*	- There is exactly one input and one output of the upstream and downstream respectively...
*	- ...
*
*/

class VISTAKERNELAPI IVistaDataTunnel
{
public:
	virtual ~IVistaDataTunnel();
	/**
	* Connect an input to the data tunnel's upstream (this may be a single producer or a whole pipe, it's up to the user)
	*/
	virtual bool ConnectUpstreamInput(IDLVistaPipeComponent* pInput) = 0;
	/**
	* Connect an output to the data tunnel's upstream (this may be a single producer or a whole pipe, it's up to the user)
	*/
	virtual bool ConnectUpstreamOutput(IDLVistaPipeComponent *pOutput) = 0;
	/**
	* Connect an input to the data tunnel's downstream (this may be a single producer or a whole pipe, it's up to the user)
	*/
	virtual bool ConnectDownstreamInput(IDLVistaPipeComponent* pInput) = 0;
	/**
	* Connect an output to the data tunnel's downstream (this may be a single producer or a whole pipe, it's up to the user)
	*/
	virtual bool ConnectDownstreamOutput(IDLVistaPipeComponent *pOutput) = 0;
	virtual bool DisconnectUpstreamInput() = 0;
	virtual bool DisconnectUpstreamOutput() = 0;
	virtual bool DisconnectDownstreamInput() = 0;
	virtual bool DisconnectDownstreamOutput() = 0;

	/*
	*	MISC
	*/
	/**
	* Indicates whether all vital components have been successfully joined together. Iff <true>
	* is returned the data tunnel is ready to use.
	*/
	virtual bool IsComplete() const;

	/******** interface for "active pumping" ************/
	/*
	* start to (actively) tunnel data (if there is any thread in there)
	*/
	virtual void StartTunneling();
	/**
	* stop to tunnel data (and make any threads join)
	* @param bJoin wait until the thread has been finished
	*/
	virtual void StopTunnelingGently(bool bJoin);
	/**
	* stop tunneling right now (by killing any threads that tunnel data)
	*/
	virtual void HaltTunneling();
	virtual void IndicateTunnelingEnd();
	virtual void SignalPendingRequest();

	/**
	* @todo In case we want to use this in a multi-threaded environment, it might be a good idea
	*		to add some kind of locking mechanism for the up/downstream here...
	*/
protected:
	IVistaDataTunnel();
	IVistaDataTunnel(const IVistaDataTunnel& );

	/**
	*	MEMBER ACCESS for derived classes
	*/
	IDLVistaPipeComponent* GetUpstreamInput() const;
	IDLVistaPipeComponent* GetUpstreamOutput() const;
	IDLVistaPipeComponent* GetDownstreamInput() const;
	IDLVistaPipeComponent* GetDownstreamOutput() const;

	void SetUpstreamInput(IDLVistaPipeComponent*);
	void SetUpstreamOutput(IDLVistaPipeComponent*);
	void SetDownstreamInput(IDLVistaPipeComponent*);
	void SetDownstreamOutput(IDLVistaPipeComponent*);

private:
	/**
	*	pointer to the producer of the upstream pipe i.e. the one that incoming upstream packets are forwarded to
	*/
	IDLVistaPipeComponent* m_pUpstreamInput;
	/**
	*	pointer to the upstream consumer i.e. the one that consumes and evaluates incoming upstream packets
	*/
	IDLVistaPipeComponent* m_pUpstreamOutput;
	/**
	*	pointer to the downstream producer, i.e. the one that handles incoming data on the downstream
	*/
	IDLVistaPipeComponent* m_pDownstreamInput;
	/**
	*	pointer to the downstream consumer, i.e. the one that forwards downstream data to the application
	*/
	IDLVistaPipeComponent* m_pDownstreamOutput;
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADATATUNNEL_H

