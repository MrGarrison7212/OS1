/*
 * mainthr.h
 *
 *  Created on: Dec 18, 2017
 *      Author: OS1
 */

#ifndef H_MAINTHR_H_
#define H_MAINTHR_H_


#include "thread.h"

class MainThread : public Thread {
public:

	PCB* getPCB();

};





#endif /* H_MAINTHR_H_ */
