#ifndef H_KERNELSE_H_
#define H_KERNELSE_H_

typedef unsigned int Time;
class PCB;


class KernelSem {

	friend class PCB;

public:
	KernelSem(int init = 1);
	~KernelSem();
	int wait(Time maxTimeToWait);
	void signal();
	int val() const;
	void timer();
private:
	int value;

	struct PCBElem {

		PCB* pcb;
		Time timeToWait;
		PCBElem* next;
	};

	PCBElem* firstInf;
	PCBElem* lastInf;
	PCBElem* firstLim;


	struct SemElem {

		KernelSem* sem;
		SemElem* next;

	};

	static SemElem* firstSem;

};



#endif
