#include "ivtentry.h"
#include "pcb.h"
#include "kernelev.h"

IVTEntry* IVTEntry::mapById[256];


void IVTEntry::registerEv(KernelEv* ev) {

	this->ev = ev;

}

void IVTEntry::unregisterEv() {

	registerEv(0);

}

IVTEntry* IVTEntry::getById(IVTNo ivtNo) {

	return mapById[ivtNo];

}

void IVTEntry::signal() {

	if(shouldCallPrevIntr) {

		if(prevIntr) {

			prevIntr();
		}
	}

	if(ev) {

		ev->signal();
	}

	dispatch();

}

IVTEntry::IVTEntry(IVTNo ivtNo, int shouldCallPrevIntr, pInterrupt newIntr)
  : ivtNo(ivtNo)
  , shouldCallPrevIntr(shouldCallPrevIntr)
  , prevIntr(0)
  , ev(0)

{
	LOCK
	prevIntr = PCB::replaceInterrupt(ivtNo, newIntr);
	mapById[ivtNo] = this;
	UNLOCK
}

IVTEntry::~IVTEntry() {

	LOCK
	if(prevIntr) {

		prevIntr();

	}

	mapById[ivtNo] = 0;
	PCB::replaceInterrupt(ivtNo, prevIntr);
	UNLOCK

}
