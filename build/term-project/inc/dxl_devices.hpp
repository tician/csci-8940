/*
 *******************************************************************************
 *  dxl_devices.h - 2014-02-20-1900
 *******************************************************************************
 * Copyright (c) 2011, 2012, 2013, 2014, Matthew Paulishen.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************
 */
 
#ifndef _DXL_DEVS_H_
#define _DXL_DEVS_H_

namespace Robot
{
	class DXL_DEFAULTS
	{
	public:
		enum
		{
			AX12								= 12,		// 0x000C
			AX12W								= 300,		// 0x012C
			AX18								= 18,		// 0x0012
			XL320								= 350,		// 0x015E

			RX24F								= 24,		// 0x0018
			RX28								= 28,		// 0x001C
			RX64								= 64,		// 0x0040
			EX106								= 107,		// 0x006B

			MX28								= 29,		// 0x001D
			MX64								= 310,		// 0x0136
			MX106								= 356,		// 0x0164		

			CM730								= 29440,	// 0x7300
			CM904								= 36928,	// 0x9040

			AXS1								= 13,		// 0x000D
			IRSA								= 330,		// 0x014A
			RFPS								= 340,		// 0x0154
			HaViMo2								= 0,		// 0x0000

		// DISCONTINUED
			JHFPS								= 17163,	// 0x6711
			AXS20								= 212,		// 0x00D4
			RX10								= 10,		// 0x000A
			DX113								= 113,		// 0x0071
			DX116								= 116,		// 0x0074
			DX117								= 117		// 0x0075
		};
		
		enum
		{
			FPS_RIGHT_ID						= 111,
			FPS_LEFT_ID							= 112,
			HAVIMO2_ID							= 100,
			CM730_ID							= 200,
			CM904_ID							= 252,
			ARBOTIX_ID							= 253
		};
	};

	class DXL_CLASSIC
	{
	public:
		// AX/DX/RX/EX Address Table
		enum
		{
			DXC_MODEL_NUMBER_L					= 0,
			DXC_MODEL_NUMBER_H					= 1,
			DXC_FIRMWARE_VERSION				= 2,
			DXC_ID								= 3,
			DXC_BAUD_RATE						= 4,
			DXC_RETURN_DELAY_TIME				= 5,
			DXC_CW_ANGLE_LIMIT_L				= 6,
			DXC_CW_ANGLE_LIMIT_H				= 7,
			DXC_CCW_ANGLE_LIMIT_L				= 8,
			DXC_CCW_ANGLE_LIMIT_H				= 9,
			DXC_SYSTEM_DATA2					= 10,
			DXC_HIGHEST_LIMIT_TEMPERATURE		= 11,
			DXC_LOWEST_LIMIT_VOLTAGE			= 12,
			DXC_HIGHEST_LIMIT_VOLTAGE			= 13,
			DXC_MAX_TORQUE_L					= 14,
			DXC_MAX_TORQUE_H					= 15,
			DXC_STATUS_RETURN_LEVEL				= 16,
			DXC_ALARM_LED						= 17,
			DXC_ALARM_SHUTDOWN					= 18,
			DXC_OPERATING_MODE					= 19,
			DXC_DOWN_CALIBRATION_L				= 20,
			DXC_DOWN_CALIBRATION_H				= 21,
			DXC_UP_CALIBRATION_L				= 22,
			DXC_UP_CALIBRATION_H				= 23,
			DXC_TORQUE_ENABLE					= 24,
			DXC_LED								= 25,
			DXC_CW_COMPLIANCE_MARGIN			= 26,
			DXC_CCW_COMPLIANCE_MARGIN			= 27,
			DXC_CW_COMPLIANCE_SLOPE				= 28,
			DXC_CCW_COMPLIANCE_SLOPE			= 29,
			DXC_GOAL_POSITION_L					= 30,
			DXC_GOAL_POSITION_H					= 31,
			DXC_MOVING_SPEED_L					= 32,
			DXC_MOVING_SPEED_H					= 33,
			DXC_TORQUE_LIMIT_L					= 34,
			DXC_TORQUE_LIMIT_H					= 35,
			DXC_PRESENT_POSITION_L				= 36,
			DXC_PRESENT_POSITION_H				= 37,
			DXC_PRESENT_SPEED_L					= 38,
			DXC_PRESENT_SPEED_H					= 39,
			DXC_PRESENT_LOAD_L					= 40,
			DXC_PRESENT_LOAD_H					= 41,
			DXC_PRESENT_VOLTAGE					= 42,
			DXC_PRESENT_TEMPERATURE				= 43,
			DXC_REGISTERED_INSTRUCTION			= 44,
			DXC_PAUSE_TIME						= 45,
			DXC_MOVING							= 46,
			DXC_LOCK							= 47,
			DXC_PUNCH_L							= 48,
			DXC_PUNCH_H							= 49
		};
	};

