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

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

#include "VistaFileRegistration.h"

#include <fstream>
using namespace std;

#include <cstdio>
#include <cstring>

#define BUFFERSIZE 1024

/*============================================================================*/
/* STATICS                                                                    */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

DLVistaFileRegistration::DLVistaFileRegistration(const string& SFName) {
  m_iPacketTypeCount = 0;
  ifstream infile;

  infile.open(SFName.c_str());
  if (infile.good()) {
    printf("DLVistaFileRegistration::DLVistaFileRegistration(%s) -- OPENED\n", SFName.c_str());
    char buffer[BUFFERSIZE];

    while (!infile.eof()) {
      memset(buffer, 0, BUFFERSIZE);
      infile.getline(buffer, BUFFERSIZE, '\n');
      printf("read buffer: %s\n", buffer);
      if (strlen(buffer) && buffer[0] != '\n')
        RegisterPacketType(buffer, ++m_iPacketTypeCount);
    }
    printf("-- READING DONE.\n");
  }

  infile.close();
}

DLVistaFileRegistration::~DLVistaFileRegistration() {
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

int DLVistaFileRegistration::CreateTypeIdForName(const string& SName) {
  return -1;
}
