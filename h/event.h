#ifndef _event_h_
#define _event_h_

#include "ivtentry.h"

typedef unsigned char IVTNo;

class KernelEv;

class Event {

public:
	Event (IVTNo ivtNo);
	~Event ();
	void wait ();
protected:
	friend class KernelEv;
	void signal();
private:
	KernelEv* myImpl;
};


#define PREPAREENTRY(ivtNo , shouldCallPrevIntr) \
	void interrupt intr##ivtNo(...); \
	IVTEntry entry##ivtNo(ivtNo, shouldCallPrevIntr, intr##ivtNo); \
	void interrupt intr##ivtNo(...) \
	{ \
		entry##ivtNo.signal(); \
	}


#endif