	class DXL_MODERN
	{
	public:
		// MX Address Table
		enum
		{
			DXM_MODEL_NUMBER_L					= 0,
			DXM_MODEL_NUMBER_H					= 1,
			DXM_VERSION							= 2,
			DXM_ID								= 3,
			DXM_BAUD_RATE						= 4,
			DXM_RETURN_DELAY_TIME				= 5,
			DXM_CW_ANGLE_LIMIT_L				= 6,
			DXM_CW_ANGLE_LIMIT_H				= 7,
			DXM_CCW_ANGLE_LIMIT_L				= 8,
			DXM_CCW_ANGLE_LIMIT_H				= 9,
			DXM_SYSTEM_DATA2					= 10,
			DXM_HIGH_LIMIT_TEMPERATURE			= 11,
			DXM_LOW_LIMIT_VOLTAGE				= 12,
			DXM_HIGH_LIMIT_VOLTAGE				= 13,
			DXM_MAX_TORQUE_L					= 14,
			DXM_MAX_TORQUE_H					= 15,
			DXM_RETURN_LEVEL					= 16,
			DXM_ALARM_LED						= 17,
			DXM_ALARM_SHUTDOWN					= 18,
			DXM_OPERATING_MODE					= 19,
			DXM_LOW_CALIBRATION_L				= 20,
			DXM_LOW_CALIBRATION_H				= 21,
			DXM_HIGH_CALIBRATION_L				= 22,
			DXM_HIGH_CALIBRATION_H				= 23,
			DXM_TORQUE_ENABLE					= 24,
			DXM_LED								= 25,
			DXM_P_GAIN							= 26,
			DXM_I_GAIN							= 27,
			DXM_D_GAIN							= 28,
			DXM_RESERVED						= 29,
			DXM_GOAL_POSITION_L					= 30,
			DXM_GOAL_POSITION_H					= 31,
			DXM_MOVING_SPEED_L					= 32,
			DXM_MOVING_SPEED_H					= 33,
			DXM_TORQUE_LIMIT_L					= 34,
			DXM_TORQUE_LIMIT_H					= 35,
			DXM_PRESENT_POSITION_L				= 36,
			DXM_PRESENT_POSITION_H				= 37,
			DXM_PRESENT_SPEED_L					= 38,
			DXM_PRESENT_SPEED_H					= 39,
			DXM_PRESENT_LOAD_L					= 40,
			DXM_PRESENT_LOAD_H					= 41,
			DXM_PRESENT_VOLTAGE					= 42,
			DXM_PRESENT_TEMPERATURE				= 43,
			DXM_REGISTERED_INSTRUCTION			= 44,
			DXM_PAUSE_TIME						= 45,
			DXM_MOVING							= 46,
			DXM_LOCK							= 47,
			DXM_PUNCH_L							= 48,
			DXM_PUNCH_H							= 49,
			DXM_RESERVED4						= 50,
			DXM_RESERVED5						= 51,
			DXM_POT_L							= 52,
			DXM_POT_H							= 53,
			DXM_PWM_OUT_L						= 54,
			DXM_PWM_OUT_H						= 55,
			DXM_P_ERROR_L						= 56,
			DXM_P_ERROR_H						= 57,
			DXM_I_ERROR_L						= 58,
			DXM_I_ERROR_H						= 59,
			DXM_D_ERROR_L						= 60,
			DXM_D_ERROR_H						= 61,
			DXM_P_ERROR_OUT_L					= 62,
			DXM_P_ERROR_OUT_H					= 63,
			DXM_I_ERROR_OUT_L					= 64,
			DXM_I_ERROR_OUT_H					= 65,
			DXM_D_ERROR_OUT_L					= 66,
			DXM_D_ERROR_OUT_H					= 67,
			DXM_SENSED_CURRENT_L				= 68,
			DXM_SENSED_CURRENT_H				= 69,
			DXM_TORQUE_CONTROL_MODE				= 70,
			DXM_GOAL_TORQUE_L					= 71,
			DXM_GOAL_TORQUE_H					= 72,
			DXM_GOAL_ACCELERATION				= 73
		};
	};

