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

#ifndef _VISTAWIIMOTEDRIVERCOMMONSHARE_H_
#define _VISTAWIIMOTEDRIVERCOMMONSHARE_H_

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaBase/VistaBaseTypes.h>

#include <wiiuse.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

namespace VistaWiimoteDriverCommonShare {
//
//	struct vec3b_t
//	{
//		VistaType::byte x,y,z;
//	};
//
//	struct vec2b_t
//	{
//		VistaType::byte x,y;
//	};
//
//	struct orient_t
//	{
//			float roll;						/**< roll, this may be smoothed
//if enabled	*/
//			float pitch;					/**< pitch, this may be smoothed if
//enabled	*/ 			float yaw;
//
//			float a_roll;					/**< absolute roll, unsmoothed
//*/ 			float a_pitch;
//	};
//
//	struct gforce_t {
//		float x, y, z;
//	};
//
//	enum aspect_t {
//		WIIUSE_ASPECT_4_3,
//		WIIUSE_ASPECT_16_9
//	};
//
//	enum ir_position_t {
//		WIIUSE_IR_ABOVE,
//		WIIUSE_IR_BELOW
//	};
//
//	struct ir_dot_t {
//		VistaType::byte visible;					/**< if the IR source is visible
//*/
//
//		unsigned int x;					/**< interpolated X coordinate
//*/ 		unsigned int y;					/**< interpolated Y coordinate
//*/
//
//		short rx;						/**< raw X coordinate (0-1023)
//*/ 		short ry;						/**< raw Y coordinate (0-767)
//*/
//
//		VistaType::byte order;						/**< increasing order by
//x-axis value	*/
//
//		VistaType::byte size;						/**< size of the IR dot
//(0-15)			*/
//	};
//
//	struct ir_t {
//		struct ir_dot_t dot[4];			/**< IR dots
//*/ 		VistaType::byte num_dots;					/**< number of dots at this time
//*/
//
//		enum aspect_t aspect;			/**< aspect ratio of the screen
//*/
//
//		enum ir_position_t pos;			/**< IR sensor bar position
//*/
//
//		unsigned int vres[2];			/**< IR virtual screen resolution
//*/ 		int offset[2];					/**< IR XY correction offset
//*/ 		int state;						/**< keeps track of the IR state
//*/
//
//		int ax;							/**< absolute X coordinate
//*/ 		int ay;							/**< absolute Y coordinate
//*/
//
//		int x;							/**< calculated X coordinate
//*/ 		int y;							/**< calculated Y coordinate
//*/
//
//		float distance;					/**< pixel distance between first 2
//dots*/ 		float z;
//	};
//
//	struct accel_t
//	{
//		struct vec3b_t cal_zero;		/**< zero calibration
//*/ 		struct vec3b_t cal_g;			/**< 1g difference around 0cal			*/
//
//		float st_roll;					/**< last smoothed roll value
//*/ 		float st_pitch;					/**< last smoothed roll pitch
//*/ 		float st_alpha;					/**< alpha value for smoothing [0-1]	*/
//	};
//
//	struct joystick_t {
//		struct vec2b_t max;				/**< maximum joystick values	*/
//		struct vec2b_t min;				/**< minimum joystick values	*/
//		struct vec2b_t center;			/**< center joystick values		*/
//
//		float ang;						/**< angle the joystick is being held
//*/ 		float mag;						/**< magnitude of the joystick (range 0-1)
//*/
//	};
//
//	struct nunchuk_t
//	{
//		struct accel_t accel_calib;		/**< nunchuk accelerometer calibration
//*/ 		struct joystick_t js;			/**< joystick calibration
//*/
//
//		int* flags;						/**< options flag (points to
//wiimote_t.flags) */
//
//		VistaType::byte btns;						/**< what buttons have just
//been pressed	*/
//		VistaType::byte btns_held;					/**< what buttons are being
//held down		*/
//		VistaType::byte btns_released;				/**< what buttons were just
//released this	*/
//
//		float orient_threshold;			/**< threshold for orient to generate an
//event */ 		int accel_threshold;			/**< threshold for accel to generate an event */
//
//		struct vec3b_t accel;			/**< current raw acceleration data
//*/ 		struct orient_t orient;			/**< current orientation on each axis		*/
//		struct gforce_t gforce;			/**< current gravity forces on each axis
//*/
//	};
//
//	struct classic_ctrl_t {
//		short btns;						/**< what buttons have just been
//pressed	*/
//		short btns_held;				/**< what buttons are being held down
//*/ 		short btns_released;			/**< what buttons were just released this	*/
//
//		float r_shoulder;				/**< right shoulder button (range 0-1)
//*/ 		float l_shoulder;				/**< left shoulder button (range 0-1)		*/
//
//		struct joystick_t ljs;			/**< left joystick calibration
//*/ 		struct joystick_t rjs;			/**< right joystick calibration
//*/
//	};
//
//	struct guitar_hero_3_t {
//		short btns;						/**< what buttons have just been
//pressed	*/
//		short btns_held;				/**< what buttons are being held down
//*/ 		short btns_released;			/**< what buttons were just released this	*/
//
//		float whammy_bar;				/**< whammy bar (range 0-1)
//*/
//
//		struct joystick_t js;			/**< joystick calibration
//*/
//	};

struct _wiimoteMeasure {
  vec3b_t  m_Acceleration;
  orient_t m_Orientation;
  gforce_t m_GravityForce;

  ir_t    m_IR;
  accel_t m_Acceleration_calib;

  unsigned short m_buttons;
};

typedef _wiimoteMeasure MeasureType;

enum eState { ST_NOEXP = 0, ST_NUNCHUK = 1, ST_GUITAR = 2, ST_CLASSIC = 4 };

struct _wiistateMeasure {
  int             m_nState;          /**< see wiiuse docu */
  int             m_nExpansionState; /**< see eState */
  float           m_nBatteryLevel;   /**< between 0 and 1 */
  VistaType::byte m_cLEDs;           /**< bit-mask */
};

typedef _wiistateMeasure StateMeasureType;

} // namespace VistaWiimoteDriverCommonShare

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VISTAWIIMOTEDRIVERCOMMONSHARE_H_
