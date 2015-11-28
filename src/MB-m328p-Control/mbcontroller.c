#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

#define FOSC 8000000
#define BAUD 1200
#define MYUBRR FOSC/16/BAUD-1

void USART_Init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
unsigned char USART_Receive(void);

void sendString(char msg[]);
void sendByte(unsigned char snd);
void sendPacket(unsigned char snd);

void initDevice(void);
int ledToggle(void);

void rfListen(void);

int greenLED = 0;
int redLED = 0;

int rxAlert = 0;

unsigned char devID = 0x44;

int main(void)
{
	unsigned char currentRegister;

	int txread = 0;

	USART_Init(MYUBRR);
	initDevice();

	PORTD = 4; //green only is on for power

	char buffer[20];

	while (1)
	{
		sendString("console-> ");
		currentRegister = USART_Receive();
		PORTD &= ~(1u << 2);
		USART_Transmit(currentRegister);
		sendString("\r\n");
		_delay_ms(10);
		PORTD |= 1u << 2;

		switch (currentRegister)
		{
		case 'w':
			sendString("USB Radio Frequency Transmitter / Receiver\r\n");
			sendString("Input 'h' for list of commands\r\n");
			break;
		case 'h':
			sendString("Command Glossary:\r\n");
			sendString("w - Device Information\r\n");
			sendString("h - Display this help message\r\n");
			sendString("t - Configure LED states\r\n");
			sendString("x - Activate TX transmittion mode\r\n");
			sendString("c - Command Mode\r\n");
			sendString("~ - Activate tuning mode\r\n");
			break;
		case 0xAA:
			sendString("Receiver picked up a signal\r\nPlease hold...\r\n");
			break;
		case 't':
			if (ledToggle() == 1)
				sendString("Exception occurred.. possible RF input\r\n");
			sprintf(buffer, "Green:%d\r\nRed:%d\r\n", greenLED, redLED);
			sendString(buffer);
			break;
		case 'x':
			sendString("Transmit data. Begin with 't'. '`' to stop input\r\n");
			txread = 1;

			PORTB = 2;

			while (txread == 1)
			{
				currentRegister = USART_Receive();
				if (redLED == 0)
					PORTD &= ~(1u << 2);
				if (!(currentRegister == '`'))
					sendByte(currentRegister);
				else
					txread = 0;
				if (redLED == 0)
					PORTD |= 1u << 2;
			}

			PORTB = 0;
			_delay_ms(100);
			sendString("\r\nTransmission complete.\r\n");

			break;
		case 'c':
			sendString("Command Mode\r\n");
			txread = 1;

			PORTB = 2;
			while (txread == 1)
			{
				currentRegister = USART_Receive();
				if (!(currentRegister == '`'))
				{
					for (int i = 0; i < 2; i++)
					{
						if (redLED == 0)
							PORTD &= ~(1u << 2);

						sendPacket(currentRegister);

						if (redLED == 0)
							PORTD |= 1u << 2;

						_delay_ms(10);
					}
				}
				else
					txread = 0;
			}

			break;
		case 'r':
			sendString("Enabling RX Mode. 'v' to disable.\r\n");
			PORTC = 1;
			PORTB |= 1u;
			break;
		case 'v':
			sendString("Disabling RX Mode\r\n");
			PORTC = 0;
			PORTB &= ~(1u);
			break;
		case 'l':
			sendString("RX Decoder/Listener. '`' to exit.\r\n");
			rfListen();
			break;
		case '~':
			sendString("Tuning mode. Restart to exit...\r\n");
			txread = 1;

			PORTB = 2;

			char counter;

			while (txread == 1)
			{
				for (counter = 48; counter < 123; counter++)
				{
					PORTD &= ~(1u << 2);
					sendByte(counter);
					PORTD |= 1u << 2;
					_delay_ms(10);
				}
			}
			break;
		default:
			sendString("Invalid input. 'h' for a list of commands\r\n");
			break;
		}

		if (rxAlert == 1)
		{
			PORTD &= ~(1u << 2);
			_delay_ms(50);
			rxAlert = 0;
		}

		if (greenLED == 4)
		{
			PORTD &= ~(1u << 3);
		}
		else if (greenLED == 3)
		{
			PORTD |= 1u << 3;
		}
		else if (greenLED == 1)
			PORTD |= 1u << 3;
		else
			PORTD = 4;
		if (redLED == 4)
		{
			PORTD &= ~(1u << 2);
		}
		else if (redLED == 3)
		{
			PORTD |= 1u << 2;
		}
		else
			PORTD |= 1u << 2;
	}
	return 0;
}

