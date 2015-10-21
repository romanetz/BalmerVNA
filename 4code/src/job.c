#include "main.h"
#include "job.h"
#include "process_sound.h"
#include "smath.h"
#include "data_process.h"
#include "commands.h"
#include "delay.h"

static JOB_STATE g_state = JOB_NONE;

//Ожидаем WAIT_TIME_US микросекунд, прежде чем начать сэмплирование.
#define WAIT_TIME_US 1000

static uint16_t g_start_wait_time;

static float result_freq = 0;
static float result_q_cconst = 0;
static float result_q_csin = 0;
static float result_q_ccos = 0;
static float result_q_sqr = 0;
static float result_i_cconst = 0;
static float result_i_csin = 0;
static float result_i_ccos = 0;
static float result_i_sqr = 0;
static float result_time = 1234; //Время выполнения

void CalculateAll();

JOB_STATE JobState()
{
	return g_state;
}

void JobSetState(JOB_STATE state)
{
	g_state = state;
}

void JobQuant()
{
    if(g_state==JOB_WAIT_BEFORE_SAMPLING)
    {
        uint16_t delta_us = TimeUs()-g_start_wait_time;
        if(delta_us>WAIT_TIME_US)
        {
            g_state = JOB_SAMPLING;
            SamplingStart();
        }
    }

    if(g_state==JOB_SAMPLING)
	{
		if(SamplingCompleted())
		{
			JobSetState(JOB_CALCULATING);
			CalculateAll();
			JobSetState(JOB_CALCULATING_COMPLETE);	
		}
	}
}

void CalculateAll()
{
	uint16_t startTime = TimeMs();
	float step = 1.0f/48000.0f;
	float freqMin = 999.3f;
	float freqMax = 999.8f;

	float freq = 999.6f;
	float q_cconst = 1, q_csin = 2, q_ccos = 3;
	float i_cconst = 4, i_csin = 5, i_ccos = 6;
	float q_sqr = 7;
	float i_sqr = 8;
	freq = findFreqMax(g_samplesQ, SAMPLE_BUFFER_SIZE, step, freqMin, freqMax, 11);
	
	calcSinCosMatrix(g_samplesQ, SAMPLE_BUFFER_SIZE, freq, step, &q_cconst, &q_csin, &q_ccos);
	q_sqr = squareMean(g_samplesQ, SAMPLE_BUFFER_SIZE, freq, step, q_cconst, q_csin, q_ccos);

	calcSinCosMatrix(g_samplesI, SAMPLE_BUFFER_SIZE, freq, step, &i_cconst, &i_csin, &i_ccos);
	i_sqr = squareMean(g_samplesI, SAMPLE_BUFFER_SIZE, freq, step, i_cconst, i_csin, i_ccos);

	result_freq = freq;
	result_q_cconst = q_cconst;
	result_q_csin = q_csin;
	result_q_ccos = q_ccos;
	result_q_sqr = q_sqr;
	result_i_cconst = i_cconst;
	result_i_csin = i_csin;
	result_i_ccos = i_ccos;
	result_i_sqr = i_sqr;

	result_time = TimeMs() - startTime;
}

void JobSendCalculated()
{
    USBAdd8(COMMAND_GET_CALCULATED);
    USBAdd8(g_state);

    if(g_state==JOB_CALCULATING_COMPLETE)
    {
	    USBAddF(result_freq);
		USBAddF(result_q_cconst);
		USBAddF(result_q_csin);
		USBAddF(result_q_ccos);
		USBAddF(result_q_sqr);
		USBAddF(result_i_cconst);
		USBAddF(result_i_csin);
		USBAddF(result_i_ccos);
		USBAddF(result_i_sqr);
		USBAdd16(result_time);
        USBAdd16(0);
	}
    USBSend();
}

void JobStartSampling()
{
    g_state = JOB_WAIT_BEFORE_SAMPLING;
    g_start_wait_time = TimeUs();
}
