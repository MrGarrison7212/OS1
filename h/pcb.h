#ifndef H_PCB_H
#define H_PCB_H

#include "thread.h"
#include "idlethr.h"


#define LOCK asm { pushf; cli; }
#define UNLOCK asm { popf; }

#ifdef BCC_BLOCK_IGNORE
#undef LOCK
#define LOCK
#undef UNLOCK
#define UNLOCK
#endif

void tick();

typedef void interrupt (*pInterrupt)(...);

class PCB {
public:
	PCB(Thread * thread, StackSize stackSize, Time timeSlice);
	~PCB();
	static void dispatch();
	void start();
	void waitToComplete();
	static void setTimer();
	static void unsetTimer();
	static pInterrupt replaceInterrupt(unsigned char num, pInterrupt intr);
	ID getId() { return id; }
	static ID getRunningId() { return running->id; }
	static Thread* getThreadById(ID id);

private:
	friend class KernelEv;
	friend int main(int argc, char*argv[]);
	friend class KernelSem;
	static void interrupt timer();
	StackSize stackSize;
	Time timeSlice;
	Thread* thread;
	unsigned bp, ss, sp;
	static PCB* running;
	static int changeRunning;
	unsigned* stack;
	static void runwrapper();

	static int notStarted;
	static int ready;
	static int blocked;
	static int completed;

	int status;

	struct WTCElem {

		PCB* pcb;
		WTCElem* next;
	};
	WTCElem* first;

	static pInterrupt prevTimer;
	static Time timeRunning;

	int wokenUpByTimer;

	static IdleThread* idleThread;

	static void signalSem();

	static ID totalId;
	ID id;

	struct AllElem {

		PCB* pcb;
		AllElem* next;
	};
	static AllElem* allFirst;

};
#endif
