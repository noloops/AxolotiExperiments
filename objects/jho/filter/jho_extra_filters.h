#ifndef JHO_EXTRA_FILTERS_h
#define JHO_EXTRA_FILTERS_h

#include "axoloti_filters.h"
#include "../disp/jho_trace_disabled.h"

#define JHO_ONE(qn) (1 << (32-(qn)))
/*

LPF:        H(s) = 1 / (s^2 + s/Q + 1)

            a0 =   1 + alpha
            a1 =  -2*cos(w0)
            a2 =   1 - alpha
            b0 =  (1 - cos(w0))/2
            b1 =   1 - cos(w0)
            b2 =  (1 - cos(w0))/2
*/

static __attribute__ ((noinline)) void jho_biquad_lp_coefs(
    biquad_coefficients *coefs, uint32_t filter_W0, uint32_t q_inv) {
  filter_W0 = filter_W0 >> 1;
  if (filter_W0 > (INT32_MAX / 4))
    filter_W0 = INT32_MAX / 4;
//    filter_W0 = filter_W0<<2;
  int32_t sinW0 = arm_sin_q31(filter_W0); //i1
  int32_t cosW0 = arm_cos_q31(filter_W0); //i1
//    int32_t sinW0 = fsini(filter_W0);
//    int32_t cosW0 = fsini(filter_W0+(INT32_MAX>>2));
  int32_t alpha = ___SMMUL(sinW0, q_inv); //i2
//    int32_t alpha = sinW0>>8;
  float filter_a0 = (HALFQ31 + alpha); //i2
  float filter_a0_inv = ((INT32_MAX >> 2) / filter_a0);
  int32_t a0_inv_q31 = (int32_t)(INT32_MAX * filter_a0_inv); //i2

  coefs->cyn_1 = ___SMMUL((-cosW0), a0_inv_q31); //i4 *2
  coefs->cyn_2 = ___SMMUL((HALFQ31 - alpha), a0_inv_q31); //i4
  coefs->cxn_0 = ___SMMUL(___SMMUL(HALFQ31 - (cosW0 >> 1), a0_inv_q31), q_inv); //i4 /2
  coefs->cxn_1 = coefs->cxn_0 << 1; //i4
  coefs->cxn_2 = coefs->cxn_0;
}

__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t jho_invert(int32_t x)
{
  float r = (INT32_MAX >> 2) / (float(x));
  return (int32_t)(INT32_MAX * r);	
}

__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t jho_invert_i(int32_t one, int32_t x)
{
  float r = float(one) / float(x);
  return (int32_t)(one * r);	
}

/* Notch filter - see http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt

notch:      H(s) = (s^2 + 1) / (s^2 + s/Q + 1)

            a0 =   1 + alpha
            a1 =  -2*cos(w0)
            a2 =   1 - alpha
            b0 =   1
            b1 =  -2*cos(w0)
            b2 =   1
*/
void __attribute__ ((noinline)) jho_biquad_notch_coefs(biquad_coefficients *coefs,
                                                uint32_t filter_W0,
                                                uint32_t q_inv) {
//  (constant 0 dB peak gain)
  filter_W0 = filter_W0 >> 1;
  if (filter_W0 > (INT32_MAX / 4))
    filter_W0 = INT32_MAX / 4;
//    filter_W0 = filter_W0<<2;

  int32_t sinW0 = arm_sin_q31(filter_W0); //q31
  int32_t cosW0 = arm_cos_q31(filter_W0); //q31
//    int32_t sinW0 = fsini(filter_W0);
//    int32_t cosW0 = fsini(filter_W0+(INT32_MAX>>2));
  int32_t alpha = ___SMMUL(sinW0, q_inv); //q30
//    int32_t alpha = sinW0>>8;
  int32_t a0_inv_q31 = jho_invert(HALFQ31 + alpha); //q30

  coefs->cyn_1 = ___SMMUL((-cosW0), a0_inv_q31); //q28 *2
  coefs->cyn_2 = ___SMMUL((HALFQ31 - alpha), a0_inv_q31); //q28
  coefs->cxn_0 = ___SMMUL(HALFQ31, a0_inv_q31); //q28
  coefs->cxn_1 = coefs->cyn_1;
  coefs->cxn_2 = coefs->cxn_0;
}

/*
peakingEQ:  H(s) = (s^2 + s*(A/Q) + 1) / (s^2 + s/(A*Q) + 1)

            a0 =   1 + alpha/A
            a1 =  -2*cos(w0)
            a2 =   1 - alpha/A
            b0 =   1 + alpha*A
            b1 =  -2*cos(w0)
            b2 =   1 - alpha*A

BPF:        H(s) = (s/Q) / (s^2 + s/Q + 1)      (constant 0 dB peak gain)

            a0 =   1 + alpha
            a1 =  -2*cos(w0)
            a2 =   1 - alpha
            b0 =   alpha
            b1 =   0
            b2 =  -alpha
*/

struct jho_biquad_para_Pars
{
	inline jho_biquad_para_Pars() : pitch_(INT32_MAX >> 4), reso_(INT32_MAX >> 6), gain_(0) {}
	int32_t pitch_, reso_, gain_;

	inline bool update(int32_t pitch, int32_t reso, int32_t gain) {
		if (pitch_ == pitch && reso_ == reso && gain_ == gain) {
			return false;
		}
		pitch_ = pitch;
		reso_ = reso; 
		gain_ = gain;
		return true;
	}
};

void __attribute__ ((noinline)) jho_biquad_para_coefs(biquad_coefficients *coefs,
													  const jho_biquad_para_Pars &pars) {
TRACE_START
	uint32_t filter_W0;
	MTOF(pars.pitch_,filter_W0);
	uint32_t q_inv = INT_MAX - (__USAT(pars.reso_,27)<<4);
	int32_t gain = pars.gain_ << 4;

//  (constant 0 dB peak gain)
  filter_W0 = filter_W0 >> 1;
  if (filter_W0 > (INT32_MAX / 4))
    filter_W0 = INT32_MAX / 4;
//    filter_W0 = filter_W0<<2;

  int32_t sinW0 = arm_sin_q31(filter_W0); //i1
  int32_t cosW0 = arm_cos_q31(filter_W0); //i1
//    int32_t sinW0 = fsini(filter_W0);
//    int32_t cosW0 = fsini(filter_W0+(INT32_MAX>>2));
  int32_t alpha = ___SMMUL(sinW0, q_inv); //i2
//    int32_t alpha = sinW0>>8;

  gain >>= 1; // range -0.5 .. 0.5
  TRACE_Q(1, gain);
  float exp       = float(gain) / float(INT32_MAX);
  //TRACE_FLOAT(exp);
  float a_f       = powf(10.0f, exp);            // range 0.32 .. 3.16
  //TRACE_FLOAT(a_f);
  int32_t a_3     = (int32_t)(JHO_ONE(3) * a_f);
  TRACE_Q(3, a_3);
  int32_t a_inv_3 = (int32_t)(JHO_ONE(3) / a_f);
  TRACE_Q(3, a_inv_3);

  int32_t alphaM = ___SMMUL(alpha, a_3) << 2; //i3
  int32_t alphaD = ___SMMUL(alpha, a_inv_3) << 2; //i3

  int32_t a0_inv_3 = jho_invert_i(JHO_ONE(3), JHO_ONE(3) + alphaD); //i3

  /*
  gain = ConvertFloatToFrac(1.7f); //i11
  gain <<= 9; //i2
  int32_t alphaM = ___SMMUL(alpha, gain); //i3
  int32_t alphaD = jho_invert(gain); //i2
  alphaD = ___SMMUL(alpha, alphaD); //i2

  int32_t a0_inv_q31 = jho_invert(HALFQ31 + alphaD); //i2
  */

  coefs->cyn_1 = ___SMMUL((-cosW0), a0_inv_3) << 1; //i4 *2
  coefs->cyn_2 = ___SMMUL((JHO_ONE(3) - alphaD), a0_inv_3) << 2; //i4
  coefs->cxn_0 = ___SMMUL(JHO_ONE(3) + alphaM, a0_inv_3) << 2; //i4
  coefs->cxn_1  = coefs->cyn_1;
  coefs->cxn_2 = ___SMMUL(JHO_ONE(3) - alphaM, a0_inv_3) << 2; //i4
  TRACE_STOP
}

#endif
