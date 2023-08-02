/*
	RCSwitch - Arduino libary for remote control outlet switches
	Copyright (c) 2011 Suat Ozgur.	All right reserved.

	Contributors:
	- Andre Koehler / info(at)tomate-online(dot)de
	- Gordeev Andrey Vladimirovich / gordeev(at)openpyro(dot)com
	- Skineffect / http://forum.ardumote.com/viewtopic.php?f=2&t=46
	- Dominik Fischer / dom_fischer(at)web(dot)de
	- Frank Oltmanns / <first name>.<last name>(at)gmail(dot)com
	- Andreas Steinel / A.<lastname>(at)gmail(dot)com
	- Max Horn / max(at)quendi(dot)de
	- Robert ter Vehn / <first name>.<last name>(at)gmail(dot)com
	- Johann Richard / <first name>.<last name>(at)gmail(dot)com
	- Vlad Gheorghe / <first name>.<last name>(at)gmail(dot)com https://github.com/vgheo
	- Matias Cuenca-Acuna

	Project home: https://github.com/sui77/rc-switch/
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
	Lesser General Public License for more details.
	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rc_switch.h"
#include "log.h"
#include "gpio.h"
#include "clock.h"
#include "timer.h"

static RCSWITCH_t rc_switch;

static RCSWITCH_t *RCSwitch = (&rc_switch);

typedef struct
{
	Protocol protocol;

	unsigned int first_logic_level;
	unsigned int second_logic_level;
	unsigned long send_value;
	unsigned int send_bit_len;
	unsigned int send_protocol;

	unsigned int send_step;
	unsigned int send_delay_len;
	unsigned int send_delay_cnt;
	unsigned int send_bit_cnt;

	unsigned int send_repeat;

}rc_send_nonsync_t;

enum{

	SEND_STEP_IDLE = 0,

	SEND_STEP_BIT_FIRST_LOGIC,
	SEND_STEP_BIT_SECOND_LOGIC,

	SEND_STEP_SYNC_FACTOR_FIRST_LOGIC,
	SEND_STEP_SYNC_FACTOR_SECOND_LOGIC,

	SEND_STEP_REPEAT,
};

static const Protocol proto[] = {
	{350, {1, 31}, {1, 3}, {3, 1}, false},	  // protocol 1
	{650, {1, 10}, {1, 2}, {2, 1}, false},	  // protocol 2
	{100, {30, 71}, {4, 11}, {9, 6}, false},  // protocol 3
	{380, {1, 6}, {1, 3}, {3, 1}, false},	  // protocol 4
	{500, {6, 14}, {1, 2}, {2, 1}, false},	  // protocol 5
	{450, {23, 1}, {1, 2}, {2, 1}, true},	  // protocol 6 (HT6P20B)
	{150, {2, 62}, {1, 6}, {6, 1}, false},	  // protocol 7 (HS2303-PT, i. e. used in AUKEY Remote)
	{200, {3, 130}, {7, 16}, {3, 16}, false}, // protocol 8 Conrad RS-200 RX
	{200, {130, 7}, {16, 7}, {16, 3}, true},  // protocol 9 Conrad RS-200 TX
	{365, {18, 1}, {3, 1}, {1, 3}, true},	  // protocol 10 (1ByOne Doorbell)
	{270, {36, 1}, {1, 2}, {2, 1}, true},	  // protocol 11 (HT12E)
	{320, {36, 1}, {1, 2}, {2, 1}, true}	  // protocol 12 (SM5212)
};

static volatile rc_send_nonsync_t nonsync_data = {0};

enum
{
	numProto = sizeof(proto) / sizeof(proto[0])
};

#define RC_DEBUG 1

//------------------- PORT ----------------------------

#if RC_DEBUG

#define RC_LOG(...) LOG(__VA_ARGS__)

#else

#define RC_LOG(...)

#endif

//发送接收模块IO口初始化
static void rc_switch_gpio_init(void)
{
	//发送模块电源
	hal_gpio_pin_init(P32, GPIO_OUTPUT);
	hal_gpio_pull_set(P32, GPIO_PULL_UP_S);
	hal_gpio_fast_write(P32, 0);

	//发送模块数据口
	hal_gpio_pin_init(P33, GPIO_OUTPUT);
	hal_gpio_pull_set(P33, GPIO_PULL_UP_S);
	hal_gpio_fast_write(P33, 0);

	//接收模块电源
	hal_gpio_pin_init(P26, GPIO_OUTPUT);
	hal_gpio_pull_set(P26, GPIO_PULL_UP_S);
	hal_gpio_fast_write(P26, 0);

	//接收数据口
	hal_gpio_pin_init(P31, GPIO_INPUT);
	hal_gpio_pull_set(P31, GPIO_PULL_DOWN);
}

//发送IO口初始化，配置输出
static void transmit_enable(void)
{
	//打开接收模块电源
	hal_gpio_fast_write(P32, 1);
}

static void transmit_disable(void)
{
	hal_gpio_fast_write(P32, 0);
	hal_gpio_fast_write(P33, 0);
}

//设置发送IO口输出状态
static void set_transmit_gpio_out(uint8_t status)
{
	hal_gpio_fast_write(P33, status);
}

//中断回调处理
static void receive_handle(gpio_pin_e pin, gpio_polarity_e type)
{
	handleInterrupt(NULL);
}

//接收IO初始化，双边沿中断回调函数
static void receive_enable(void)
{
	//打开接收模块电源
	hal_gpio_fast_write(P26, 1);
	
	hal_gpioin_enable(P31);
	hal_gpioin_register(P31, receive_handle, receive_handle);
}

static void receive_disable(void)
{
	hal_gpio_fast_write(P26, 0);
	hal_gpioin_unregister(P31);
}

//系统运行时间获取，us
static uint32_t sys_time_get_us(void)
{
	return hal_systick_us();
}

//延时函数，us
static void sys_delay_us(uint32_t us)
{
	WaitUs(us);
}

#define TIMER_US	10
//us定时器中断回调，参数为定时事件
static void sys_nonsync_send_handle(unsigned int us);

__ATTR_SECTION_SRAM__ static void timer_handle(uint8_t evt)
{
	if(evt == HAL_EVT_TIMER_5)
	{
		sys_nonsync_send_handle(TIMER_US);
	}
}

static void sys_us_time_init(void)
{
	hal_timer_init(timer_handle);
}

static void sys_us_time_start(void)
{
	RC_LOG("sys_us_time_start");
	hal_timer_set(AP_TIMER_ID_5, TIMER_US);
	//hal_timer_mask_int(AP_TIMER_ID_5, TRUE);
}

static void sys_us_time_stop(void)
{
	//RC_LOG("sys_us_time_stop");
	hal_timer_stop(AP_TIMER_ID_5);
	//hal_timer_mask_int(AP_TIMER_ID_5, FALSE);
}


//----------------  END PORT --------------------------

void initSwich(void)
{
	RCSwitch->nReceivedValue = 0;
	RCSwitch->nReceivedBitlength = 0;
	RCSwitch->nReceivedDelay = 0;
	RCSwitch->nReceivedProtocol = 0;
	RCSwitch->nReceiveTolerance = 60;
	RCSwitch->nSeparationLimit = 4300;

	RCSwitch->nTransmitterPin = -1;
	setRepeatTransmit(10);
	setProtocol(1);
	RCSwitch->nReceiverInterrupt = -1;
	setReceiveTolerance(60);
	RCSwitch->nReceivedValue = 0;

	rc_switch_gpio_init();
	sys_us_time_init();
	nonsync_data.send_step = SEND_STEP_IDLE;
}

/**
 * Sets the protocol to send, from a list of predefined protocols
 */
