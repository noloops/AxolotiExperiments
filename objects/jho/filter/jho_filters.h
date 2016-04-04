#ifndef JHO_FILTERS_h
#define JHO_FILTERS_h

#include <math.h>

struct jho_ParaEqPars
{
	int32_t pitch; 
	int32_t reso; 
	int32_t gain; 
	int32_t level;
};

__attribute__ ( ( always_inline ) ) __STATIC_INLINE void jho_clear(jho_ParaEqPars &o)
{
	o.pitch = o.reso = o.gain = o.level = 0;
}

__attribute__ ( ( always_inline ) ) __STATIC_INLINE bool jho_checkChanged(jho_ParaEqPars &o, 
										int32_t pitch, int32_t reso, int32_t gain, int32_t level)
{
	if (o.pitch == pitch && 
		o.reso == reso && 
		o.gain == gain && 
		o.level == level) {
			return false;
	}
    o.pitch = pitch; 
	o.reso = reso; 
	o.gain = gain; 
	o.level = level;
	return true;
}

__attribute__ ( ( always_inline ) ) __STATIC_INLINE float jho_sPar2c(int i)
{
	static const float k = 0.5f / 0x08000000;
	return k * float(i) + 0.5f;
}

__attribute__ ( ( always_inline ) ) __STATIC_INLINE float jho_uPar2c(int i)
{
	static const float k = 1.0f / 0x08000000;
	return k * float(i);
}

static float jho_tan2w(int32_t pitch)
{
	int32_t w;
	MTOF(pitch,w);
	w = w >> 2;
	q31_t sinW = arm_sin_q31(w); // >> 4
	q31_t cosW = arm_cos_q31(w); // >> 4
	return ((float)sinW) / ((float)cosW);
}

// c1 = Q, c2 = gain, c3 = level
static __attribute__ ((noinline)) void jho_calcPeakCombi_1(const jho_ParaEqPars &o, biquad_coefficients &res)
{
	float t1 = jho_tan2w(o.pitch);

	float c1 = jho_uPar2c(o.reso);
	float c2 = jho_sPar2c(o.gain);
	float c3 = jho_uPar2c(o.level);

	c1 = 0.7f;
	c2 = 0.2f;
	c3 = 0.3f;

	float t2=10*c1+0.1;
	float t3=1-c2;
	float t4=c2+0.01;
	float t5=1+(t4*t1/t2)+t1*t1;
	// calculate coefficients
	float a0=c3*(1+(t3*t1/t2)+t1*t1)/t5;
	float a1=c3*(2*(t1*t1-1))/t5;
	float a2=c3*(1-(t3*t1/t2)+t1*t1)/t5;
	float b1=(2*(t1*t1-1))/t5;
	float b2=(1-(t4*t1/t2)+t1*t1)/t5;

	// ONEQ31
	res.cyn_1 = (int32_t)(b1 * (1<<24));
	res.cyn_2 = (int32_t)(b2 * (1<<24));
	res.cxn_0 = (int32_t)(a0 * (1<<24));
	res.cxn_1 = (int32_t)(a1 * (1<<24));
	res.cxn_2 = (int32_t)(a2 * (1<<24));
};

#endif
