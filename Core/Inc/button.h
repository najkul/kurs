/*
 * button.h
 *
 *  Created on: Dec 20, 2021
 *      Author: jlu
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

//states for state machine
typedef enum
{
	IDLE = 0,
	DEBOUNCE,
	PRESSED,
	REPEAT,
	RELEASE,
	OFF
} BUTTON_STATE;

// structure for button
typedef struct
{
	BUTTON_STATE State;

	GPIO_TypeDef *GpioPort;
	uint16_t GpioPin;

	uint8_t NumberTogleLed;

	uint32_t LastTick;
	uint32_t TimmerDebounce;
	uint32_t TimmerLongPress;
	uint32_t TimmerRepeat;
	uint32_t TimmerTogle;

	void (*ButtonPressed)(void);
	void (*ButtonLongPressed)(void);
	void (*ButtonRepeat)(void);
	void (*ButtonReleaseTogle)(void);
	void (*ButtonOff)(void);
} TButton;

// public function
void ButtonSetDebounceTime(TButton *Keym, uint32_t mils);
void ButtonSetLongPressTime(TButton *Key, uint32_t mils);
void ButtonSetRepeatTime(TButton *Key, uint32_t mils);
void ButtonSetTogleTime(TButton *Key, uint32_t mils);

void ButtonRegisterLongPressCallback(TButton *Key, void *Callback);
void ButtonRegisterRepeatCallback(TButton *Key, void *Callback);
void ButtonRegisterPressCallback(TButton *Key, void *Callback);
void ButtonRegisterReleaseTogleCallback(TButton *Key, void *Callback);
void ButtonRegisterOffCallback(TButton *Key, void *Callback);

void ButtonTask(TButton *Key);
void ButtonInitKey(TButton *Key, GPIO_TypeDef *GpioPort, uint16_t GpioPin, uint32_t TimmerDebounce,
		uint32_t TimmerLongPress,uint32_t TimmerRepeat, uint8_t NumberTogleLed, uint32_t TimmerTogle);

#endif /* INC_BUTTON_H_ */
