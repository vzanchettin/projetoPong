# Define o nome do projeto/binário que será gerado.
TARGET = potenciometro

# Define os fontes do projeto.
SOURCES += \
	main.c

# Modelo do microcontrolador.
#  Arduino Uno  = atmega328p
#  Arduino Mega = atmega2560
MCU = atmega2560

# Frequência do cristal.
F_CPU = 16000000UL

# Opções do programador.
#  PG 	   = arduino (Arduino Uno) | wiring (Arduino Mega/ADK)
#  PG_PORT = porta de comunicação
#  PG_BAUD = velocidade de gravação
PG		= wiring
PG_PORT	= /dev/ttyACM0
PG_BAUD	= 115200

# Outras configurações.
CC		= avr-gcc
CP		= cp
DEL 	= rm
OBJCOPY = avr-objcopy

CFLAGS  = -Os -Wall -Wextra -g -std=gnu99 -DF_CPU=$(F_CPU)
ARCH	= -mmcu=$(MCU)

OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET).hex

%.hex: %.elf
	$(OBJCOPY) -R .eeprom -j .text -j .data -O ihex $< $@

$(TARGET).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(ARCH) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $(ARCH) -o $@ $(CCFLAGS) $<

clean:
	$(DEL) -f *.o *.elf *.hex

prog: $(TARGET).hex
	sudo avrdude -D -c $(PG) -p $(MCU) -P $(PG_PORT) -b $(PG_BAUD) -U flash:w:$<
