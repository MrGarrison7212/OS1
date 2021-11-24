#ifndef H_IVTENTRY_H_
#define H_IVTENTRY_H_


typedef unsigned char IVTNo;
class KernelEv;
typedef void interrupt (*pInterrupt)(...);

class IVTEntry {

public:
	void signal();
	IVTEntry(IVTNo ivtNo, int shouldCallPrevIntr, pInterrupt newIntr);
	~IVTEntry();

private:
	friend class KernelEv;
	void registerEv(KernelEv* ev);
	void unregisterEv();
	static IVTEntry* getById(IVTNo ivtNo);

	static IVTEntry* mapById[];
	KernelEv* ev;
	IVTNo ivtNo;
	int shouldCallPrevIntr;
	pInterrupt prevIntr;
};


#endif
