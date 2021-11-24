
#ifndef H_KERNELEV_H_
#define H_KERNELEV_H_

typedef unsigned char IVTNo;
class IVTEntry;
class PCB;

class KernelEv {

public:
	KernelEv(IVTNo ivtNo);
	~KernelEv();
	void wait();
	void signal();

private:
	IVTNo ivtNo;
	IVTEntry* ivtEntry;
	PCB* pcb;
	int val;
};


#endif
