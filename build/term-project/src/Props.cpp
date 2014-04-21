

#include "Props.hpp"

using namespace Robot;

void BotProperties::makeValid(RoClus& modder)
{
	uint64_t iter, jter;
	for (iter=0; iter<count; iter++)
	{
		bool matched = false;
		
		// check if even need to bother fixing
		if (!modder[iter].en)
			continue;
		
		// quick check for one-to-one correlation of arrays
		if (modder[iter].id==physLimits_[iter].id)
		{
			matched = true;

			if (modder[iter].inv)
			{
/*
				if (modder[iter].ang < _physLimits[iter].ang_min)
					modder[iter].ang = _physLimits[iter].ang_min;
				else if (modder[iter].ang > _physLimits[iter].ang_max)
					modder[iter].ang = _physLimits[iter].ang_max;

				if (modder[iter].spd < _physLimits[iter].spd_min)
					modder[iter].spd = _physLimits[iter].spd_min;
				else if (modder[iter].spd > _physLimits[iter].spd_max)
					modder[iter].spd = _physLimits[iter].spd_max;

				if (modder[iter].trq < _physLimits[iter].trq_min)
					modder[iter].trq = _physLimits[iter].trq_min;
				else if (modder[iter].trq > _physLimits[iter].trq_max)
					modder[iter].trq = _physLimits[iter].trq_max;
*/
				// temporary disable until get design right
				modder[iter].en = false;
			}
			else
			{
				if (modder[iter].ang < physLimits_[iter].ang_min)
					modder[iter].ang = physLimits_[iter].ang_min;
				else if (modder[iter].ang > physLimits_[iter].ang_max)
					modder[iter].ang = physLimits_[iter].ang_max;

				if (modder[iter].spd < physLimits_[iter].spd_min)
					modder[iter].spd = physLimits_[iter].spd_min;
				else if (modder[iter].spd > physLimits_[iter].spd_max)
					modder[iter].spd = physLimits_[iter].spd_max;

				if (modder[iter].trq < physLimits_[iter].trq_min)
					modder[iter].trq = physLimits_[iter].trq_min;
				else if (modder[iter].trq > physLimits_[iter].trq_max)
					modder[iter].trq = physLimits_[iter].trq_max;
			}
		}
		else
		{
			// exhaustive search of available entries in robot limits
			for (jter=0; jter<numPhysJoints_; jter++)
			{
				if (modder[iter].id==physLimits_[jter].id)
				{
					matched = true;

					if (modder[iter].inv)
					{
/*
						if (modder[iter].ang < _physLimits[iter].ang_min)
							modder[iter].ang = _physLimits[iter].ang_min;
						else if (modder[iter].ang > _physLimits[iter].ang_max)
							modder[iter].ang = _physLimits[iter].ang_max;

						if (modder[iter].spd < _physLimits[iter].spd_min)
							modder[iter].spd = _physLimits[iter].spd_min;
						else if (modder[iter].spd > _physLimits[iter].spd_max)
							modder[iter].spd = _physLimits[iter].spd_max;

						if (modder[iter].trq < _physLimits[iter].trq_min)
							modder[iter].trq = _physLimits[iter].trq_min;
						else if (modder[iter].trq > _physLimits[iter].trq_max)
							modder[iter].trq = _physLimits[iter].trq_max;
*/
						// temporary disable until get design right
						modder[iter].en = false;
					}
					else
					{
						if (modder[iter].ang < physLimits_[jter].ang_min)
							modder[iter].ang = physLimits_[jter].ang_min;
						else if (modder[iter].ang > physLimits_[jter].ang_max)
							modder[iter].ang = physLimits_[jter].ang_max;

						if (modder[iter].spd < physLimits_[jter].spd_min)
							modder[iter].spd = physLimits_[jter].spd_min;
						else if (modder[iter].spd > physLimits_[jter].spd_max)
							modder[iter].spd = physLimits_[jter].spd_max;

						if (modder[iter].trq < physLimits_[jter].trq_min)
							modder[iter].trq = physLimits_[jter].trq_min;
						else if (modder[iter].trq > physLimits_[jter].trq_max)
							modder[iter].trq = physLimits_[jter].trq_max;
					}
					break;
				}
			}
		}
		if (!matched)
			modder[iter].en = false;
	}
}
