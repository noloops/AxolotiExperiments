#ifndef JHO_TRACE_h
#define JHO_TRACE_h

#include "jho_trace_config.h"

#ifndef JHO_TRACE_ENABLED
#define TRACE_START
#define TRACE_STOP
#undef JHO_TRACE_QIU_COUNT
#undef JHO_TRACE_FLOAT_COUNT
#else
#define TRACE_START { jho_trace_start(); }
#define TRACE_STOP  { jho_trace_stop(); }
static bool jho_trace_collect = false;
static bool jho_trace_selected = false;
#endif // JHO_TRACE_ENABLED

#define TRACE_STARTN
#define TRACE_SIG(var) TRACE_Q(0,var)
#define TRACE_I(var) TRACE_Q(i,var)
#define TRACE_U(var) TRACE_Q(u,var)

// Length of buffer must be at least (decPlaces + 1)
static __attribute__ ((noinline)) int32_t jho_trace_writeDecimalPlaces(int32_t value, int binPlaces, int decPlaces, char *buffer)
{
    if (binPlaces < 10) {
        *buffer = '?';
        *++buffer = '\0';
        return 0;
    }
	if (value < 0) {
		value = -value;
	}
    uint32_t u = value;
    value >>= binPlaces;
    while (binPlaces > 28) {
        --binPlaces;
        u >>= 1; 
    }
    uint32_t mask = 0xFFFFFFFF >> (32 - binPlaces);
    char *p = buffer;
    if ((u & mask) == 0) {
        for (int i = 0; i < decPlaces; ++i)
            *(p++) = '0';
    }
    else {
        for (int i = 0; i < decPlaces; ++i) {
            u &= mask;
            u *= 10;
            *(p++) = '0' + (u >> binPlaces);
        }
    }
    *p = '\0';
    return value;
}

#ifndef JHO_TRACE_QIU_COUNT
#define TRACE_Q(n,var)
#else // JHO_TRACE_QIU_COUNT

#define TRACE_Q(n,var) { jho_trace_QIU_updateNext((var), (#n #var), (uint16_t)__LINE__); }

struct jho_trace_QIU
{
    const char *info_;
    int32_t min_;
    int32_t max_;
    int32_t first_;
    int32_t last_;
    uint16_t count_;
    uint16_t line_;
};

static __attribute__ ((noinline)) void jho_trace_QIU_update(jho_trace_QIU * me, int32_t value, const char *info, uint16_t line)
{
    if (jho_trace_collect) {
        if (me->info_ == 0) {
            me->min_ = me->max_ = me->first_ = me->last_ = value;
            me->info_ = info;
            me->line_ = line;
            me->count_ = 0;
        }
        else if (info != me->info_ || line != me->line_) {
            return;
        }
        else if (*(me->info_) == 'u') {
            if ((uint32_t)value < (uint32_t)me->min_) {
                me->min_ = value;
            }
            else if ((uint32_t)value > (uint32_t)me->max_) {
                me->max_ = value;
            }
        }
        else if (value < me->min_) {
            me->min_ = value;
        }
        else if (value > me->max_) {
            me->max_ = value;
        }
        me->last_ = value;
        me->count_++;
    }
}

