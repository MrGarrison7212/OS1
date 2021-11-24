#include "pcb.h"
#include <dos.h>
#include "schedule.h"
#include "kernelse.h"

PCB* PCB::running = 0;
int PCB::changeRunning = 0;
int PCB::notStarted = 0;
int PCB::ready = 1;
int PCB::blocked = 2;
int PCB::completed = 3;
pInterrupt PCB::prevTimer = 0;
Time PCB::timeRunning = 0;
IdleThread* PCB::idleThread = 0;
ID PCB::totalId = 1;
PCB::AllElem* PCB::allFirst = 0;


PCB::PCB(Thread* thread, StackSize stackSize, Time timeSlice)
 : thread(thread)
 , stackSize(stackSize)
 , timeSlice(timeSlice)
 , bp(0)
 , ss(0)
 , sp(0)
 , stack(0)
 , status(notStarted)
 , first(0)
 , wokenUpByTimer(0)
 , id(totalId++)
{
 LOCK
 AllElem* newElem = new AllElem();
 newElem->pcb = this;
 newElem->next = allFirst;
 allFirst = newElem;
 UNLOCK
}


PCB::~PCB(){

	LOCK
	delete[] stack;
	UNLOCK

}

void PCB::dispatch(){

	LOCK
	changeRunning = 1;
	timer();
	UNLOCK

}

void interrupt PCB::timer(){

	if (!changeRunning) {

		prevTimer();

		if (running->timeSlice) {

			if(++timeRunning >= running->timeSlice) {

				changeRunning = 1;
			}
		}

		signalSem();

		tick();

	}

	if (changeRunning) {

		changeRunning = 0;

    static unsigned tss, tsp, tbp;

#ifndef BCC_BLOCK_IGNORE
	asm {
		mov tbp , bp
		mov tss , ss
		mov tsp , sp
	}
#endif

	running->bp = tbp;
	running->ss = tss;
	running->sp = tsp;

	if(!(running->status == notStarted) && !(running->status == blocked) && !(running->status == completed) && !(running == idleThread->getPCB())) {

	Scheduler::put(running);

	}

	running = Scheduler::get();

	if(!running) {

		running = idleThread->getPCB();

	}

	tbp = running->bp;
	tss = running->ss;
	tsp = running->sp;

#ifndef BCC_BLOCK_IGNORE
	asm {
		mov bp, tbp
		mov ss, tss
		mov sp, tsp
	}
#endif

	timeRunning = 0;

	}
}

void PCB::start(){

	LOCK
	if(!(status == notStarted)) {

		UNLOCK
		return;

	}

	status = ready;

	StackSize size = stackSize / 2;
	stack = new unsigned[size];



	stack[size - 1] = 0x200;
#ifndef BCC_BLOCK_IGNORE
	stack[size - 2] = FP_SEG(&PCB::runwrapper);
	stack[size - 3] = FP_OFF(&PCB::runwrapper);

	unsigned* top = stack + size - 12;
	bp = FP_OFF(top);
	ss = FP_SEG(top);
	sp = FP_OFF(top);
#endif

	Scheduler::put(this);
	UNLOCK

}



void PCB::waitToComplete() {

	LOCK
	if(!(this->completed == this->status)) {

		if(this->notStarted == this->status) {
			UNLOCK
			return;
		}

		if(this == running) {
			UNLOCK
			return;
		}

		if(this == idleThread->getPCB()){
			UNLOCK
			return;
		}

		WTCElem* elem = new WTCElem();
		elem->pcb = running;
		elem->next = first;
		first = elem;

		running->status = blocked;
		dispatch();

	}
	UNLOCK
}

void PCB::setTimer(){

	prevTimer = replaceInterrupt(8, (pInterrupt)&PCB::timer);

}

void PCB::unsetTimer(){

	replaceInterrupt(8, prevTimer);

}

pInterrupt PCB::replaceInterrupt(unsigned char num, pInterrupt intr){

	LOCK
	pInterrupt prev = 0;
#ifndef BCC_BLOCK_IGNORE
	prev = getvect(num);
	setvect(num, intr);
#endif
	UNLOCK
	return prev;


}

void PCB::runwrapper() {

	if(running && running->thread)
			running->thread->run();

	LOCK
	WTCElem* elem = running->first;
	while(elem) {

		running->first = running->first->next;
		elem->pcb->status = ready;
		Scheduler::put(elem->pcb);
		delete elem;
		elem = running->first;

	}

	running->status = completed;
	dispatch();
	UNLOCK

}

void PCB::signalSem() {
	KernelSem::SemElem* cur = KernelSem::firstSem;
	while(cur) {

		cur->sem->timer();
		cur = cur->next;

	}
}

Thread* PCB::getThreadById(ID id) {

	LOCK
	AllElem* cur = allFirst;
	while(cur != 0 && cur->pcb->id != id) {

		cur = cur->next;

	}
	if(cur == 0) {

		UNLOCK
		return 0;
	}

	Thread* t = cur->pcb->thread;
	UNLOCK
	return t;

}




