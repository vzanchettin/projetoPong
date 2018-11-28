#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

volatile int xBola, yBola;

/* Sentido da bola 0(esqueda), 1(direita) 
   0(subindo) e 1(descendo) */
volatile int sentidoBolaX = 1;
volatile int sentidoBolaY = 1;

ISR(TIMER3_COMPA_vect){

	/* Caminhar da bola*/
	if (xBola == 39) {

		sentidoBolaX = 0;

	} else if (xBola == 0) {

		sentidoBolaX = 1;

	}

	if (yBola == 28) {

		sentidoBolaY = 1;

	} else if (yBola == 1) {

		sentidoBolaY = 0;

	}

	desenharBola(xBola, yBola, 7);

	if (sentidoBolaX == 1) {

		xBola++;

	} else if (sentidoBolaX == 0) {

		xBola--;

	}

	if (sentidoBolaY == 0) {

		yBola++;

	} else if (sentidoBolaY == 1) {

		yBola--;

	}

	desenharBola(xBola, yBola,0);
	

}

// Configuração do timer3
void init_timer3(int tempo) {

	//Configurando os registradores do timer
	TCCR3A = 0x00;
	TCCR3B = 0x00;
	TCCR3C = 0x00;

	//Precisa disso porque a contagem é muito rápida (aproximandamente 4 ms para contar de 0 até FFFF)
	TCCR3B |= (1 << 2) | (1 << 0);

	//Configura o timer para modo CTC
	TCCR3B |= (1 << 3);

	//Habilitar a interrupção de overflow do Timer 1 (quando chegar no valor máximo)
	TIMSK3 = 1 << 1;

    // Os ciclos definem o tempo da interrupção casa ciclo equivale a 100ms
	OCR3A = (1562*tempo);
}

void usart_init0(void) {
	//Configurar o baud rate (taxa de transmissao e recepção dos dados pela porta serial)
	int baud = 103;

	//Dois registradores uma com parte alta e outra com parte baixa
	UBRR0H = baud >> 8; //Desloca os 8 primeiros bits
	UBRR0L = baud & 0x00FF; //Zera a parte alta

	/* Habilitar o modo da USART: transmissao e recepção dos dados  */
	UCSR0B = (1 << 3) | (1 << 4);

	//Para compatibilidade com futuros dispositivos colocamos esse bit com 0
	//Zeramos esse bit para garantir que somente a interrupção guardará
	// 1 quando tiver a primeira interrupção
	//UCSR0A &= ~(1 << 5);

	//Habilita as interrupções de recebimento
	UCSR0B |= (1 << 7);

	/* Configurar o tamanho dos frames, número de bits de parada e a paridade

		frame: 8 bits
		sem paridade
		1 bit de parada (stop bit)
	 */
	 UCSR0C = (1 << 2) | (1 << 1);

	 //UCSR0B |= (1 << RXCIE0);

	// sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed
}
void usart_init1(int taxa) {
	//Configurar o baud rate (taxa de transmissao e recepção dos dados pela porta serial)
	unsigned short baud = ((F_CPU / 16) / (taxa) - 1);

	//Dois registradores uma com parte alta e outra com parte baixa
	//unsigned short ubrr0 = ((F_CPU / 16) / (baud) - 1);
	UBRR1H = baud >> 8; //Desloca os 8 primeiros bits
	UBRR1L = baud & 0x00FF; //Zera a parte alta

	/* Habilitar o modo da USART: transmissao e recepção dos dados  */
	UCSR1B = (1 << 3) | (1 << 4);

	//Para compatibilidade com futuros dispositivos colocamos esse bit com 0
	//Zeramos esse bit para garantir que somente a interrupção guardará
	// 1 quando tiver a primeira interrupção
	//UCSR0A &= ~(1 << 5);

	//Habilita as interrupções de recebimento
	UCSR1B |= (1 << 7);

	/* Configurar o tamanho dos frames, número de bits de parada e a paridade
		frame: 8 bits
		sem paridade
		1 bit de parada (stop bit)
	 */
	 UCSR1C = (1 << 2) | (1 << 1);

	 //UCSR1B |= (1 << RXCIE0);

	  sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed
}

void usart_putc1(char c){
	/* Verificar se pode enviar um novo dado */
	while (!(UCSR1A & (1 << UDRE1)));

	UDR1 = c;

    // Enquanto a transmissao não estiver finalizada
	while (!(UCSR1A & (1 << TXC1)));
}

char get_char0(void){
	//Verifica se tem uma informacao para ler
	if (UCSR0A & (1 << 7)) {
		return UDR0;
	}

	//Senao existem dados para ler retorna -1
	return -1;
}

void usart_putc0(char c){
	/* Verificar se pode enviar um novo dado */
	while (!(UCSR0A & (1 << UDRE0)));

	UDR0 = c;

    // Enquanto a transmissao não estiver finalizada
	while (!(UCSR0A & (1 << TXC0)));
}

void usart_puts0(char *str)
{
	while (*str) {
		usart_putc0(*str++);
	}
}

void usart_puts1(char *str)
{
	while (*str) {
		usart_putc1(*str++);
	}
}

ISR(USART1_RX_vect){
	 
	 /* Pegar o retorno do UDR1*/
}

/* Fução para pintar a tela*/
void corTela(int bordas, int centro) {
	usart_putc1(0x26);
	usart_putc1(0x78);
 	usart_putc1(centro & 0xFF); // de 0 á 6


	usart_putc1(0x26);
	usart_putc1(0x72);
	usart_putc1(0 & 0xFF); // x
	usart_putc1(1 & 0xFF); // y
	usart_putc1(28 & 0xFF); // h
	usart_putc1(40 & 0xFF); // w
	usart_putc1(bordas & 0xFF); // Color

}

/* Desenha a bola*/
void desenharBola(int x, int y, int cor) {

    usart_putc1(0x26);
	usart_putc1(0x72);
	usart_putc1(x & 0xFF); // x
	usart_putc1(y & 0xFF); // y
	usart_putc1(1 & 0xFF); // h
	usart_putc1(1 & 0xFF); // w
	usart_putc1(cor & 0xFF); // Color

	xBola = x;
	yBola = y;

}

void desenharPayer(int player, int y, int a, int l) {
	
	int x = 1;

	if (player == 1) {

		x = 1;
	} else {
		x = 38;
	}

    usart_putc1(0x26);
	usart_putc1(0x72);
	usart_putc1(x & 0xFF); // x
	usart_putc1(y & 0xFF); // y
	usart_putc1(a & 0xFF); // h
	usart_putc1(l & 0xFF); // w
	usart_putc1(0x04); // Color
}

int main(void){
	usart_init0(); // Inicializa a porta serial 0
	usart_init1(9600);//  Inicializa a porta serial 1

	/* Pinta o fundo e as bordas centro e bordas */
	corTela(7,4);
	
	/* Desenha a bola inicial */
	desenharBola(3,8,0);

	/* Desenha o player */
	desenharPayer(1, 13, 5, 1);
	desenharPayer(2, 13, 5, 1);

	/* Inicializa o timer em 100ms */
	init_timer3(1);
	sei();

	while (1) {

	}

	return 0;
}