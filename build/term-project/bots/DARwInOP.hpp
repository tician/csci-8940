/*
 *******************************************************************************
 * 
 *******************************************************************************
 * Copyright (c) 2014, Matthew Paulishen.
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

#ifndef _WALKY_DARWINOP_H_
#define _WALKY_DARWINOP_H_

#include "Props.hpp"

namespace Robot
{
	class DARwInOP : public BotProperties
	{
	private:
		

	protected:
		

	public:
		enum
		{
			ID_R_SHOULDER_PITCH					= 1,
			ID_L_SHOULDER_PITCH					= 2,
			ID_R_SHOULDER_ROLL					= 3,
			ID_L_SHOULDER_ROLL					= 4,
			ID_R_ELBOW							= 5,
			ID_L_ELBOW							= 6,
			ID_R_HIP_YAW						= 7,
			ID_L_HIP_YAW						= 8,
			ID_R_HIP_ROLL						= 9,
			ID_L_HIP_ROLL						= 10,
			ID_R_HIP_PITCH						= 11,
			ID_L_HIP_PITCH						= 12,
			ID_R_KNEE							= 13,
			ID_L_KNEE							= 14,
			ID_R_ANKLE_PITCH					= 15,
			ID_L_ANKLE_PITCH					= 16,
			ID_R_ANKLE_ROLL						= 17,
			ID_L_ANKLE_ROLL						= 18,
			ID_HEAD_PAN							= 19,
			ID_HEAD_TILT						= 20,
#ifdef BOT_HAS_HANDS
			ID_R_GRIPPER						= 21,
			ID_L_GRIPPER						= 22,
#endif
#ifdef BOT_HAS_WRISTS
			ID_R_WRIST							= 23,
			ID_L_WRIST							= 24,
#endif
			NUMBER_OF_JOINTS
		};



		enum
		{
			CORE_TO_HEART,

			CORE_TO_HEAD,
			HEAD_TO_EYES,

			CORE_TO_HIP_R,
			CORE_TO_HIP_L,
			HIP_TO_KNEE_R,
			HIP_TO_KNEE_L,
			KNEE_TO_ANKLE_R,
			KNEE_TO_ANKLE_L,
			ANKLE_TO_FOOT_R,
			ANKLE_TO_FOOT_L,

			CORE_TO_SHLDR_P_R,
			CORE_TO_SHLDR_P_L,
			SHLDR_P_TO_R_R,
			SHLDR_P_TO_R_L,
			SHLDR_R_TO_ELBOW_R,
			SHLDR_R_TO_ELBOW_L,

#ifdef BOT_HAS_WRISTS
			ELBOW_TO_WRIST_R,
			ELBOW_TO_WRIST_L,
	#ifdef BOT_HAS_HANDS
			WRIST_TO_HAND_R,
			WRIST_TO_HAND_L,
			HAND_TO_THUMB_R,
			HAND_TO_THUMB_L,
			HAND_TO_FINGER_R,
			HAND_TO_FINGER_L,
	#else
			WRIST_TO_FINGER_R,
			WRIST_TO_FINGER_L,
	#endif
#elif BOT_HAS_HANDS
			ELBOW_TO_HAND_R,
			ELBOW_TO_HAND_L,
			HAND_TO_THUMB_R,
			HAND_TO_THUMB_L,
			HAND_TO_FINGER_R,
			HAND_TO_FINGER_L,
#else
			ELBOW_TO_FINGER_R,
			ELBOW_TO_FINGER_L,
#endif
			NUMBER_OF_LINKS
		};

/*
		dVect		CORE_TO_SHOULDER;
		dVect		SHOULDER_TO_ELBOW;
		dVect		ELBOW_TO_WRIST;
		dVect		WRIST_TO_HAND;

		dVect		CORE_TO_HIP;
		dVect		HIP_TO_KNEE;
		dVect		KNEE_TO_ANKLE;
		dVect		ANKLE_TO_FOOT;
*/

		DARwInOP();
		~DARwInOP();

		void Initialize();
	};
}

#endif

