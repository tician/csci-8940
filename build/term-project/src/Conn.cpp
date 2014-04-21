

#include "Conn.hpp"

using namespace Robot;

void BotController::AddMod(WalkyModule *modus)
{
	modus->Initialize();
	modList_.push_back(modus);
}

void BotController::RemMod(WalkyModule *modus)
{
	modList_.remove(modus);
}

void BotController::Initialize(BotProperties* botprop)
{
	bpp_ = botprop;
	if (bpp_.getNumJoints()<1)
		return;

	offsets_ = new Joint[numJoints_];
}

bool BotController::operator < (WalkyModule& lhs, WalkyModule& rhs)
{
	return (lhs.modPriority < rhs.modPriority);
}

void BotController::Process()
{
	if (modList_.size() == 0)
		return;

	// ascending order (priority: 0==lowest, 10==high)
	modList_.sort();

	// iterate through modules
	std::list<WalkyModule*>::iterator lister;
	for (lister = modList_.begin(); lister != modList_.end(); lister++)
	{
		// run risk of not making it to highest priority module process...
		(*lister)->Process();

		// Get joint angles from module and update manager output
		int iter;
		for (iter=0; iter < (*lister)->numJoints; iter++)
		{
			
		}
	}

	// write joint angles to servos

	// grab servo angles and torques
}

void BotController::Start()
{

}

void BotController::Stop()
{

}

bool BotController::IsRunning()
{
	if (timerState_ == RUNNING)
		return true;

	return false
}

void BotController::TimerProcess(void)
{
	while (timerState_ == RUNNING)
	{
		system_clock::duration		CYCLE_TIME (duration<int>(8,1000));
		system_clock::time_point	nextTimePoint = system_clock::now() + CYCLE_TIME;
	
		this->Process();
		
		std::this_thread::sleep_until(nextTimePoint);
	}
}
void BotController::StartTimer()
{
	if (timerState_ == STOPPED)
	{
		timerThread_ = std::thread(this->TimerProcess);
		timerState_ = RUNNING;
	}
}

void BotController::StopTimer()
{
	if (timerState_ == RUNNING)
	{
		timerState_ = STOPPING;
		timerThread_.join();
		timerThread_.~thread();
		timerState_ = STOPPED;
	}
}



