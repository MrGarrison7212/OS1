#include "kernelev.h"
#include "pcb.h"
#include "ivtentry.h"
#include "schedule.h"


KernelEv::KernelEv(IVTNo ivtNo)
  : ivtNo(ivtNo)
  , ivtEntry(0)
  , pcb(0)
  , val(0)
{
	 LOCK
	 pcb = PCB::running;
	 ivtEntry = IVTEntry::getById(ivtNo);
	 ivtEntry->registerEv(this);
	 UNLOCK

}

KernelEv::~KernelEv() {

	LOCK
	ivtEntry->unregisterEv();
	UNLOCK

}

void KernelEv::wait() {

	LOCK
	if(pcb != PCB::running) {
		UNLOCK
		return;
	}

	if(--val < 0) {

		pcb->status = PCB::blocked;
		dispatch();
	}
	UNLOCK
}

void KernelEv::signal() {

	LOCK
	if(val++ < 0) {

		pcb->status = PCB::ready;
		Scheduler::put(pcb);

	}

	if(val > 1) {

		val = 1;

	}
	UNLOCK
}