	class DXL_CONTROLLER
	{
	public:
		// CM-730/904 Address Table
		enum
		{
			CMX_MODEL_NUMBER_L					= 0,
			CMX_MODEL_NUMBER_H					= 1,
			CMX_FIRMWARE_VERSION				= 2,
			CMX_ID								= 3,
			CMX_BAUD_RATE						= 4,
			CMX_RETURN_DELAY_TIME				= 5,
			CMX_STATUS_RETURN_LEVEL				= 16,
			CMX_DXL_POWER						= 24,
			CMX_LED_PANEL						= 25,
			CMX_LED_HEAD_L						= 26,
			CMX_LED_HEAD_H						= 27,
			CMX_LED_EYE_L						= 28,
			CMX_LED_EYE_H						= 29,
			CMX_BUTTON							= 30,
			CMX_GYRO_Z_L						= 38,
			CMX_GYRO_Z_H						= 39,
			CMX_GYRO_Y_L						= 40,
			CMX_GYRO_Y_H						= 41,
			CMX_GYRO_X_L						= 42,
			CMX_GYRO_X_H						= 43,
			CMX_ACCEL_X_L						= 44,
			CMX_ACCEL_X_H						= 45,
			CMX_ACCEL_Y_L						= 46,
			CMX_ACCEL_Y_H						= 47,
			CMX_ACCEL_Z_L						= 48,
			CMX_ACCEL_Z_H						= 49,
			CMX_VOLTAGE							= 50,
			CMX_LEFT_MIC_L						= 51,
			CMX_LEFT_MIC_H						= 52,
			CMX_ADC2_L							= 53,
			CMX_ADC2_H							= 54,
			CMX_ADC3_L							= 55,
			CMX_ADC3_H							= 56,
			CMX_ADC4_L							= 57,
			CMX_ADC4_H							= 58,
			CMX_ADC5_L							= 59,
			CMX_ADC5_H							= 60,
			CMX_ADC6_L							= 61,
			CMX_ADC6_H							= 62,
			CMX_ADC7_L							= 63,
			CMX_ADC7_H							= 64,
			CMX_ADC8_L							= 65,
			CMX_ADC8_H							= 66,
			CMX_RIGHT_MIC_L						= 67,
			CMX_RIGHT_MIC_H						= 68,
			CMX_ADC10_L							= 69,
			CMX_ADC10_H							= 70,
			CMX_ADC11_L							= 71,
			CMX_ADC11_H							= 72,
			CMX_ADC12_L							= 73,
			CMX_ADC12_H							= 74,
			CMX_ADC13_L							= 75,
			CMX_ADC13_H							= 76,
			CMX_ADC14_L							= 77,
			CMX_ADC14_H							= 78,
			CMX_ADC15_L							= 79,
			CMX_ADC15_H							= 80
		};
	};

	class DXL_SENSORS
	{
	public:
		// Robotis' FSR Foot Pressure Sensor Boards
		enum
		{
			RFPS_MODEL_NUMBER_L					= 0,
			RFPS_MODEL_NUMBER_H					= 1,
			RFPS_FIRMWARE_VERSION				= 2,
			RFPS_ID								= 3,
			RFPS_BAUD_RATE						= 4,
			RFPS_RETURN_DELAY_TIME				= 5,
			RFPS_STATUS_RETURN_LEVEL			= 16,
			RFPS_LED							= 25,
			RFPS_FSR1_L							= 26,
			RFPS_FSR1_H							= 27,
			RFPS_FSR2_L							= 28,
			RFPS_FSR2_H							= 29,
			RFPS_FSR3_L							= 30,
			RFPS_FSR3_H							= 31,
			RFPS_FSR4_L							= 32,
			RFPS_FSR4_H							= 33,
			RFPS_FSR_CENTRAL_X					= 34,
			RFPS_FSR_CENTRAL_Y					= 35,
			RFPS_PRESENT_VOLTAGE				= 42,
			RFPS_REGISTERED_INSTRUCTION			= 44,
			RFPS_LOCK							= 47
		};
		
