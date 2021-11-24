#include "kernelse.h"
#include "pcb.h"
#include "schedule.h"

KernelSem::SemElem* KernelSem::firstSem = 0;


KernelSem::KernelSem(int init)
    : value(init)
    , firstInf(0)
    , lastInf(0)
    , firstLim(0)

{
	LOCK
	SemElem* elem = new SemElem();
	elem->sem = this;
	elem->next = firstSem;
	firstSem = elem;
	UNLOCK
	}


KernelSem::~KernelSem() {
	LOCK
	while(firstInf || firstLim) {

		signal();

	}

	SemElem* prev = 0;
	SemElem* cur = firstSem;
	while(cur && !(cur->sem == this)) {

		prev = cur;
		cur = cur->next;

	}
	if(cur) {

		if(prev)
			prev->next = cur->next;
		else
			firstSem = cur->next;
		delete cur;
	}
	UNLOCK
}

int KernelSem::wait(Time maxTimeToWait) {

	LOCK
	if(--value >= 0) {
		UNLOCK
		return 1;
	}

	PCB::running->wokenUpByTimer = 0;
	if(maxTimeToWait == 0) {

		if(!firstInf) {

			firstInf = lastInf = new PCBElem();
			firstInf->pcb = PCB::running;
			firstInf->timeToWait = 0;
			firstInf->next = 0;
			PCB::running->status = PCB::blocked;
			dispatch();

		}
		else {

			lastInf = lastInf->next = new PCBElem();
			lastInf->pcb = PCB::running;
			lastInf->timeToWait = 0;
			lastInf->next = 0;
			PCB::running->status = PCB::blocked;
			dispatch();
		}
	}
	else {

		PCBElem* prev = 0, *cur = firstLim, *elem = new PCBElem();
		elem->pcb = PCB::running;
		elem->timeToWait = maxTimeToWait;
		elem->next = 0;

		while (cur && elem->timeToWait >= cur->timeToWait) {

			elem->timeToWait -= cur->timeToWait;
			prev = cur;
			cur = cur->next;

		}

		if(cur) {

			elem->next = cur;
			cur->timeToWait -= elem->timeToWait;
		}

		if(prev) {

			prev->next = elem;

		}
		else {

			firstLim = elem;

		}

		PCB::running->status = PCB::blocked;
		dispatch();
	}
	UNLOCK
	return PCB::running->wokenUpByTimer == 0;

}

void KernelSem::signal() {

	LOCK
	if(value++ >= 0) {
		UNLOCK
		return;
	}

	if(firstInf) {

		PCBElem* elem = firstInf;
		firstInf = firstInf->next;
		if(elem == lastInf) {
			lastInf = 0;
			}
		elem->pcb->status = PCB::ready;
		Scheduler::put(elem->pcb);
		delete elem;
	}

	else if (firstLim) {

		PCBElem* elem = firstLim;
		firstLim = firstLim->next;
		if(firstLim) {
			firstLim->timeToWait += elem->timeToWait;
			}
		elem->pcb->status = PCB::ready;
		Scheduler::put(elem->pcb);
		delete elem;

	}
	UNLOCK
}

int KernelSem::val() const {

	return value;

}

void KernelSem::timer() {

	LOCK
	if(firstLim) {

		if(firstLim->timeToWait > 0) {

			--firstLim->timeToWait;
		}

		while(firstLim) {

			if(firstLim->timeToWait > 0) {

				break;

			}

			firstLim->pcb->status = PCB::ready;
			firstLim->pcb->wokenUpByTimer = 1;
			Scheduler::put(firstLim->pcb);
			PCBElem* elem = firstLim;
			firstLim = firstLim->next;
			delete elem;

		}

	}
	UNLOCK

}


