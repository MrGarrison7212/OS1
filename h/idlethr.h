
#ifndef H_IDLETHR_H_
#define H_IDLETHR_H_

#include "thread.h"

class IdleThread : public Thread {

public:

	PCB* getPCB();
	void run();

};

#endif