void setProtocol(int nProtocol)
{
	if (nProtocol < 1 || nProtocol > numProto)
	{
		nProtocol = 1; // TODO: trigger an error, e.g. "bad protocol" ???
	}
	RCSwitch->protocol = proto[nProtocol - 1];
}

/**
 * Sets the protocol to send with pulse length in microseconds.
 */
void setProtocolPulseLength(int nProtocol, int nPulseLength)
{
	setProtocol(nProtocol);
	setPulseLength(nPulseLength);
}

/**
 * Sets pulse length in microseconds
 */
void setPulseLength(int nPulseLength)
{
	RCSwitch->protocol.pulseLength = nPulseLength;
}

/**
 * Sets Repeat Transmits
 */
void setRepeatTransmit(int nRepeatTransmit)
{
	RCSwitch->nRepeatTransmit = nRepeatTransmit;
}

/**
 * Set Receiving Tolerance
 */
void setReceiveTolerance(int nPercent)
{
	RCSwitch->nReceiveTolerance = nPercent;
}

/**
 * Enable transmissions
 *
 * @param nTransmitterPin	 Arduino Pin to which the sender is connected to
 */
void enableTransmit(int nTransmitterPin)
{
	RCSwitch->nTransmitterPin = nTransmitterPin;
	// pinMode(this->nTransmitterPin, OUTPUT);
	// gpio_pad_select_gpio( RCSwitch->nTransmitterPin );
	transmit_enable();
}

