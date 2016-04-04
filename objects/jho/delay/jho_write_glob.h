#ifndef JHO_WRITE_GLOB_h
#define JHO_WRITE_GLOB_h

#define JHO_WRITE_GLOB_SIZE 13

// 8  => 256 (5.33ms)
// 9  => 512 (10.66ms)
// 10 => 1024 (21.33ms)
// 11 => 2048 (42.66ms)
// 12 => 4096 (85.33ms)
// 13 => 8192 (170ms)
// 14 => 16384 (341ms)
// 15 => 32768 (682ms)

static const uint32_t JHO_WRITE_GLOB_LENGTHPOW = (JHO_WRITE_GLOB_SIZE);
static const uint32_t JHO_WRITE_GLOB_LENGTH = (1<<JHO_WRITE_GLOB_SIZE);
static const uint32_t JHO_WRITE_GLOB_LENGTHMASK = ((1<<JHO_WRITE_GLOB_SIZE)-1);
static int16_t jho_write_glob_array[1<<JHO_WRITE_GLOB_SIZE];
static uint32_t jho_write_glob_writepos = 0;

static inline int32_t joh_dlyNext(uint32_t *readIndex_q15, int32_t shift_q15)
{
	*readIndex_q15 += shift_q15;
	int32_t a = *readIndex_q15 >> 15;
	int32_t b = a+1;
	a &= JHO_WRITE_GLOB_LENGTHMASK;
	b &= JHO_WRITE_GLOB_LENGTHMASK;
	a = jho_write_glob_array[a]<<14;
	b = jho_write_glob_array[b]<<14;
	int32_t wa = *readIndex_q15 & ((1<<15)-1);
	int32_t wb = ((1<<15)-1) - wa;
	wa <<= 16;
	wb <<= 16;
	int32_t res = ___SMMUL(a,wa);
	res = ___SMMLA(b,wb,res);
	res <<= 1;
	return res;
}

#endif