static __attribute__ ((noinline)) void jho_trace_QIU_sendQ(jho_trace_QIU * me)
{
    int32_t nFirst = 0;
    int32_t nLast = 0;
    char fFirst[12] = { ' ', '\0' };
    char fLast[12] = { ' ', '\0' };
    const char *t = "?";
    switch(*(me->info_)) {
    case '0':
        nFirst = jho_trace_writeDecimalPlaces(me->first_, 21, 6, fFirst);
        nLast = jho_trace_writeDecimalPlaces(me->last_, 21, 6, fLast);
        t = "sig64";
        break;
    case '1':
        nFirst = jho_trace_writeDecimalPlaces(me->first_, 31, 9, fFirst);
        nLast = jho_trace_writeDecimalPlaces(me->last_, 31, 9, fLast);
        t = "q1.31";
        break;
    case '2':
        nFirst = jho_trace_writeDecimalPlaces(me->first_, 30, 9, fFirst);
        nLast = jho_trace_writeDecimalPlaces(me->last_, 30, 9, fLast);
        t = "q2.30";
        break;
    case '3':
        nFirst = jho_trace_writeDecimalPlaces(me->first_, 29, 8, fFirst);
        nLast = jho_trace_writeDecimalPlaces(me->last_, 29, 8, fLast);
        t = "q3.29";
        break;
    case '4':
        nFirst = jho_trace_writeDecimalPlaces(me->first_, 28, 8, fFirst);
        nLast = jho_trace_writeDecimalPlaces(me->last_, 28, 8, fLast);
        t = "q4.28";
        break;
    case '5':
        nFirst = jho_trace_writeDecimalPlaces(me->first_, 27, 8, fFirst);
        nLast = jho_trace_writeDecimalPlaces(me->last_, 27, 8, fLast);
        t = "q5.27";
        break;
    case '6':
        nFirst = jho_trace_writeDecimalPlaces(me->first_, 26, 7, fFirst);
        nLast = jho_trace_writeDecimalPlaces(me->last_, 26, 7, fLast);
        t = "q6.26";
        break;
    case '7':
        nFirst = jho_trace_writeDecimalPlaces(me->first_, 25, 7, fFirst);
        nLast = jho_trace_writeDecimalPlaces(me->last_, 25, 7, fLast);
        t = "q7.25";
        break;
    case '8':
        nFirst = jho_trace_writeDecimalPlaces(me->first_, 24, 7, fFirst);
        nLast = jho_trace_writeDecimalPlaces(me->last_, 24, 7, fLast);
        t = "q8.24";
        break;
    case '9':
        nFirst = jho_trace_writeDecimalPlaces(me->first_, 23, 6, fFirst);
        nLast = jho_trace_writeDecimalPlaces(me->last_, 23, 6, fLast);
        t = "q9.23";
        break;
    default:
        break;
    }
    LogTextMessage("\t%u\t%s\t%s\t%s%d.%s\t%s%d.%s\t%08x\t%08x\t%u", 
        me->line_, t, me->info_ + 1, 
		(me->first_ < 0) ? "-" : "", nFirst, fFirst, 
		(me->last_ < 0) ? "-" : "", nLast, fLast, 
		me->min_, me->max_, me->count_);
}

static __attribute__ ((noinline)) void jho_trace_QIU_send(jho_trace_QIU * me)
{
    switch(*(me->info_)) {
        case 'i':
            LogTextMessage("\t%u\tint\t%s\t%d\t%d\t%08x\t%08x\t%u", 
                me->line_, me->info_+1, me->first_, me->last_, me->min_, me->max_, me->count_);
            break;
        case 'u':
            LogTextMessage("\t%u\tuint\t%s\t%u\t%u\t%08x\t%08x\t%u", 
                me->line_, me->info_+1, me->first_, me->last_, me->min_, me->max_, me->count_);
            break;
        default:
            jho_trace_QIU_sendQ(me);
            break;
    }
}

static jho_trace_QIU jho_trace_QIU_instances[JHO_TRACE_QIU_COUNT];
static jho_trace_QIU* jho_trace_QIU_next = jho_trace_QIU_instances;

static __attribute__ ((noinline))  void jho_trace_QIU_updateNext(int32_t value, const char *info, uint16_t line)
{
    if (jho_trace_selected && jho_trace_QIU_next < (jho_trace_QIU_instances + JHO_TRACE_QIU_COUNT)) {
        jho_trace_QIU_update(jho_trace_QIU_next, value, info, line);
        ++jho_trace_QIU_next;
    }
}

#endif // JHO_TRACE_QIU_COUNT


#ifndef JHO_TRACE_FLOAT_COUNT
#define TRACE_FLOAT(var)
#else // JHO_TRACE_FLOAT_COUNT

#define TRACE_FLOAT(var) { jho_trace_Float_updateNext((var), (#var), (uint16_t)__LINE__); }

struct jho_trace_F
{
    int32_t e;
    const char *a;
    char b[12];
};

static __attribute__ ((noinline))  void jho_trace_F_init(struct jho_trace_F * me, float r)
{
    Float_t f;
    f.f = r;
    if (f.i == 0) {
        me->e = 0;
        me->b[0] = '0';
        me->b[1] = '\0';
        me->a = "0.";
    }
    me->a = (f.parts.sign) ? "-1." : "1.";

	if (0 != jho_trace_writeDecimalPlaces(f.parts.mantissa, 23, 6, me->b)) {
        me->a = "?.";
	}
    
    me->e = f.parts.exponent - 127;
}

struct jho_trace_Float
{
    const char *info_;
    float min_;
    float max_;
    float first_;
    float last_;
    uint16_t count_;
    uint16_t line_;
};

