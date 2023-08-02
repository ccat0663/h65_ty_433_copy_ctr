#ifndef MAIN_RCSWITCH_H_
#define MAIN_RCSWITCH_H_

#include <stdint.h>
#include <stdbool.h>

#define LOW 0
#define HIGH 1

/**
 * Description of a single pulse, which consists of a high signal
 * whose duration is "high" times the base pulse length, followed
 * by a low signal lasting "low" times the base pulse length.
 * Thus, the pulse overall lasts (high+low)*pulseLength
 */
typedef struct HighLow
{
	uint8_t high;
	uint8_t low;
} HighLow;

/**
 * A "protocol" describes how zero and one bits are encoded into high/low
 * pulses.
 */
typedef struct Protocol
{
	/** base pulse length in microseconds, e.g. 350 */
	uint16_t pulseLength;

	HighLow syncFactor;
	HighLow zero;
	HighLow one;

	/**
	 * If true, interchange high and low logic levels in all transmissions.
	 *
	 * By default, RCSwitch assumes that any signals it sends or receives
	 * can be broken down into pulses which start with a high signal level,
	 * followed by a a low signal level. This is e.g. the case for the
	 * popular PT 2260 encoder chip, and thus many switches out there.
	 *
	 * But some devices do it the other way around, and start with a low
	 * signal level, followed by a high signal level, e.g. the HT6P20B. To
	 * accommodate this, one can set invertedSignal to true, which causes
	 * RCSwitch to change how it interprets any HighLow struct FOO: It will
	 * then assume transmissions start with a low signal lasting
	 * FOO.high*pulseLength microseconds, followed by a high signal lasting
	 * FOO.low*pulseLength microseconds.
	 */
	bool invertedSignal;
} Protocol;

// Number of maximum high/Low changes per packet.
// We can handle up to (unsigned long) => 32 bit * 2 H/L changes per bit + 2 for sync
#define RCSWITCH_MAX_CHANGES 67

typedef struct
{
	unsigned long nReceivedValue;
	unsigned int nReceivedBitlength;
	unsigned int nReceivedDelay;
	unsigned int nReceivedProtocol;
	int nReceiveTolerance;
	unsigned nSeparationLimit;
	/*
	 * timings[0] contains sync timing, followed by a number of bits
	 */
	unsigned int timings[RCSWITCH_MAX_CHANGES];
	int nReceiverInterrupt;

	int nTransmitterPin;
	int nRepeatTransmit;

	Protocol protocol;
} RCSWITCH_t;

void initSwich(void);

void enableReceive(int interrupt);
void enableReceiveInternal(void);
void disableReceive(void);
bool available(void);
void resetAvailable(void);

unsigned long getReceivedValue(void);
unsigned int getReceivedBitlength(void);
unsigned int getReceivedDelay(void);
unsigned int getReceivedProtocol(void);
unsigned int *getReceivedRawdata(void);

void handleInterrupt(void *arg);

void setProtocol(int nProtocol);
void setProtocolPulseLength(int nProtocol, int nPulseLength);
void setPulseLength(int nPulseLength);
void setRepeatTransmit(int nRepeatTransmit);
void setReceiveTolerance(int nPercent);

void enableTransmit(int nTransmitterPin);
void disableTransmit(void);

char *getCodeWordA(const char *sGroup, const char *sDevice, bool bStatus);
char *getCodeWordB(int nAddressCode, int nChannelCode, bool bStatus);
char *getCodeWordC(char sFamily, int nGroup, int nDevice, bool bStatus);
char *getCodeWordD(char group, int nDevice, bool bStatus);
void sendTriState(const char *sCodeWord);
void send(unsigned long code, unsigned int length);
unsigned int send_nonsync(unsigned long code, unsigned int length, unsigned int repeat);

void transmit(HighLow pulses);

#endif /* MAIN_RXB6_RECEIVER_H_ */