		// Robotis' IR Sensor Array Boards
		enum
		{
			IRSA_MODEL_NUMBER_L					= 0,
			IRSA_MODEL_NUMBER_H					= 1,
			IRSA_FIRMWARE_VERSION				= 2,
			IRSA_ID								= 3,
			IRSA_BAUD_RATE						= 4,
			IRSA_RETURN_DELAY_TIME				= 5,
			IRSA_ETH_L_1						= 6,
			IRSA_ETH_H_1						= 7,
			IRSA_ETH_L_2						= 8,
			IRSA_ETH_H_2						= 9,
			IRSA_ETH_L_3						= 10,
			IRSA_ETH_H_3						= 11,
			IRSA_ETH_L_4						= 12,
			IRSA_ETH_H_4						= 13,
			IRSA_ETH_L_5						= 14,
			IRSA_ETH_H_5						= 15,
			IRSA_STATUS_RETURN_LEVEL			= 16,
			IRSA_ETH_L_6						= 17,
			IRSA_ETH_H_6						= 18,
			IRSA_ETH_L_7						= 20,
			IRSA_ETH_H_7						= 21,
			IRSA_FD_L_1							= 24,
			IRSA_FD_H_1							= 25,
			IRSA_FD_L_2							= 26,
			IRSA_FD_H_2							= 27,
			IRSA_FD_L_3							= 28,
			IRSA_FD_H_3							= 29,
			IRSA_FD_L_4							= 30,
			IRSA_FD_H_4							= 31,
			IRSA_FD_L_5							= 32,
			IRSA_FD_H_5							= 33,
			IRSA_FD_L_6							= 34,
			IRSA_FD_H_6							= 35,
			IRSA_FD_L_7							= 36,
			IRSA_FD_H_7							= 37,
			IRSA_BUZZ_NOTE						= 40,
			IRSA_BUZZ_TIME						= 41,
			IRSA_AUTO_TH						= 42,
			IRSA_OBS_DET						= 43,
			IRSA_REGISTERED_INSTRUCTION			= 44,
			IRSA_LOCK							= 47,
			IRSA_TH_L_1							= 48,
			IRSA_TH_H_1							= 49,
			IRSA_TH_L_2							= 50,
			IRSA_TH_H_2							= 51,
			IRSA_TH_L_3							= 52,
			IRSA_TH_H_3							= 53,
			IRSA_TH_L_4							= 54,
			IRSA_TH_H_4							= 55,
			IRSA_TH_L_5							= 56,
			IRSA_TH_H_5							= 57,
			IRSA_TH_L_6							= 58,
			IRSA_TH_H_6							= 59,
			IRSA_TH_L_7							= 60,
			IRSA_TH_H_7							= 61
		};

		// John Hyland's FSR Foot Pressure Sensor Boards
		enum
		{
			JHFPS_MODEL_NUMBER_L				= 0,
			JHFPS_MODEL_NUMBER_H				= 1,
			JHFPS_FIRMWARE_VERSION				= 2,
			JHFPS_ID							= 3,
			JHFPS_BAUD_RATE						= 4,
			JHFPS_RETURN_DELAY_TIME				= 5,
			JHFPS_STATUS_RETURN_LEVEL			= 16,
			JHFPS_LED							= 25,
			JHFPS_FRONT_INSIDE_L				= 26,
			JHFPS_FRONT_INSIDE_H				= 27,
			JHFPS_FRONT_OUTSIDE_L				= 28,
			JHFPS_FRONT_OUTSIDE_H				= 29,
			JHFPS_REAR_INSIDE_L					= 30,
			JHFPS_REAR_INSIDE_H					= 31,
			JHFPS_REAR_OUTSIDE_L				= 32,
			JHFPS_REAR_OUTSIDE_H				= 33
		};