void jho_trace_Float_update(jho_trace_Float * me, float value, const char *info, uint16_t line)
{
    if (jho_trace_collect) {
        if (me->info_ == 0) {
            me->min_ = me->max_ = me->first_ = me->last_ = value;
            me->info_ = info;
            me->line_ = line;
            me->count_ = 0;
        }
        else if (info != me->info_ || line != me->line_) {
            return;
        }
        else if (value < me->min_) {
            me->min_ = value;
        }
        else if (value > me->max_) {
            me->max_ = value;
        }
        me->last_ = value;
        me->count_++;
    }
}

void jho_trace_Float_send(jho_trace_Float * me)
{
    jho_trace_F first, last, min, max;
    jho_trace_F_init(&first, me->first_);
    jho_trace_F_init(&last, me->last_);
    jho_trace_F_init(&min, me->min_);
    jho_trace_F_init(&max, me->max_);

    LogTextMessage("\t%u\tfloat\t%s\t=%s%s*2^%d\t=%s%s*2^%d\t=%s%s*2^%d\t=%s%s*2^%d\t%u",
        me->line_, me->info_,
        first.a, first.b, first.e,
        last.a, last.b, last.e,
        min.a, min.b, min.e,
        max.a, max.b, max.e,
        me->count_);
}

static jho_trace_Float jho_trace_Float_instances[JHO_TRACE_FLOAT_COUNT];
static jho_trace_Float* jho_trace_Float_next = jho_trace_Float_instances;

static __attribute__ ((noinline))  void jho_trace_Float_updateNext(float value, const char *info, uint16_t line)
{
    if (jho_trace_selected && (jho_trace_Float_next < (jho_trace_Float_instances + JHO_TRACE_FLOAT_COUNT))) {
        jho_trace_Float_update(jho_trace_Float_next, value, info, line);
        ++jho_trace_Float_next;
    }
}

#endif // JHO_TRACE_FLOAT_COUNT


#ifdef JHO_TRACE_ENABLED

static __attribute__ ((noinline))  void jho_trace_reset()
{
#ifdef JHO_TRACE_QIU_COUNT
	for (int i = 0; i < JHO_TRACE_QIU_COUNT; ++i) {
		jho_trace_QIU_instances[i].info_ = 0;
	}
#endif // JHO_TRACE_QIU_COUNT

#ifdef JHO_TRACE_FLOAT_COUNT
	for (int i = 0; i < JHO_TRACE_FLOAT_COUNT; ++i) {
		jho_trace_Float_instances[i].info_ = 0;
	}
#endif // JHO_TRACE_FLOAT_COUNT
}

static __attribute__ ((noinline))  void jho_trace_start()
{
	jho_trace_selected = true;
#ifdef JHO_TRACE_QIU_COUNT
	jho_trace_QIU_next = jho_trace_QIU_instances;
#endif
#ifdef JHO_TRACE_FLOAT_COUNT
    jho_trace_Float_next = jho_trace_Float_instances;
#endif
}

static __attribute__ ((noinline))  void jho_trace_stop()
{
    jho_trace_selected = false;
}

static __attribute__ ((noinline))  void jho_trace_logHeader()
{
    LogTextMessage("\tLINE\tTYPE\tNAME\tFIRST_VAL\tLAST_VAL\tMIN\tMAX\tCOUNT");
}

static __attribute__ ((noinline))  void jho_trace_sendNext()
{
#ifdef JHO_TRACE_QIU_COUNT
	for (int i = 0; i < JHO_TRACE_QIU_COUNT; ++i) {
		if (jho_trace_QIU_instances[i].info_ == 0) {
			continue;
		}
		jho_trace_QIU_send(jho_trace_QIU_instances + i);
		jho_trace_QIU_instances[i].info_ = 0;
		return;
	}
#endif // JHO_TRACE_QIU_COUNT

#ifdef JHO_TRACE_FLOAT_COUNT
	for (int i = 0; i < JHO_TRACE_FLOAT_COUNT; ++i) {
		if (jho_trace_Float_instances[i].info_ == 0) {
			continue;
		}
		jho_trace_Float_send(jho_trace_Float_instances + i);
		jho_trace_Float_instances[i].info_ = 0;
		return;
	}
#endif // JHO_TRACE_FLOAT_COUNT
}
#endif // JHO_TRACE_ENABLED

#endif