/**
 * Disable transmissions
 */
void disableTransmit(void)
{
	RCSwitch->nTransmitterPin = -1;

	transmit_disable();
}

/**
 * Returns a char[13], representing the code word to be send.
 *
 */
char *getCodeWordA(const char *sGroup, const char *sDevice, bool bStatus)
{
	static char sReturn[13];
	int nReturnPos = 0;

	for (int i = 0; i < 5; i++)
	{
		sReturn[nReturnPos++] = (sGroup[i] == '0') ? 'F' : '0';
	}

	for (int i = 0; i < 5; i++)
	{
		sReturn[nReturnPos++] = (sDevice[i] == '0') ? 'F' : '0';
	}

	sReturn[nReturnPos++] = bStatus ? '0' : 'F';
	sReturn[nReturnPos++] = bStatus ? 'F' : '0';

	sReturn[nReturnPos] = '\0';
	return sReturn;
}

/**
 * Encoding for type B switches with two rotary/sliding switches.
 *
 * The code word is a tristate word and with following bit pattern:
 *
 * +-----------------------------+-----------------------------+----------+------------+
 * | 4 bits address							 | 4 bits address							 | 3 bits		| 1 bit			 |
 * | switch group								 | switch number							 | not used | on / off	 |
 * | 1=0FFF 2=F0FF 3=FF0F 4=FFF0 | 1=0FFF 2=F0FF 3=FF0F 4=FFF0 | FFF			| on=F off=0 |
 * +-----------------------------+-----------------------------+----------+------------+
 *
 * @param nAddressCode	Number of the switch group (1..4)
 * @param nChannelCode	Number of the switch itself (1..4)
 * @param bStatus				Whether to switch on (true) or off (false)
 *
 * @return char[13], representing a tristate code word of length 12
 */
char *getCodeWordB(int nAddressCode, int nChannelCode, bool bStatus)
{
	static char sReturn[13];
	int nReturnPos = 0;

	if (nAddressCode < 1 || nAddressCode > 4 || nChannelCode < 1 || nChannelCode > 4)
	{
		return 0;
	}

	for (int i = 1; i <= 4; i++)
	{
		sReturn[nReturnPos++] = (nAddressCode == i) ? '0' : 'F';
	}

	for (int i = 1; i <= 4; i++)
	{
		sReturn[nReturnPos++] = (nChannelCode == i) ? '0' : 'F';
	}

	sReturn[nReturnPos++] = 'F';
	sReturn[nReturnPos++] = 'F';
	sReturn[nReturnPos++] = 'F';

	sReturn[nReturnPos++] = bStatus ? 'F' : '0';

	sReturn[nReturnPos] = '\0';
	return sReturn;
}

/**
 * Like getCodeWord (Type C = Intertechno)
 */
