#ifndef JHO_MIDI_DEBUG_h
#define JHO_MIDI_DEBUG_h

#include "MidiTracer.h"

extern "C++"
{
	namespace midi_tracer
	{
		void handler(uint8_t a, uint8_t b, uint8_t c)
		{
			::MidiSend3((midi_device_t) MIDI_DEVICE_USB_DEVICE, 1, a, b, c);
		}

		enum { MaxMessages = ::MidiTracer::MsgLen * 32 };
		uint16_t buffer_[MaxMessages];
		::MidiTracer tracer_(buffer_, MaxMessages, handler, 50);
	}
}

#define TRACE_Q31(key, value) midi_tracer::tracer_.addI(key, '1', value)
#define TRACE_Q30(key, value) midi_tracer::tracer_.addI(key, '2', value)
#define TRACE_Q29(key, value) midi_tracer::tracer_.addI(key, '3', value)
#define TRACE_Q28(key, value) midi_tracer::tracer_.addI(key, '4', value)
#define TRACE_Q27(key, value) midi_tracer::tracer_.addI(key, '5', value)
#define TRACE_Q26(key, value) midi_tracer::tracer_.addI(key, '6', value)
#define TRACE_Q25(key, value) midi_tracer::tracer_.addI(key, '7', value)
#define TRACE_Q21(key, value) midi_tracer::tracer_.addI(key, 'a', value)

#define TRACE_I(key, value)   midi_tracer::tracer_.addI(key, 'i', value)
#define TRACE_U(key, value)   midi_tracer::tracer_.addU(key, 'u', value)
#define TRACE_F(key, value)  midi_tracer::tracer_.addF(key, value)

#endif
