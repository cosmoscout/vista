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
  bool aPressed = false;                         // SDL_CONTROLLER_BUTTON_A
  bool bPressed = false;                         // SDL_CONTROLLER_BUTTON_B
  bool xPressed = false;                         // SDL_CONTROLLER_BUTTON_X
  bool yPressed = false;                         // SDL_CONTROLLER_BUTTON_Y

  bool dpadUpPressed    = false;                 // SDL_CONTROLLER_BUTTON_DPAD_UP
  bool dpadDownPressed  = false;                 // SDL_CONTROLLER_BUTTON_DPAD_DOWN
  bool dpadLeftPressed  = false;                 // SDL_CONTROLLER_BUTTON_DPAD_LEFT
  bool dpadRightPressed = false;                 // SDL_CONTROLLER_BUTTON_DPAD_RIGHT

  bool stickLeftPressed  = false;                // SDL_CONTROLLER_BUTTON_LEFTSTICK
  bool stickRightPressed = false;                // SDL_CONTROLLER_BUTTON_RIGHTSTICK
  
  bool shoulderLeftPressed  = false;             // SDL_CONTROLLER_BUTTON_LEFTSHOULDER
  bool shoulderRightPressed = false;             // SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
  
  bool backPressed  = false;                     // SDL_CONTROLLER_BUTTON_BACK
  bool guidePressed = false;                     // SDL_CONTROLLER_BUTTON_GUIDE
  bool startPressed = false;                     // SDL_CONTROLLER_BUTTON_START

  bool misc1Pressed = false;                     // SDL_CONTROLLER_BUTTON_MISC1

  bool paddle1Pressed = false;                   // SDL_CONTROLLER_BUTTON_PADDLE1
  bool paddle2Pressed = false;                   // SDL_CONTROLLER_BUTTON_PADDLE2
  bool paddle3Pressed = false;                   // SDL_CONTROLLER_BUTTON_PADDLE3
  bool paddle4Pressed = false;                   // SDL_CONTROLLER_BUTTON_PADDLE4
  
  bool touchpadPressed = false;                  // SDL_CONTROLLER_BUTTON_TOUCHPAD

  float stickLeftX = 0.0F;                       // SDL_CONTROLLER_AXIS_LEFTX
  float stickLeftY = 0.0F;                       // SDL_CONTROLLER_AXIS_LEFTY

  float stickRightX = 0.0F;                      // SDL_CONTROLLER_AXIS_RIGHTX
  float stickRightY = 0.0F;                      // SDL_CONTROLLER_AXIS_RIGHTY
  
  float triggerLeft  = 0.0F;                     // SDL_CONTROLLER_AXIS_TRIGGERLEFT
  float triggerRight = 0.0F;                     // SDL_CONTROLLER_AXIS_TRIGGERRIGHT

  bool  hasAcceleration    = false;
  float imuAcceleration[3] = {0.0F, 0.0F, 0.0F}; // SDL_CONTROLLERSENSORUPDATE
  bool  hasGyro            = false;
  float imuGyro[3]         = {0.0F, 0.0F, 0.0F}; // SDL_CONTROLLERSENSORUPDATE
  
  bool  touchpadFinger1Down = false;             // SDL_CONTROLLERTOUCHPADDOWN, SDL_CONTROLLERTOUCHPADUP
  float touchpadFinger1X    = 0.0F;              // SDL_CONTROLLERTOUCHPADMOTION
  float touchpadFinger1Y    = 0.0F;              // SDL_CONTROLLERTOUCHPADMOTION

  bool  touchpadFinger2Down = false;             // SDL_CONTROLLERTOUCHPADDOWN, SDL_CONTROLLERTOUCHPADUP
  float touchpadFinger2X    = 0.0F;              // SDL_CONTROLLERTOUCHPADMOTION
  float touchpadFinger2Y    = 0.0F;              // SDL_CONTROLLERTOUCHPADMOTION
};

#endif //__VISTASDL2CONTROLLERSENSORMEASURES_H
