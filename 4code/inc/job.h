#ifndef _JOB_H_
#define _JOB_H_

typedef enum JOB_STATE
{
	JOB_NONE = 0,
	//JOB_WAIT_BEFORE_SAMPLING = 1,
	JOB_SAMPLING = 2,
	JOB_CALCULATING = 3,
	JOB_CALCULATING_COMPLETE = 4,
} JOB_STATE;

void JobQuant();

JOB_STATE JobState();
void JobSetState(JOB_STATE state);

void JobStartSampling();
void JobSendCalculated();

#endif//_JOB_H_