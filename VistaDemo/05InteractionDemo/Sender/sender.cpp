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

#include <VistaInterProcComm/Connections/VistaConnectionIP.h>
#include <VistaInterProcComm/Connections/VistaMsgChannel.h>

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaExceptionBase.h>

#include <iostream>

int main() {
  // simple sender to a message port
  // sends s signal over the message port
  // the hostname and port must be the same as
  // given in the vista.ini from your receiver (Bob)

  VistaMsgChannel    oMsgChannel;
  VistaConnectionIP* pIPConnection = new VistaConnectionIP(VistaConnectionIP::CT_TCP);

  pIPConnection->SetHostNameAndPort("127.0.0.1", 6666);
  pIPConnection->Open();
  oMsgChannel.SetConnection(pIPConnection);

  if (pIPConnection == NULL)
    VISTA_THROW("Failed to open connection", 1);

  // All ok, inform the user how to quit.
  std::cout << "Enter 0 to end the program!" << std::endl << std::endl;

  // This PropertyList contains the data you want to send
  // here it is only a simple bool
  VistaPropertyList oPropList;
  oPropList.SetValue<bool>("someValue", true);
  while (true) {
    char c[256];
    std::cout << "Enter Msg: ";
    std::cout << std::endl;
    std::cin.getline(c, 256);
    // Enter 0 to end the programm
    if (c[0] == '0') {
      pIPConnection->Close();
      delete pIPConnection;
      return 0;
    }

    // send the message with the token `c` as int
    // and the properties.
    oMsgChannel.WriteMsg(((int)(c[0])) - 48, oPropList);
  }
}