		// AX-S1 Address Table
		enum
		{
			AXS1_MODEL_NUMBER_L					= 0,
			AXS1_MODEL_NUMBER_H					= 1,
			AXS1_FIRMWARE_VERSION				= 2,
			AXS1_ID								= 3,
			AXS1_BAUD_RATE						= 4,
			AXS1_RETURN_DELAY_TIME				= 5,
			AXS1_STATUS_RETURN_LEVEL			= 16,
			AXS1_IR_LEFT						= 26,
			AXS1_IR_CENTER						= 27,
			AXS1_IR_RIGHT						= 28,
			AXS1_LUMINOSITY_LEFT				= 29,
			AXS1_LUMINOSITY_CENTER				= 30,
			AXS1_LUMINOSITY_RIGHT				= 31,
			AXS1_IR_OBSTACLE_DETECTED			= 32,
			AXS1_LUMINOSITY_DETECTED			= 33,
			AXS1_SOUND_DATA						= 35,
			AXS1_SOUND_DATA_MAX_HOLD			= 36,
			AXS1_SOUND_DETECT_COUNT				= 37,
			AXS1_SOUND_DETECT_TIME_L			= 38,
			AXS1_SOUND_DETECT_TIME_H			= 39,
			AXS1_BUZZER_INDEX					= 40,
			AXS1_BUZZER_TIME					= 41,
			AXS1_IR_REMOCON_ARRIVED				= 46,
			AXS1_IR_REMOCON_RX_DATA_L			= 48,
			AXS1_IR_REMOCON_RX_DATA_H			= 49,
			AXS1_IR_REMOCON_TX_DATA_L			= 50,
			AXS1_IR_REMOCON_TX_DATA_H			= 51,
			AXS1_OBSTACLE_DETECTED_COMPARE		= 52,
			AXS1_LIGHT_DETECTED_COMPARE			= 53
		};


		// AX-S20 Address Table
		enum
		{
			AXS20_MODEL_NUMBER_L				= 0,
			AXS20_MODEL_NUMBER_H				= 1,
			AXS20_FIRMWARE_VERSION				= 2,
			AXS20_ID							= 3,
			AXS20_BAUD_RATE						= 4,
			AXS20_RETURN_DELAY_TIME				= 5,
			AXS20_STATUS_RETURN_LEVEL			= 16,
			AXS20_LED							= 25,
		// 0-360 degrees			+CW from top/-CCW from top
			AXS20_AZIMUTH_Z_L					= 26,
			AXS20_AZIMUTH_Z_H					= 27,
		// -90-90 degrees [word]	+forward/-backward
			AXS20_PITCH_Y_L						= 28,
			AXS20_PITCH_Y_H						= 29,
		// -90-90 degrees [word]	+left/-right
			AXS20_ROLL_X_L						= 30,
			AXS20_ROLL_X_H						= 31,
		// raw magneto flux density
			AXS20_MAGNETO_Z_L					= 32,
			AXS20_MAGNETO_Z_H					= 33,
		// raw magneto flux density
			AXS20_MAGNETO_X_L					= 34,
			AXS20_MAGNETO_X_H					= 35,
		// raw magneto flux density
			AXS20_MAGNETO_Y_L					= 36,
			AXS20_MAGNETO_Y_H					= 37,
		// -2048~2048			   +forward/-backward
			AXS20_ACCEL_X_L						= 38,
			AXS20_ACCEL_X_H						= 39,
		// -2048~2048			   +left/-right
			AXS20_ACCEL_Y_L						= 40,
			AXS20_ACCEL_Y_H						= 41,
		// -2048~2048			   +up/-down
			AXS20_ACCEL_Z_L						= 46,
			AXS20_ACCEL_Z_H						= 47,
		// -90~90 degrees [byte] from accelerometer
			AXS20_ACCEL_PITCH_Y					= 48,
		// -90~90 degrees [byte] from accelerometer
			AXS20_ACCEL_ROLL_X					= 49
		};
	};

}
#endif
