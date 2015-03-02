#include "dac.h"
#include "cs4272.h"
#include "process_sound.h"
#include "arm_math.h"
#include "delay.h"

uint16_t* DacGetBuffer();
uint16_t DacGetBufferSize();
void CalculateFft();

static uint16_t g_cur_pos = DAC_BUFFER_SIZE/2;
static uint16_t g_dma_cur_pos = 0;
static uint16_t prev_time_ms = 0;

uint16_t g_sound_quant_time = 0;

//Количество сэмплов, сгенерированных с ADC (мгновенное значение)
static uint32_t summary_adc_samples = 0;
//Количество сэмплов, воспроизведенных DAC (мгновенное значение)
static uint32_t summary_dac_samples = 0;
static uint16_t adc_cur_pos_momental = 0;
static uint16_t dac_cur_pos_momental = 0;

void OnSoundDataFft(int32_t sampleQ, int32_t sampleI);

void UpdateSummarySamples(uint16_t adc_pos);

uint16_t DacGetWritePos()
{
	return g_cur_pos;
}

uint16_t DacGetDeltaPos()
{
	uint16_t pos_in = DacGetWritePos();
	uint16_t pos_out = DacGetReadPos();
	uint16_t pos_delta;
	if(pos_out>pos_in)
	{
		pos_delta = pos_out-pos_in;
	} else
	{
		pos_delta = DAC_BUFFER_SIZE+pos_out-pos_in;
	}
	return pos_delta;
	//return (uint16_t)summary_adc_samples-(uint16_t)summary_dac_samples;
}

void DacCorrectWritePos()
{
	return;
	if(summary_adc_samples==summary_dac_samples)
	{
	} else
	if(summary_adc_samples>summary_dac_samples)
	{
		uint16_t pos_delta = summary_adc_samples-summary_dac_samples;

		if(pos_delta>1)
		{
			g_cur_pos += DAC_BUFFER_SIZE-pos_delta;
			g_cur_pos %= DAC_BUFFER_SIZE;
			summary_dac_samples = summary_adc_samples;
		}
	} else
	{
		//Тут код еще не написали, надо проверить случай, когда таймер идет быстрее
	}

}

void OnSoundData(int32_t sampleQ, int32_t sampleI)
{
	uint16_t* out_buffer = DacGetBuffer();

	int s;
	s = (sampleQ>>(14))+DAC_ZERO;
	//s = (sampleQ>>8)+DAC_ZERO;


	if(s<0)
		s = 0;
	if(s>4095)
		s=4095;
	out_buffer[g_cur_pos] = s;

	g_cur_pos = (g_cur_pos+1)%DAC_BUFFER_SIZE;
}

void CopySoundData(uint16_t start, uint16_t count)
{
	uint16_t* data4 = sound_buffer+start;
	for(int idx=0; idx<count; idx+=4, data4+=4)
	{
		int32_t sampleQ = (((int32_t)data4[0])<<16)+data4[1];
		int32_t sampleI = (((int32_t)data4[2])<<16)+data4[3];
		OnSoundData(sampleQ, sampleI);
	}
}

void SoundQuant()
{
	if(!g_i2s_dma)
		return;
	uint16_t start = TimeUs();

	uint16_t pos = cs4272_getPos();
	UpdateSummarySamples(pos);

	if(g_dma_cur_pos==pos)
	{
	} else
	if(g_dma_cur_pos<pos)
	{
		//pos = (pos+SOUND_BUFFER_SIZE-4)%SOUND_BUFFER_SIZE;
		if(g_dma_cur_pos<pos)
		{
			CopySoundData(g_dma_cur_pos, pos-g_dma_cur_pos);
			g_dma_cur_pos = pos;
		}
	} else
	{
		//pos = (pos+SOUND_BUFFER_SIZE-4)%SOUND_BUFFER_SIZE;
		if(pos<g_dma_cur_pos)
		{
			CopySoundData(g_dma_cur_pos, SOUND_BUFFER_SIZE-g_dma_cur_pos);
			CopySoundData(0, pos);
			g_dma_cur_pos = pos;
		}
	}

	//Раз в секунду корректируем положение записи, чтобы была синхронизация частоты чтением и записью.
	uint16_t cur_time = TimeMs();
	if( ((uint16_t)(cur_time-prev_time_ms))>1000)
	{
		prev_time_ms = cur_time;
		DacCorrectWritePos();
	}

	uint16_t quant_time = TimeUs()-start;
	if(quant_time>5)
		g_sound_quant_time = quant_time;
}

void UpdateSummarySamples(uint16_t adc_pos)
{
//summary_adc_samples
//summary_dac_samples
	uint16_t pos = adc_pos;
	if(adc_cur_pos_momental==pos)
	{
	} else
	if(adc_cur_pos_momental<pos)
	{
		summary_adc_samples += (pos-adc_cur_pos_momental)/4;
	} else
	{
		summary_adc_samples += (SOUND_BUFFER_SIZE-adc_cur_pos_momental)/4;
		summary_adc_samples += (pos)/4;
	}
	adc_cur_pos_momental = pos;

	pos = DacGetReadPos();

	if(dac_cur_pos_momental==pos)
	{
	} else
	if(dac_cur_pos_momental<pos)
	{
		summary_dac_samples += pos-dac_cur_pos_momental;
	} else
	{
		summary_dac_samples += DAC_BUFFER_SIZE-dac_cur_pos_momental;
		summary_dac_samples += pos;
	}

	dac_cur_pos_momental = pos;

}
