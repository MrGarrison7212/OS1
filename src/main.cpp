#include "mainthr.h"
#include "pcb.h"
#include "schedule.h"


int userMain(int argc, char* argv[]);


int main (int argc, char* argv[]) {

	MainThread* mainThr = new MainThread();
	PCB::running = mainThr->getPCB();
	PCB::running->status = PCB::ready;

	PCB::idleThread = new IdleThread();
	PCB::idleThread->start();
	Scheduler::get();

	PCB::setTimer();

	int result = userMain(argc, argv);

	PCB::unsetTimer();

	delete PCB::idleThread;
	delete mainThr;

	return result;
}
