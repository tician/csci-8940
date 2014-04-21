
#include "DARwInOP.hpp"

using namespace Robot;

RoDoFLimit DARwInOP_servos[] = 
{
//	 		ID,				A_min,	A_max,	S_min,	S_max,	T_min,	T_max
	{ID_R_SHOULDER_PITCH,	-180.0,	180.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_L_SHOULDER_PITCH,	-180.0,	180.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_R_SHOULDER_ROLL,	-75.0,	129.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_L_SHOULDER_ROLL,	-129.0,	75.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_R_ELBOW,			-94.0,	66.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_L_ELBOW,			-66.0,	94.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_R_HIP_YAW,			-30.0,	50.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_L_HIP_YAW,			-50.0,	30.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_R_HIP_ROLL,			-5.0,	57.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_L_HIP_ROLL,			-57.0,	5.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_R_HIP_PITCH			-96.0,	29.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_L_HIP_PITCH,		-29.0,	96.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_R_KNEE,				0.0,	132.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_L_KNEE,				-132.0,	0.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_R_ANKLE_PITCH,		-51.0,	79.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_L_ANKLE_PITCH,		-79.0,	51.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_R_ANKLE_ROLL,		-37.0,	57.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_L_ANKLE_ROLL,		-57.0,	37.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_HEAD_PAN,			-120.0,	120.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_HEAD_PITCH,			-22.0,	58.0,	-1.0,	1.0,	-1.0,	1.0		}
#ifdef BOT_HAS_HANDS
,	{ID_R_GRIPPER,			-90.0,	20.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_L_GRIPPER,			-90.0,	20.0,	-1.0,	1.0,	-1.0,	1.0		}
#endif
#ifdef BOT_HAS_WRISTS
,	{ID_R_WRIST,			0.0,	180.0,	-1.0,	1.0,	-1.0,	1.0		}
,	{ID_L_WRIST,			0.0,	180.0,	-1.0,	1.0,	-1.0,	1.0		}
#endif
};

RoDoF DARwInOP_neutral[] = 
{
//			ID,				En,		Ang,	Spd,	Trq,	Inv
	{ID_R_SHOULDER_PITCH,	false,	0.0,	1.0,	1.0,	false			}
,
};

