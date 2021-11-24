#include "thread.h"
#include "pcb.h"


Thread::Thread(StackSize stackSize, Time timeSlice){

	myPCB = new PCB(this, stackSize, timeSlice);

}

void dispatch(){

	LOCK
	PCB::dispatch();
	UNLOCK
}

Thread::~Thread(){

	waitToComplete();
	delete myPCB;

}

void Thread::start() {

	myPCB->start();

}

void Thread::waitToComplete() {

	myPCB->waitToComplete();

}

ID Thread::getId() {

	return myPCB->getId();

}

ID Thread::getRunningId() {

	return PCB::getRunningId();

}

Thread* Thread::getThreadById(ID id) {

	return PCB::getThreadById(id);

}