char *getCodeWordC(char sFamily, int nGroup, int nDevice, bool bStatus)
{
	static char sReturn[13];
	int nReturnPos = 0;

	int nFamily = (int)sFamily - 'a';
	if (nFamily < 0 || nFamily > 15 || nGroup < 1 || nGroup > 4 || nDevice < 1 || nDevice > 4)
	{
		return 0;
	}

	// encode the family into four bits
	sReturn[nReturnPos++] = (nFamily & 1) ? 'F' : '0';
	sReturn[nReturnPos++] = (nFamily & 2) ? 'F' : '0';
	sReturn[nReturnPos++] = (nFamily & 4) ? 'F' : '0';
	sReturn[nReturnPos++] = (nFamily & 8) ? 'F' : '0';

	// encode the device and group
	sReturn[nReturnPos++] = ((nDevice - 1) & 1) ? 'F' : '0';
	sReturn[nReturnPos++] = ((nDevice - 1) & 2) ? 'F' : '0';
	sReturn[nReturnPos++] = ((nGroup - 1) & 1) ? 'F' : '0';
	sReturn[nReturnPos++] = ((nGroup - 1) & 2) ? 'F' : '0';

	// encode the status code
	sReturn[nReturnPos++] = '0';
	sReturn[nReturnPos++] = 'F';
	sReturn[nReturnPos++] = 'F';
	sReturn[nReturnPos++] = bStatus ? 'F' : '0';

	sReturn[nReturnPos] = '\0';
	return sReturn;
}

/**
 * Encoding for the REV Switch Type
 *
 * The code word is a tristate word and with following bit pattern:
 *
 * +-----------------------------+-------------------+----------+--------------+
 * | 4 bits address							 | 3 bits address		 | 3 bits		| 2 bits			 |
 * | switch group								 | device number		 | not used | on / off		 |
 * | A=1FFF B=F1FF C=FF1F D=FFF1 | 1=0FF 2=F0F 3=FF0 | 000			| on=10 off=01 |
 * +-----------------------------+-------------------+----------+--------------+
 *
 * Source: http://www.the-intruder.net/funksteckdosen-von-rev-uber-arduino-ansteuern/
 *
 * @param sGroup				Name of the switch group (A..D, resp. a..d)
 * @param nDevice				Number of the switch itself (1..3)
 * @param bStatus				Whether to switch on (true) or off (false)
 *
 * @return char[13], representing a tristate code word of length 12
 */
char *getCodeWordD(char sGroup, int nDevice, bool bStatus)
{
	static char sReturn[13];
	int nReturnPos = 0;

	// sGroup must be one of the letters in "abcdABCD"
	int nGroup = (sGroup >= 'a') ? (int)sGroup - 'a' : (int)sGroup - 'A';
	if (nGroup < 0 || nGroup > 3 || nDevice < 1 || nDevice > 3)
	{
		return 0;
	}

	for (int i = 0; i < 4; i++)
	{
		sReturn[nReturnPos++] = (nGroup == i) ? '1' : 'F';
	}

	for (int i = 1; i <= 3; i++)
	{
		sReturn[nReturnPos++] = (nDevice == i) ? '1' : 'F';
	}

	sReturn[nReturnPos++] = '0';
	sReturn[nReturnPos++] = '0';
	sReturn[nReturnPos++] = '0';

	sReturn[nReturnPos++] = bStatus ? '1' : '0';
	sReturn[nReturnPos++] = bStatus ? '0' : '1';

	sReturn[nReturnPos] = '\0';
	return sReturn;
}

/**
 * @param sCodeWord		a tristate code word consisting of the letter 0, 1, F
 */
void sendTriState(const char *sCodeWord)
{
	// turn the tristate code word into the corresponding bit pattern, then send it
	unsigned long code = 0;
	unsigned int length = 0;
	for (const char *p = sCodeWord; *p; p++)
	{
		code <<= 2L;
		switch (*p)
		{
		case '0':
			// bit pattern 00
			break;
		case 'F':
			// bit pattern 01
			code |= 1L;
			break;
		case '1':
			// bit pattern 11
			code |= 3L;
			break;
		}
		length += 2;
	}
	send(code, length);
}

