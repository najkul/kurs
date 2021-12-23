/*
 * button.c
 *
 *  Created on: Dec 20, 2021
 *      Author: jlu
 */

#include "main.h"
#include "button.h"

// button init
void ButtonInitKey(TButton *Key, GPIO_TypeDef *GpioPort, uint16_t GpioPin,
		uint32_t TimmerDebounce, uint32_t TimmerLongPress,
		uint32_t TimmerRepeat, uint8_t NumberTogleLed, uint32_t TimmerTogle)
{
	Key->State = IDLE;

	Key->GpioPort = GpioPort;
	Key->GpioPin = GpioPin;

	Key->TimmerDebounce = TimmerDebounce;
	Key->TimmerLongPress = TimmerLongPress;
	Key->TimmerRepeat = TimmerRepeat;

	Key->NumberTogleLed = NumberTogleLed;
	Key->TimmerTogle = TimmerTogle;

}

// time setting functions
void ButtonSetDebounceTime(TButton *Key, uint32_t mils)
{
	Key->TimmerDebounce = mils;
}
void ButtonSetLongPressTime(TButton *Key, uint32_t mils)
{
	Key->TimmerLongPress = mils;
}
void ButtonSetRepeatTime(TButton *Key, uint32_t mils)
{
	Key->TimmerRepeat = mils;
}
void ButtonSetTogleTime(TButton *Key, uint32_t mils)
{
	Key->TimmerTogle = mils;
}

// register callbecks
void ButtonRegisterPressCallback(TButton *Key, void *Callback)
{
	Key->ButtonPressed = Callback;
}

void ButtonRegisterLongPressCallback(TButton *Key, void *Callback)
{
	Key->ButtonLongPressed = Callback;
}

void ButtonRegisterRepeatCallback(TButton *Key, void *Callback)
{
	Key->ButtonRepeat = Callback;
}

void ButtonRegisterReleaseTogleCallback(TButton *Key, void *Callback)

{
	Key->ButtonReleaseTogle = Callback;
}

void ButtonRegisterOffCallback(TButton *Key, void *Callback)
{
	Key->ButtonOff = Callback;
}

// state routing
void ButtonIdleRoutine(TButton *Key)
{
	if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(Key->GpioPort, Key->GpioPin))
	{
		Key->State = DEBOUNCE;
		Key->LastTick = HAL_GetTick();
	}
}

void ButtonBounceRoutine(TButton *Key)
{
	if ((HAL_GetTick() - Key->LastTick) > Key->TimmerDebounce)
	{
		if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(Key->GpioPort, Key->GpioPin))
		{
			Key->State = PRESSED;
			Key->LastTick = HAL_GetTick();

			if (Key->ButtonPressed != NULL)
			{
				Key->ButtonPressed();
			}
		}
		else
		{
			Key->State = IDLE;
		}
	}
}

void ButtonPressedRoutine(TButton *Key)
{
	if (GPIO_PIN_SET == HAL_GPIO_ReadPin(Key->GpioPort, Key->GpioPin))
	{
//		Key->State = IDLE;
		Key->State = RELEASE;
		Key->LastTick = HAL_GetTick();
	}
	else
	{
		if ((HAL_GetTick() - Key->LastTick) > Key->TimmerLongPress)
		{
			Key->State = REPEAT;
			Key->LastTick = HAL_GetTick();

			if (Key->ButtonLongPressed != NULL)
			{
				Key->ButtonLongPressed();
			}
		}
	}
}

void ButtonRepeatRoutine(TButton *Key)
{
	if (GPIO_PIN_SET == HAL_GPIO_ReadPin(Key->GpioPort, Key->GpioPin))
	{
//		Key->State = IDLE;
		Key->State = RELEASE;
		Key->LastTick = HAL_GetTick();
	}
	else
	{
		if ((HAL_GetTick() - Key->LastTick) > Key->TimmerRepeat)
		{
			Key->LastTick = HAL_GetTick();
			if (Key->ButtonRepeat != NULL)
			{
				Key->ButtonRepeat();
			}

		}
	}
}
void ButtonRelaseRoutine(TButton *Key)
{
	uint8_t i;
	for (i = 0; i < Key->NumberTogleLed;)
	{
		if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(Key->GpioPort, Key->GpioPin))
		{
			Key->State = IDLE;
			i=Key->NumberTogleLed;
		}

		if ((HAL_GetTick() - Key->LastTick) > Key->TimmerTogle)
		{
			Key->LastTick = HAL_GetTick();
			i++;
			if (Key->ButtonReleaseTogle != NULL)
			{
				Key->ButtonReleaseTogle();
			}
		}
	}

	Key->State = OFF;

}

void ButtonOffRoutine(TButton *Key)
{
	Key->State = IDLE;
	if (Key->ButtonOff != NULL)
	{
		Key->ButtonOff();
	}
}

// state machine
void ButtonTask(TButton *Key)
{
	switch (Key->State)
	{
	case IDLE:
		ButtonIdleRoutine(Key);
		break;

	case DEBOUNCE:
		ButtonBounceRoutine(Key);
		break;

	case PRESSED:
		ButtonPressedRoutine(Key);
		break;

	case REPEAT:
		ButtonRepeatRoutine(Key);
		break;

	case RELEASE:
		ButtonRelaseRoutine(Key);
		break;

	case OFF:
		ButtonOffRoutine(Key);
		break;
	}
}

