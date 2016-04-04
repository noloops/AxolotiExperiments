#ifndef MIDI_TRACER_h
#define MIDI_TRACER_h

class MidiTracer
{
public:
	typedef void (*Handler)(uint8_t a, uint8_t b, uint8_t c);
	enum { MsgLen = 3 };
	enum { MsgType = 0xA0 }; // Polyphonic aftertouch

	MidiTracer(uint16_t *buffer, uint32_t buffLen, Handler handler, uint32_t kDiv) : 
		begin_(buffer), 
		end_(buffer + buffLen), 
		handler_(handler),
		kDiv_(kDiv),
		write_(end_), 
		read_(end_), 
		index_(0),
		kCounter_(0),
		maxMessages_(0),
		startChar_(' ')
	{
	}

	void start(uint32_t maxMessages = 9999, char startChar = ' ')
	{
		maxMessages_ = maxMessages;
		startChar_ = startChar;

		write_ = begin_;
		read_ = begin_;
		index_ = 0;
		kCounter_ = 0;

		handler_(MsgType, 0, 0);
	}

	inline void addU(char key, char typeInfo, uint32_t value)
	{
		add(key, (uint8_t)typeInfo, &value);
	}

	inline void addI(char key, char typeInfo, int32_t value)
	{
		add(key, (uint8_t)typeInfo, &value);
	}

	inline void addF(char key, float value)
	{
		add(key, (uint8_t)'f', &value);
	}

	void work()
	{
		if (++kCounter_ < kDiv_)
			return;
		kCounter_ = 0;

		if (read_ >= write_)
			return;
		if (++index_ > MsgLen)
			index_ = 1;

		const uint8_t *p = (uint8_t*)read_++;
		uint8_t a = MsgType | index_;
		if (0x80 & p[0])
			a |= 0x4;
		if (0x80 & p[1])
			a |= 0x8;
		uint8_t b = 0x7F & p[0];
		uint8_t c = 0x7F & p[1];

		handler_(a, b, c);
	}

private:
	void add(char key, char typeInfo, const void* val32)
	{
		if (write_ + MsgLen > end_)
			return;
		if (write_ == begin_ && startChar_ != ' ' && key != startChar_)
			return;
		{
			uint8_t *p = (uint8_t*)(write_++);
			p[0] = (uint8_t)key;
			p[1] = (uint8_t)typeInfo;
		}
		{
			uint16_t *p = (uint16_t*)val32;
			*(write_++) = p[0];
			*(write_++) = p[1];
		}
	}

	uint16_t * const begin_;
	uint16_t * const end_;
	const Handler handler_;
	const uint32_t kDiv_;

	uint16_t *write_;
	uint16_t *read_;
	uint32_t index_;
	uint32_t kCounter_;
	uint32_t maxMessages_;
	char startChar_;
};

#endif