/**
 * Transmit the first 'length' bits of the integer 'code'. The
 * bits are sent from MSB to LSB, i.e., first the bit at position length-1,
 * then the bit at position length-2, and so on, till finally the bit at position 0.
 */
void send(unsigned long code, unsigned int length)
{
	RC_LOG("RCSwitch->nTransmitterPin=%d", RCSwitch->nTransmitterPin);
	if (RCSwitch->nTransmitterPin == -1)
		return;

	// make sure the receiver is disabled while we transmit
	RC_LOG("RCSwitch->nReceiverInterrupt=%d", RCSwitch->nReceiverInterrupt);
	int nReceiverInterrupt_backup = RCSwitch->nReceiverInterrupt;
	if (nReceiverInterrupt_backup != -1)
	{
		disableReceive();
	}

	RC_LOG("RCSwitch->nRepeatTransmit=%d", RCSwitch->nRepeatTransmit);
	for (int nRepeat = 0; nRepeat < RCSwitch->nRepeatTransmit; nRepeat++)
	{
		for (int i = length - 1; i >= 0; i--)
		{
			if (code & (1L << i))
				transmit(RCSwitch->protocol.one);
			else
				transmit(RCSwitch->protocol.zero);
		}
		transmit(RCSwitch->protocol.syncFactor);
	}

	// Disable transmit after sending (i.e., for inverted protocols)
	// digitalWrite(this->nTransmitterPin, LOW);
	set_transmit_gpio_out(LOW);

	// enable receiver again if we just disabled it
	if (nReceiverInterrupt_backup != -1)
	{
		enableReceive(nReceiverInterrupt_backup);
	}
}

__ATTR_SECTION_SRAM__ static void sys_nonsync_send_handle(unsigned int us)
{
	nonsync_data.send_delay_cnt += us;

	if(nonsync_data.send_delay_cnt < nonsync_data.send_delay_len)
	{
		return;
	}

	nonsync_data.send_delay_cnt = 0;

	switch (nonsync_data.send_step)
	{
	case SEND_STEP_IDLE:
		sys_us_time_stop();
		break;

	case SEND_STEP_BIT_FIRST_LOGIC:
		
		set_transmit_gpio_out(nonsync_data.first_logic_level);

		if (nonsync_data.send_value & (1L << nonsync_data.send_bit_cnt))
		{
			nonsync_data.send_delay_len = nonsync_data.protocol.pulseLength * nonsync_data.protocol.one.high;
		}
		else
		{
			nonsync_data.send_delay_len = nonsync_data.protocol.pulseLength * nonsync_data.protocol.zero.high;
		}

		nonsync_data.send_step = SEND_STEP_BIT_SECOND_LOGIC;

		break;

	case SEND_STEP_BIT_SECOND_LOGIC:
		
		set_transmit_gpio_out(nonsync_data.second_logic_level);

		if (nonsync_data.send_value & (1L << nonsync_data.send_bit_cnt))
		{
			nonsync_data.send_delay_len = nonsync_data.protocol.pulseLength * nonsync_data.protocol.one.low;
		}
		else
		{
			nonsync_data.send_delay_len = nonsync_data.protocol.pulseLength * nonsync_data.protocol.zero.low;
		}

		if(nonsync_data.send_bit_cnt == 0)
		{
			nonsync_data.send_step = SEND_STEP_SYNC_FACTOR_FIRST_LOGIC;
		}
		else
		{
			nonsync_data.send_step = SEND_STEP_BIT_FIRST_LOGIC;

			nonsync_data.send_bit_cnt--;
		}

		break;

	case SEND_STEP_SYNC_FACTOR_FIRST_LOGIC:

		set_transmit_gpio_out(nonsync_data.first_logic_level);

		nonsync_data.send_delay_len = nonsync_data.protocol.pulseLength * nonsync_data.protocol.syncFactor.high;

		nonsync_data.send_step = SEND_STEP_SYNC_FACTOR_SECOND_LOGIC;

		break;

	case SEND_STEP_SYNC_FACTOR_SECOND_LOGIC:

		set_transmit_gpio_out(nonsync_data.second_logic_level);

		nonsync_data.send_delay_len = nonsync_data.protocol.pulseLength * nonsync_data.protocol.syncFactor.low;

		nonsync_data.send_step = SEND_STEP_REPEAT;

		break;

	case SEND_STEP_REPEAT:

		nonsync_data.send_repeat--;

		if(nonsync_data.send_repeat > 0)
		{
			nonsync_data.send_step = SEND_STEP_BIT_FIRST_LOGIC;
			nonsync_data.send_bit_cnt = nonsync_data.send_bit_len - 1;
		}
		else
		{
			nonsync_data.send_step = SEND_STEP_IDLE;
			sys_us_time_stop();
			disableTransmit();
		}

		break;
	
	default:
		break;
	}
}

