#ifndef JHO_TRACE_DISABLED_h
#define JHO_TRACE_DISABLED_h

#undef  JHO_TRACE_ENABLED
#undef JHO_TRACE_QIU_COUNT
#undef JHO_TRACE_FLOAT_COUNT

#define TRACE_START
#define TRACE_STOP
#define TRACE_STARTN

#define TRACE_SIG(var) TRACE_Q(0,var)
#define TRACE_I(var) TRACE_Q(i,var)
#define TRACE_U(var) TRACE_Q(u,var)

#define TRACE_Q(n,var)
#define TRACE_FLOAT(var)

#endif
