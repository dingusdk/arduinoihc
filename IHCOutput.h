#ifndef _ihcoutput_h
#define _ihcoutput_h

typedef unsigned long dword;

//#define IHC_LOW 1
//#define IHC_HIGH 0

#define IHC_LOW 0
#define IHC_HIGH 1


class IHCOutput {

public:

	class IHCTemperature {

		int channel;
		float temperature;
		float humidity;
		float gulvtemp;
		bool gulvibrug;

		IHCTemperature* next;

		uint8_t data[9];
		int bitpos;
		unsigned long starttime;
			
		friend class IHCOutput;

		void Init();
		void Tick();

		void AddBits(word value, int bits = 12);

	public:
		
		IHCTemperature( int ch);

		void SetTemperature(float t) { temperature = t;  }
		void SetHumidity(float h) { humidity = h; }
		void SetSecondTemperature(float t) { gulvtemp = t; gulvibrug = true; }

		int GetChannel() { return channel; }
	};

protected:

	int pin;
	word output;

	int pulsepos;
	// a snapshow of output including the parity bit
	// a bit mask for the bit we are sending
	dword outputmask;
	dword outputp;

	IHCTemperature* FirstTemperature;
	IHCTemperature* TemperatureToProcess;

	void SendBits(int channel, int value, int bits = 12);

public:
	static IHCOutput* pTheOneAndOnly;
	void Tick();

	IHCOutput();
	virtual ~IHCOutput();

	void Begin(int pin);
	void SetOutput(word output);

	void Set(int channel, int state);
	void AddTemperature(IHCTemperature& temp);
};


extern IHCOutput TheIHCOutput;

#endif