unsigned int send_nonsync(unsigned long code, unsigned int length, unsigned int repeat)
{
	if (nonsync_data.send_step != SEND_STEP_IDLE)
	{
		return 0;
	}
	
	// if (RCSwitch->nTransmitterPin == -1)
	// 	return 0;

	int nReceiverInterrupt_backup = RCSwitch->nReceiverInterrupt;
	if (nReceiverInterrupt_backup != -1)
	{
		disableReceive();
	}

	enableTransmit(0);

	nonsync_data.protocol = RCSwitch->protocol;
	nonsync_data.send_value = code;
	nonsync_data.send_bit_len = length;
	nonsync_data.send_repeat = repeat;

	nonsync_data.first_logic_level = (nonsync_data.protocol.invertedSignal) ? LOW : HIGH;
	nonsync_data.second_logic_level = (nonsync_data.protocol.invertedSignal) ? HIGH : LOW;

	nonsync_data.send_bit_cnt = nonsync_data.send_bit_len - 1;
	nonsync_data.send_delay_len = 0;
	nonsync_data.send_delay_cnt = 0;

	nonsync_data.send_step = SEND_STEP_BIT_FIRST_LOGIC;

	sys_us_time_start();

	return 1;
}

/**
 * Transmit a single high-low pulse.
 */
void transmit(HighLow pulses)
{
	uint8_t firstLogicLevel = (RCSwitch->protocol.invertedSignal) ? LOW : HIGH;
	uint8_t secondLogicLevel = (RCSwitch->protocol.invertedSignal) ? HIGH : LOW;

	set_transmit_gpio_out(firstLogicLevel);
	// ets_delay_us(RCSwitch->protocol.pulseLength * pulses.high);
	sys_delay_us(RCSwitch->protocol.pulseLength * pulses.high);
	set_transmit_gpio_out(secondLogicLevel);
	// ets_delay_us(RCSwitch->protocol.pulseLength * pulses.low);
	sys_delay_us(RCSwitch->protocol.pulseLength * pulses.low);
}

/**
 * Enable receiving data
 */
void enableReceive(int interrupt)
{
	RCSwitch->nReceiverInterrupt = interrupt;
	enableReceiveInternal();
}


void enableReceiveInternal(void)
{
	uint64_t gpio_pin_sel = (1ULL << RCSwitch->nReceiverInterrupt);
	RC_LOG("RCSwitch->nReceiverInterrupt=%d gpio_pin_sel=%llu", RCSwitch->nReceiverInterrupt, gpio_pin_sel);

	receive_enable();
}

/**
 * Disable receiving data
 */
void disableReceive(void)
{
	receive_disable();
	RCSwitch->nReceiverInterrupt = -1;
}

bool available(void)
{
	return RCSwitch->nReceivedValue != 0;
}

void resetAvailable(void)
{
	RCSwitch->nReceivedValue = 0;
}

unsigned long getReceivedValue(void)
{
	return RCSwitch->nReceivedValue;
}