/*
dVect DARwInOP_links[] = 
{
//			ID,				Tx,		Ty,		Tz,		Rx,		Ry,		Rz
	{CORE_TO_HEART,			-20.0,	0.0,	-20.0,	0.0,	0.0,	0.0		}
,	{CORE_TO_HEAD,			0.0,	0.0,	50.5,	0.0,	0.0,	0.0		}
,	{HEAD_TO_EYES,			33.2,	0.0,	34.4,	0.0,	0.0,	0.0		}

,	{CORE_TO_HIP_R,			-5.0,	-37.0,	-122.2,	0.0,	1.57,	0.0		}
,	{CORE_TO_HIP_L,			-5.0,	37.0,	-122.2,	0.0,	1.57,	0.0		}
,	{HIP_TO_KNEE_R,			93.0,	0.0,	0.0,	0.0,	0.0,	0.0		}
,	{HIP_TO_KNEE_L,			93.0,	0.0,	0.0,	0.0,	0.0,	0.0		}
,	{KNEE_TO_ANKLE_R,		93.0,	0.0,	0.0,	0.0,	-1.57,	0.0		}
,	{KNEE_TO_ANKLE_L,		93.0,	0.0,	0.0,	0.0,	-1.57,	0.0		}
,	{ANKLE_TO_FOOT_R,		0.0,	-10.5,	-33.5,	0.0,	0.0,	0.0		}
,	{ANKLE_TO_FOOT_L,		0.0,	10.5,	-33.5,	0.0,	0.0,	0.0		}

,	{CORE_TO_SHLDR_P_R,		0.0,	-82.0,	0.0,	0.0,	0.0,	-1.57	}
,	{CORE_TO_SHLDR_P_L,		0.0,	82.0,	0.0,	0.0,	0.0,	1.57	}
,	{SHLDR_P_TO_R_R,		0.0,	0.0,	-16.0,	0.0,	1.57,	0.0		}
,	{SHLDR_P_TO_R_L,		0.0,	0.0,	-16.0,	0.0,	1.57,	0.0		}
,	{SHLDR_R_TO_ELBOW_R,	60.0,	0.0,	16.0,	-1.57,	0.0,	1.57	}
,	{SHLDR_R_TO_ELBOW_L,	60.0,	0.0,	16.0,	-1.57,	0.0,	1.57	}
,	{ELBOW_TO_HAND_R,		129.0,	0.0,	0.0,	0.0,	0.0,	0.0		}
,	{ELBOW_TO_HAND_L,		129.0,	0.0,	0.0,	0.0,	0.0,	0.0		}

};

dVect DARwInOP_joints[] = 
{
//			ID,				Tx,		Ty,		Tz,		Rx,		Ry,		Rz
	{ID_R_SHOULDER_PITCH,	0.0,	0.0,	0.0,	1.0,	0.0,	0.0		}
,	{ID_L_SHOULDER_PITCH,	0.0,	0.0,	0.0,	1.0,	0.0,	0.0		}
,	{ID_R_SHOULDER_ROLL,	0.0,	0.0,	0.0,	0.0,	1.0,	0.0		}
,	{ID_L_SHOULDER_ROLL,	0.0,	0.0,	0.0,			}
,	{ID_R_ELBOW,			0.0,	0.0,	0.0,			}
,	{ID_L_ELBOW,			0.0,	0.0,	0.0,			}
,	{ID_R_HIP_YAW,			0.0,	0.0,	0.0,			}
,	{ID_L_HIP_YAW,			0.0,	0.0,	0.0,			}
,	{ID_R_HIP_ROLL,			0.0,	0.0,	0.0,			}
,	{ID_L_HIP_ROLL,			0.0,	0.0,	0.0,			}
,	{ID_R_HIP_PITCH			0.0,	0.0,	0.0,			}
,	{ID_L_HIP_PITCH,		0.0,	0.0,	0.0,			}
,	{ID_R_KNEE,				0.0,	0.0,	0.0,			}
,	{ID_L_KNEE,				0.0,	0.0,	0.0,			}
,	{ID_R_ANKLE_PITCH,		0.0,	0.0,	0.0,			}
,	{ID_L_ANKLE_PITCH,		0.0,	0.0,	0.0,			}
,	{ID_R_ANKLE_ROLL,		0.0,	0.0,	0.0,			}
,	{ID_L_ANKLE_ROLL,		0.0,	0.0,	0.0,			}
,	{ID_HEAD_PAN,			0.0,	0.0,	0.0,			}
,	{ID_HEAD_PITCH,			0.0,	0.0,	0.0,			}
#ifdef BOT_HAS_HANDS
,	{ID_R_GRIPPER,			0.0,	0.0,	0.0,			}
,	{ID_L_GRIPPER,			0.0,	0.0,	0.0,			}
#endif
#ifdef BOT_HAS_WRISTS
,	{ID_R_WRIST,			0.0,	0.0,	0.0,			}
,	{ID_L_WRIST,			0.0,	0.0,	0.0,			}
#endif
};
*/

DARwInOP::DARwInOP()
{
	numPhysRoDoF_ = NUMBER_OF_JOINTS;
	physLimits_ = DARwInOP_servos;

	neuRoDoFs = 

	numLinks_ = NUMBER_OF_LINKS;
	links_ = DARwInOP_links;
}

DARwInOP::~DARwInOP()
{
}

DARwInOP::Initialize()
{
//	uint64_t iter;
//	for (iter=0; iter<numPhysRoDoF_; iter++)
//	{
//	
//	}
	
	
}