void rfListen(void)
{
	unsigned char receivedByte = USART_Receive();

	unsigned char addr;
	unsigned char data;
	unsigned char chksum;

	int recStat = 0;
	while (!(recStat == 7))
		;
	{
		receivedByte = USART_Receive();
		switch (recStat)
		{
		case 0:
			addr = receivedByte;
			recStat++;
			break;
		case 1:
			data = receivedByte;
			recStat++;
			break;
		case 2:
			chksum = receivedByte;
			break;
		}
		if ((receivedByte != '`') && (addr == 0x44))
		{
			sendString("Received from correct address 0x44\r\n");
			if (data + addr == chksum)
			{
				sendString("Good Data received: ");
				USART_Transmit(data);
				sendString("\r\n");
			}
			else
				sendString("Bad checksum\r\n");
		}
		else if (receivedByte == '`')
		{
			sendString("Received stop '`' signal\r\n");
			recStat = 7;
		}
		else
			sendString("Data received from incorrect address\r\n");
	}
}

void initDevice(void)
{
	DDRD = 0xFF;

	return;
}

void sendString(char msg[])
{
	int len = strlen(msg);
	for (int i = 0; i < len; i++)
	{
		USART_Transmit(msg[i]);
	}
	return;
}

void sendByte(unsigned char snd)
{
	if (greenLED == 1)
		PORTD &= ~(1u << 3);
	for (int i = 0; i < 30; i++)
	{
		USART_Transmit(0xAA);
	}
	USART_Transmit(devID);
	USART_Transmit(snd);
	USART_Transmit(devID + snd);
	if (greenLED == 1)
		PORTD |= 1u << 3;
	return;
}

void sendPacket(unsigned char snd)
{
	if (greenLED == 1)
		PORTD &= ~(1u << 3);
	for (int i = 0; i < 30; i++)
	{
		USART_Transmit(0xAA);
		//_delay_ms(10);
	}
	USART_Transmit(devID);
	//_delay_ms(10);
	USART_Transmit(devID);
	//_delay_ms(10);
	USART_Transmit(snd);
	//_delay_ms(10);
	//USART_Transmit(devID + snd);
	USART_Transmit(~snd);
	//_delay_ms(10);
	USART_Transmit(~devID);
	if (greenLED == 1)
		PORTD |= 1u << 3;
	return;
}

int ledToggle(void)
{
	sendString("Toggle LED Usage:\r\n");
	sendString("'p' : Green LED represents Power\r\n");
	sendString("'t' : Green LED represents TX\r\n");
	sendString("'r' : Red LED represents RX\r\n");
	sendString("'c' : Allow for Manual LED Control\r\n");
	switch (USART_Receive())
	{
	case 'p':
		greenLED = 0;
		break;
	case 't':
		greenLED = 1;
		PORTD |= 1u << 3;
		break;
	case 'r':
		redLED = 0;
		break;
	case 'c':
		sendString("Input 'g' or 'r' to control, then 0 or 1:\r\n");
		switch (USART_Receive())
		{
		case 'g':
			greenLED = USART_Receive() - '0' + 3;
			break;
		case 'r':
			redLED = USART_Receive() - '0' + 3;
			break;
		}
		break;
	}

	return 0;
}

void USART_Init(unsigned int ubrr)
{
	UBRR0H = (unsigned char) (ubrr >> 8);
	UBRR0L = (unsigned char) ubrr;

	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);

	return;
}

void USART_Transmit(unsigned char data)
{
	while (!(UCSR0A & (1 << UDRE0)))
		;
	UDR0 = data;

	return;
}

unsigned char USART_Receive(void)
{
	while (!(UCSR0A & (1 << RXC0)))
		;
	return UDR0;
}
