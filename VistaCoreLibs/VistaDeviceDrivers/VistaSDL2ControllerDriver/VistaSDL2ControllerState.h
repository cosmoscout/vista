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

#ifndef __VISTASDL2CONTROLLERSENSORMEASURES_H
#define __VISTASDL2CONTROLLERSENSORMEASURES_H

struct VistaSDL2ControllerState {
  bool aPressed;             // SDL_CONTROLLER_BUTTON_A
  bool bPressed;             // SDL_CONTROLLER_BUTTON_B
  bool xPressed;             // SDL_CONTROLLER_BUTTON_X
  bool yPressed;             // SDL_CONTROLLER_BUTTON_Y

  bool dpadUpPressed;        // SDL_CONTROLLER_BUTTON_DPAD_UP
  bool dpadDownPressed;      // SDL_CONTROLLER_BUTTON_DPAD_DOWN
  bool dpadLeftPressed;      // SDL_CONTROLLER_BUTTON_DPAD_LEFT
  bool dpadRightPressed;     // SDL_CONTROLLER_BUTTON_DPAD_RIGHT

  bool stickLeftPressed;     // SDL_CONTROLLER_BUTTON_LEFTSTICK
  bool stickRightPressed;    // SDL_CONTROLLER_BUTTON_RIGHTSTICK
  
  bool shoulderLeftPressed;  // SDL_CONTROLLER_BUTTON_LEFTSHOULDER
  bool shoulderRightPressed; // SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
  
  bool backPressed;          // SDL_CONTROLLER_BUTTON_BACK
  bool guidePressed;         // SDL_CONTROLLER_BUTTON_GUIDE
  bool startPressed;         // SDL_CONTROLLER_BUTTON_START

  bool misc1Pressed;         // SDL_CONTROLLER_BUTTON_MISC1

  bool paddle1Pressed;       // SDL_CONTROLLER_BUTTON_PADDLE1
  bool paddle2Pressed;       // SDL_CONTROLLER_BUTTON_PADDLE2
  bool paddle3Pressed;       // SDL_CONTROLLER_BUTTON_PADDLE3
  bool paddle4Pressed;       // SDL_CONTROLLER_BUTTON_PADDLE4
  
  bool touchpadPressed;      // SDL_CONTROLLER_BUTTON_TOUCHPAD

  float stickLeftX;          // SDL_CONTROLLER_AXIS_LEFTX
  float stickLeftY;          // SDL_CONTROLLER_AXIS_LEFTY

  float stickRightX;         // SDL_CONTROLLER_AXIS_RIGHTX
  float stickRightY;         // SDL_CONTROLLER_AXIS_RIGHTY
  
  float triggerLeft;         // SDL_CONTROLLER_AXIS_TRIGGERLEFT
  float triggerRight;        // SDL_CONTROLLER_AXIS_TRIGGERRIGHT
};

#endif //__VISTASDL2CONTROLLERSENSORMEASURES_H
