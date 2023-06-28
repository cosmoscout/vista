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

#ifndef _VISTASTANDALONEDATATUNNEL_H
#define _VISTASTANDALONEDATATUNNEL_H
/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/Cluster/VistaDataTunnel.h>
#include <VistaKernel/VistaKernelConfig.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaStandaloneDataTunnel is a concrete implementation of IVistaDataTunnel for standalone
 * applications, i.e. applications not using ViSTA cluster capabilities. Basically this is just
 * composed of two tubes, through which data can be routed.
 *
 * The standalone version of the data tunnel just pumps the data through its two tubes. Therefore
 * the given producers and consumers are directly connected to each other. The pipes are therefore
 * completely passive.
 *
 * For basic concepts see VistaDataTunnel.h.
 *
 */

class VISTAKERNELAPI VistaStandaloneDataTunnel : public IVistaDataTunnel {
 public:
  VistaStandaloneDataTunnel();
  virtual ~VistaStandaloneDataTunnel();

  virtual bool ConnectUpstreamInput(IDLVistaPipeComponent* pInput);
  virtual bool ConnectUpstreamOutput(IDLVistaPipeComponent* pOutput);
  virtual bool ConnectDownstreamInput(IDLVistaPipeComponent* pInput);
  virtual bool ConnectDownstreamOutput(IDLVistaPipeComponent* pOutput);

  virtual bool DisconnectUpstreamInput();
  virtual bool DisconnectUpstreamOutput();
  virtual bool DisconnectDownstreamInput();
  virtual bool DisconnectDownstreamOutput();
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASTANDALONEDATATUNNEL_H
