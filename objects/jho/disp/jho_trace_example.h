#ifndef JHO_TRACE_EXAMPLE_h
#define JHO_TRACE_EXAMPLE_h

#include "jho_trace.h"

//#define JHO_ONE(qn) (INT32_MAX >> ((qn)-1))
#define JHO_ONE(qn) (1 << (32-(qn)))

void jho_trace_example(int32_t sig)
{
	TRACE_START // Start of active trace block
	TRACE_SIG(sig);
	TRACE_Q(5, sig);
	float exp       = float(sig) / JHO_ONE(5 - 1); // range -0.5 .. 0.5
	float a_f       = powf(10.0f, exp);            // range 0.32 .. 3.16
	// TRACE_FLOAT(a_f);
	int32_t a_3     = (int32_t)(JHO_ONE(3) * a_f);
	int32_t a_inv_3 = (int32_t)(JHO_ONE(3) / a_f);
	TRACE_Q(3, a_3);
	TRACE_Q(3, a_inv_3);
	TRACE_STOP
}
/*
void jho_trace_example(int32_t sig)
{
    TRACE_Q(0, sig); // Ignored (outside trace block)

	TRACE_START // Start of active trace block

	float ff = float(sig) / float(1 << 21);
	ff = powf(10.0f, ff);
	TRACE_FLOAT(ff);

	TRACE_SIG(sig);

	int32_t var_a = sig;
	TRACE_Q(5, var_a);
	var_a <<= 1;
	TRACE_Q(4, var_a);
	var_a <<= 1;
	TRACE_Q(3, var_a);
	var_a <<= 1;
	TRACE_Q(2, var_a);

	int32_t b = sig << 4;
	TRACE_Q(1, b);

	int32_t s_int = sig >> 21;
	if (s_int > 0) {
		s_int = -s_int;
	}
	int32_t u_int = -s_int;
	TRACE_I(s_int);
	TRACE_U(u_int);

	int32_t another_var = sig >> 1;
	TRACE_Q(6, another_var);
	another_var >>= 1;
	TRACE_Q(7, another_var);
	another_var >>= 1;
	TRACE_Q(8, another_var);
	another_var >>= 1;
	TRACE_Q(9, another_var);
	TRACE_STOP // End of active trace block

	TRACE_Q(0, sig); // Ignored (outside trace block)

	TRACE_STARTN // Deactivated trace block
	TRACE_Q(0, sig); // Ignored (outside active trace block)
	TRACE_STOP
}
*/
#endif
