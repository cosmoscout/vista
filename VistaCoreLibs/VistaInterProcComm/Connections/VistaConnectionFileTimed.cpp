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

#include "VistaConnectionFileTimed.h"
#include <string>
using namespace std;

VistaConnectionFileTimed::VistaConnectionFileTimed(
    const string& sFilename, const bool bPulse, const float fFrequency, const bool bIncoming)
    : VistaConnectionFile(sFilename, VistaConnectionFile::READ)
    , m_bPulse(bPulse)
    , m_fFrequency(fFrequency)
    , m_bIncoming(bIncoming)
    , m_bStatusOK(bIncoming) {
  m_LastTime = clock();
}

int VistaConnectionFileTimed::Receive(void* buffer, const int length, int iTimeout) {
  if (m_bStatusOK && m_bIncoming) {
    if (m_bPulse) {
      bool elapsed;
#if defined(WIN32)
      elapsed = ((float)(clock() - m_LastTime) / (float)CLK_TCK) > (1.0f / m_fFrequency);
#else
      elapsed = ((float)(clock() - m_LastTime) / (float)CLOCKS_PER_SEC) > (1.0f / m_fFrequency);
#endif
      if (!elapsed)
        return 0;
    }

    int read;
    read       = VistaConnectionFile::Receive(buffer, length, iTimeout);
    m_LastTime = clock();

    return read;
  } else
    return 0;
}

int VistaConnectionFileTimed::Send(const void* buffer, const int length) {
  return 0;
}