unsigned int getReceivedBitlength(void)
{
	return RCSwitch->nReceivedBitlength;
}

unsigned int getReceivedDelay(void)
{
	return RCSwitch->nReceivedDelay;
}

unsigned int getReceivedProtocol(void)
{
	return RCSwitch->nReceivedProtocol;
}

unsigned int *getReceivedRawdata(void)
{
	return RCSwitch->timings;
}

/* helper function for the receiveProtocol method */
static inline unsigned int diff(int A, int B)
{
	return abs(A - B);
}

bool receiveProtocol(const int p, unsigned int changeCount)
{
	const Protocol pro = proto[p - 1];

	unsigned long code = 0;
	// Assuming the longer pulse length is the pulse captured in timings[0]
	const unsigned int syncLengthInPulses = ((pro.syncFactor.low) > (pro.syncFactor.high)) ? (pro.syncFactor.low) : (pro.syncFactor.high);
	const unsigned int delay = RCSwitch->timings[0] / syncLengthInPulses;
	const unsigned int delayTolerance = delay * RCSwitch->nReceiveTolerance / 100;

	/* For protocols that start low, the sync period looks like
	 *							 _________
	 * _____________|					|XXXXXXXXXXXX|
	 *
	 * |--1st dur--|-2nd dur-|-Start data-|
	 *
	 * The 3rd saved duration starts the data.
	 *
	 * For protocols that start high, the sync period looks like
	 *
	 *	______________
	 * |							|____________|XXXXXXXXXXXXX|
	 *
	 * |-filtered out-|--1st dur--|--Start data--|
	 *
	 * The 2nd saved duration starts the data
	 */
	const unsigned int firstDataTiming = (pro.invertedSignal) ? (2) : (1);

	for (unsigned int i = firstDataTiming; i < changeCount - 1; i += 2)
	{
		code <<= 1;
		if (diff(RCSwitch->timings[i], delay * pro.zero.high) < delayTolerance &&
			diff(RCSwitch->timings[i + 1], delay * pro.zero.low) < delayTolerance)
		{
			// zero
		}
		else if (diff(RCSwitch->timings[i], delay * pro.one.high) < delayTolerance &&
				 diff(RCSwitch->timings[i + 1], delay * pro.one.low) < delayTolerance)
		{
			// one
			code |= 1;
		}
		else
		{
			// Failed
			return false;
		}
	}

	if (changeCount > 7)
	{ // ignore very short transmissions: no device sends them, so this must be noise
		RCSwitch->nReceivedValue = code;
		RCSwitch->nReceivedBitlength = (changeCount - 1) / 2;
		RCSwitch->nReceivedDelay = delay;
		RCSwitch->nReceivedProtocol = p;

		return true;
	}

	return false;
}

void handleInterrupt(void *arg)
{
	static unsigned int changeCount = 0;
	static unsigned long lastTime = 0;
	static unsigned int repeatCount = 0;

	const long time = sys_time_get_us();
	const unsigned int duration = time - lastTime;

	if (duration > RCSwitch->nSeparationLimit)
	{
		// A long stretch without signal level change occurred. This could
		// be the gap between two transmission.
		if (diff(duration, RCSwitch->timings[0]) < 200)
		{
			// This long signal is close in length to the long signal which
			// started the previously recorded timings; this suggests that
			// it may indeed by a a gap between two transmissions (we assume
			// here that a sender will send the signal multiple times,
			// with roughly the same gap between them).
			repeatCount++;
			if (repeatCount == 2)
			{
				for (uint8_t i = 1; i <= numProto; i++)
				{
					if (receiveProtocol(i, changeCount))
					{
						// receive succeeded for protocol i
						break;
					}
				}
				repeatCount = 0;
			}
		}
		changeCount = 0;
	}
	// detect overflow
	if (changeCount >= RCSWITCH_MAX_CHANGES)
	{
		changeCount = 0;
		repeatCount = 0;
	}

	RCSwitch->timings[changeCount++] = duration;
	lastTime = time;
}
