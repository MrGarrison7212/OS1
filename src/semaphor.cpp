#include "semaphor.h"
#include "kernelse.h"


Semaphore::Semaphore(int init) {

	myImpl = new KernelSem(init);

}

Semaphore::~Semaphore() {

	delete myImpl;

}

int Semaphore::wait(Time maxTimeToWait) {

	return myImpl->wait(maxTimeToWait);

}

void Semaphore::signal() {

	myImpl->signal();

}

int Semaphore::val() const {

	return myImpl->val();

}


