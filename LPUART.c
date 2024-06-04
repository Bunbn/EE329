/*
 * LPUART.c
 *
 *  Created on: May 28, 2024
 *      Author: adash
 */
#include "main.h"
#include "LPUART.h"
#include "DELAY.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
int volt1;
int volt2;
int volt3;
int volt4;
int volt5;

int fingerVoltage[5] = { 0, 0, 0, 0, 0 };
char finger[5];
void LPUART_init(void) {
	PWR->CR2 |= (PWR_CR2_IOSV); // power avail on PG[15:2] (LPUART1)
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOGEN); // enable GPIOG clock
	RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN; // enable LPUART clock bridge

	// configs the registers in order: AFR, OTYPER, PUPDR, OSPEEDR, MODDER
	// configure the AFRL for PG7
	GPIOG->AFR[0] &= ~(GPIO_AFRL_AFSEL7); // reset the bits
	GPIOG->AFR[0] |= GPIO_AFRL_AFSEL7_3; // set PG7 to AF8 for LPUART1
	// configure the AFRH for PG8
	GPIOG->AFR[1] &= ~(GPIO_AFRH_AFSEL8); // reset the bits
	GPIOG->AFR[1] |= GPIO_AFRH_AFSEL8_3; // set PG7 to AF8 for LPUART1

	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOCEN);
	GPIOG->OTYPER &= ~(GPIO_OTYPER_OT7 | // PG7: TX
			GPIO_OTYPER_OT8); // PG8: RX
	GPIOG->PUPDR &= ~(GPIO_PUPDR_PUPD7 |
	GPIO_PUPDR_PUPD8); // no PUPDRs for both
	GPIOG->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED7_Pos)
			| (3 << GPIO_OSPEEDR_OSPEED8_Pos)); // maximum speed
	GPIOG->MODER &= ~(GPIO_MODER_MODE7 |
	GPIO_MODER_MODE8); // reset the GPIO ports
	GPIOG->MODER |= (GPIO_MODER_MODE7_1 |
	GPIO_MODER_MODE8_1); // alternate function mode

	// select UART pins, P/P, no PU/PD, high speed, alt. fcn mode
	// enable UART clock, set data size
	LPUART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0); // 8-bit data
	LPUART1->CR1 |= USART_CR1_UE; // enable LPUART1
	LPUART1->CR1 |= (USART_CR1_TE | USART_CR1_RE); // enable xmit & recv
	LPUART1->CR1 |= USART_CR1_RXNEIE; // enable LPUART1 recv interrupt
	LPUART1->ISR &= ~(USART_ISR_RXNE); // clear Recv-Not-Empty flag

	// LPUART clk speed is 2MHz, goal to get 115200 baud
	// 256*LPUART_CLK/BRR = baud
	// BRR calculates to 53333.333, truncate decimals
	// for 115200 baud, use 4444.444, truncate decimals
	LPUART1->BRR = 4444; // set LPUART_DIV value

	// enable UART, Tx/Rx, interrupts
	NVIC->ISER[2] = (1 << (LPUART1_IRQn & 0x1F)); // enable LPUART1 ISR
	__enable_irq(); // enable global interrupts
}

void translateBit(uint16_t bit1, uint16_t voltage1) {
	bit1 = 0;
}

//0 Closed
//1 Halfway
//2 Up
//change to uint16_t FOR ALL VOLTAGE READING max 65535
void readVoltage(uint16_t finger1, uint16_t finger2, uint16_t finger3,
		uint16_t finger4, uint16_t finger5) {
	//int voltage[5]={finger1, finger2, finger3, finger4, finger5};
	if (finger1 < 20000) {
		volt1 = 0;
	} else if (finger1 >= 20000 && finger1 < 35000) {
		volt1 = 1;
	} else if (finger1 >= 35000 && finger1 <= 65535) {
		volt1 = 2;
	} else {
		volt1 = 0; // Default case, although this will not be reached with the current ranges
	}

	if (finger2 < 20000) {
		volt2 = 0;
	} else if (finger2 >= 20000 && finger2 < 35000) {
		volt2 = 1;
	} else if (finger2 >= 35000 && finger2 <= 65535) {
		volt2 = 2;
	} else {
		volt2 = 0; // Default case
	}
	if (finger3 < 20000) {
		volt3 = 0;
	} else if (finger3 >= 20000 && finger3 < 35000) {
		volt3 = 1;
	} else if (finger3 >= 35000 && finger3 <= 65535) {
		volt3 = 2;
	} else {
		volt3 = 0; // Default case
	}
	if (finger4 < 20000) {
		volt4 = 0;
	} else if (finger4 >= 20000 && finger4 < 35000) {
		volt4 = 1;
	} else if (finger4 >= 35000 && finger4 <= 65535) {
		volt4 = 2;
	} else {
		volt4 = 0; // Default case
	}
	if (finger5 < 20000) {
		volt5 = 0;
	} else if (finger5 >= 20000 && finger5 < 35000) {
		volt5 = 1;
	} else if (finger5 >= 35000 && finger5 <= 65535) {
		volt5 = 2;
	} else {
		volt5 = 0; // Default case
	}
}

char createWord(int volt1, int volt2, int volt3, int volt4, int volt5) {
	char letter = ' ';
	if (volt1 == 0 && volt2 == 0 && volt3 == 0 && volt4 == 0 && volt5 == 0
			&& finger[5] == 0) {
		letter = 'A';
	} else if (volt1 == 1 && volt2 == 0 && volt3 == 0 && volt4 == 0
			&& volt5 == 0) {
		letter = 'B';
	} else if (volt1 == 0 && volt2 == 1 && volt3 == 0 && volt4 == 0
			&& volt5 == 0) {
		letter = 'C';
	} else if (volt1 == 0 && volt2 == 0 && volt3 == 1 && volt4 == 0
			&& volt5 == 0) {
		letter = 'D';
	} else if (volt1 == 0 && volt2 == 0 && volt3 == 0 && volt4 == 1
			&& volt5 == 0) {
		letter = 'E';
	} else if (volt1 == 0 && volt2 == 0 && volt3 == 0 && volt4 == 0
			&& volt5 == 1) {
		letter = 'F';
	} else if (volt1 == 0 && volt2 == 0 && volt3 == 0 && volt4 == 0
			&& volt5 == 0) {
		letter = 'G';
	} else if (volt1 == 0 && volt2 == 0 && volt3 == 0 && volt4 == 0
			&& volt5 == 0) {
		letter = 'H';
	} else if (volt1 == 0 && volt2 == 0 && volt3 == 0 && volt4 == 0
			&& volt5 == 0) {
		letter = 'I';
	} else if (volt1 == 0 && volt2 == 0 && volt3 == 0 && volt4 == 0
			&& volt5 == 0) {
		letter = 'J';
	} else if (volt1 == 0 && volt2 == 0 && volt3 == 0 && volt4 == 0
			&& volt5 == 0) {
		letter = 'K';
	} else if (volt1 == 0 && volt2 == 0 && volt3 == 0 && volt4 == 0
			&& volt5 == 0) {
		letter = 'L';
	} else if (volt1 == 0 && volt2 == 0 && volt3 == 0 && volt4 == 0
			&& volt5 == 0) {
		letter = 'M';
	}
	return letter;
}
void bottomHand(void){

	printColumn(29,7,":----===----------------------==-----------------");
	printColumn(30,7,":----===----------------------==-----------------");
	printColumn(31,7,":--=-------------::---------==--------------=-==-.");
	printColumn(32,7,":-::::-------------------===----------====-==+===.");
	printColumn(33,7,".::::::--------------====---------=====--=++==-=+:");
	printColumn(34,7,".::::-------------====---------==+=---===========+");
	printColumn(35,7,".:::-----------===----------====----===-==========.");
	printColumn(36,7," ::::--------==-----------==-------==-----====++++-");
	printColumn(37,7," :::---===--------------==--------==-------======++=");
	printColumn(38,7," :-------------------===---=-----==---=-----====++*+-");
	printColumn(39,7,".::::::-----------==-----==----==--=-----=-====+**+=+====++=++***.");
	printColumn(40,7,".::::::---------==-------=----===----==-======+++=+======+++++*+ ");
	printColumn(41,7," -::::-----------------===---=======--=======--====++==+++++**+ ");
	printColumn(42,7," --::::--------------========+=---=----=-==========+++++++++*+.");
	printColumn(43,7," :--::::-------------=======+======================++==+++++++");
	printColumn(44,7,"  ----::----------=========+========================++=+++**+.");
	printColumn(45,7,"  -=-:::----------===================================*++****=");
	printColumn(46,7,"   =---:----------========+==========-==========+=++++*****:");
	printColumn(47,7,"   -=-------------======-===-==========-========++++*****:");
	printColumn(48,7,"    ==---------------=--==---=----===-======++++++****+:");
	printColumn(49,7,"    .=------------------==--=---==--======++++++*****-");
	printColumn(50,7,"     :---------------=====---===========++++++*****+");
	printColumn(51,7,"      ---------=======+=+========++++++++++++*****:");
	printColumn(52,7,"      .-----=========+=++++++++++++*++++*+******+");
	printColumn(53,7,"       .==-=========+==+++***++****************.");
	printColumn(54,7,"       .=====++++++++++******#***#*********=.");

}

//Each Finger by segment, maybe case for each finger?
void handReact(int volt1, int volt2, int volt3, int volt4, int volt5) {
	//printColumn(20, 0, "");
	switch (volt1) {
	case 0:
		printColumn(14, 0, "       ");
		printColumn(15, 0, "        ");
		printColumn(16, 0, "         ");
		printColumn(17, 0, "         ");
		printColumn(18, 0, "          ");
		printColumn(19, 0, "           ");
		printColumn(20, 0, "           ");
		printColumn(21, 0, "            ");
		printColumn(22, 0, "             ");
		printColumn(23, 0, "              ");
		printColumn(24, 0, "               ");
		printColumn(25, 0, "                ");
		printColumn(26, 0, "                ");
		printColumn(27, 0, "                ");
		printColumn(28, 0, "                 ");
		break;
	case 1:
		printColumn(23, 0, "    :----==++=");
		printColumn(24, 0, "     --==----=-");
		printColumn(25, 0, "     .---:-----.");
		printColumn(26, 0, "      ::-------=");
		printColumn(27, 0, "      -----===-:");
		printColumn(28, 0, "      .--==-----:");
		break;
	case 2:
		printColumn(14, 0, "  :--=:");
		printColumn(15, 0, ".:----=-");
		printColumn(16, 0, ".------=.");
		printColumn(17, 0, " :-------");
		printColumn(18, 0, " :::------");
		printColumn(19, 0, "  :---===+.");
		printColumn(20, 0, "  :-=======");
		printColumn(21, 0, "   :--------");
		printColumn(22, 0, "   .:------=-");
		printColumn(23, 0, "    :----==++=");
		printColumn(24, 0, "     --==----=-");
		printColumn(25, 0, "     .---:-----.");
		printColumn(26, 0, "      ::-------=");
		printColumn(27, 0, "      -----===-:");
		printColumn(28, 0, "      .--==-----:");
		break;
	default:
		break;
	}
	switch (volt2) {
	case 0:
		printColumn(11, 18, "         ");
		printColumn(12, 17, "          ");
		printColumn(13, 17, "          ");
		printColumn(14, 16, "           ");
		printColumn(15, 16, "          ");
		printColumn(16, 16, "          ");
		printColumn(17, 16, "          ");
		printColumn(18, 16, "          ");
		printColumn(19, 16, "           ");
		printColumn(20, 16, "          ");
		printColumn(21, 16, "           ");
		printColumn(22, 16, "           ");
		printColumn(23, 16, "            ");
		printColumn(24, 16, "            ");
		printColumn(25, 16, "            ");
		printColumn(26, 16, "            ");
		printColumn(27, 16, "            ");
		printColumn(28, 16, "             ");
		break;
	case 1:
		printColumn(23, 16, " ---:-----=.");
		printColumn(24, 16, " ---------=:");
		printColumn(25, 16, " ----------.");
		printColumn(26, 16, "  ---------:");
		printColumn(27, 16, "  --------=-");
		printColumn(28, 16, "  -=========-");
		break;
	case 2:
		printColumn(11, 18, ".-=====- ");
		printColumn(12, 17, ".----====.");
		printColumn(13, 17, ".---=====:");
		printColumn(14, 16, "  ---=====:");
		printColumn(15, 16, " :----====");
		printColumn(16, 16, " :--======");
		printColumn(17, 16, " :--======");
		printColumn(18, 16, " :========");
		printColumn(19, 16, " .:------=.");
		printColumn(20, 16, " -------==");
		printColumn(21, 16, " -------==.");
		printColumn(22, 16, " .========+");
		printColumn(23, 16, " ---:-----=.");
		printColumn(24, 16, " ---------=:");
		printColumn(25, 16, " ----------.");
		printColumn(26, 16, "  ---------:");
		printColumn(27, 16, "  --------=-");
		printColumn(28, 16, "  -=========-");
		break;
	default:
		break;
	}
	switch (volt3) {
	case 0:
		printColumn(11, 32, "           ");
		printColumn(12, 32, "           ");
		printColumn(13, 32, "           ");
		printColumn(14, 32, "           ");
		printColumn(15, 32, "           ");
		printColumn(16, 32, "           ");
		printColumn(17, 32, "           ");
		printColumn(18, 32, "           ");
		printColumn(19, 32, "           ");
		printColumn(20, 32, "           ");
		printColumn(21, 32, "           ");
		printColumn(22, 32, "           ");
		printColumn(23, 32, "           ");
		printColumn(24, 32, "           ");
		printColumn(25, 32, "           ");
		printColumn(26, 32, "           ");
		printColumn(27, 32, "           ");
		printColumn(28, 32, "           ");
		break;
	case 1:
		printColumn(22, 32, "=---------:");
		printColumn(23, 32, "=---------:");
		printColumn(24, 32, "=---------.");
		printColumn(25, 32, "==-------- ");
		printColumn(26, 32, "-=-------- ");
		printColumn(27, 32, "-====---== ");
		printColumn(28, 32, "===---==++.");
		break;
	case 2:
		printColumn(11, 32, "  :--==-.  ");
		printColumn(12, 32, " .-------: ");
		printColumn(13, 32, " :-----=-- ");
		printColumn(14, 32, " :-----==-.");
		printColumn(15, 32, " :----=---.");
		printColumn(16, 32, " :--------.");
		printColumn(17, 32, " --======- ");
		printColumn(18, 32, ".---------.");
		printColumn(19, 32, ":--=-==---:");
		printColumn(20, 32, "===-======-");
		printColumn(21, 32, "++==--===+-");
		printColumn(22, 32, "=---------:");
		printColumn(23, 32, "=---------:");
		printColumn(24, 32, "=---------.");
		printColumn(25, 32, "==-------- ");
		printColumn(26, 32, "-=-------- ");
		printColumn(27, 32, "-====---== ");
		printColumn(28, 32, "===---==++.");
		break;
	default:
		break;
	}
	switch (volt4) {
	case 0:
		printColumn(11, 51, "      ");
		printColumn(12, 51, "       ");
		printColumn(13, 50, "         ");
		printColumn(14, 50, "         ");
		printColumn(15, 49, "          ");
		printColumn(16, 49, "          ");
		printColumn(17, 49, "          ");
		printColumn(18, 49, "          ");
		printColumn(19, 48, "          ");
		printColumn(20, 48, "          ");
		printColumn(21, 43, "               ");
		printColumn(22, 43, "              ");
		printColumn(23, 43, "              ");
		printColumn(24, 43, "              ");
		printColumn(25, 43, "             ");
		printColumn(26, 43, "             ");
		printColumn(27, 43, "            ");
		printColumn(28, 43, "            ");
		break;
	case 1:
		printColumn(22, 43, "   .+=======--");
		printColumn(23, 43, "   ==========-");
		printColumn(24, 43, "   ===------=.");
		printColumn(25, 43, "  :===-------");
		printColumn(26, 43, " .====-----=:");
		printColumn(27, 43, " -==------=-");
		printColumn(28, 43, "-===------=-");
		break;
	case 2:
		printColumn(11, 51, " :==-:");
		printColumn(12, 51, "====---");
		printColumn(13, 50, ":=------:");
		printColumn(14, 50, "==------:");
		printColumn(15, 49, ".==------:");
		printColumn(16, 49, ":=-------:");
		printColumn(17, 49, ":==--==--:");
		printColumn(18, 49, "=+=====--.");
		printColumn(19, 48, ":=--------");
		printColumn(20, 48, "==-------:");
		printColumn(21, 43, "    ===-===-=-.");
		printColumn(22, 43, "   .+=======--");
		printColumn(23, 43, "   ==========-");
		printColumn(24, 43, "   ===------=.");
		printColumn(25, 43, "  :===-------");
		printColumn(26, 43, " .====-----=:");
		printColumn(27, 43, " -==------=-");
		printColumn(28, 43, "-===------=-");
		break;
	default:
		break;
	}
	switch (volt5) {
	case 0:
		printColumn(29, 61, "                           ");
		printColumn(30, 61, "                            ");
		printColumn(31, 61, "                           ");
		printColumn(32, 61, "                        ");
		printColumn(33, 61, "                     ");
		printColumn(34, 61, "                     ");
		printColumn(35, 61, "                    ");
		printColumn(36, 61, "                  ");
		printColumn(37, 61, "                   ");
		printColumn(38, 61, "                ");
		break;
	case 1:
		printColumn(35, 61, "    :==========++++.");
		printColumn(36, 61, "   -=-======++++*=");
		printColumn(37, 61, " .==-====++++***.  ");
		printColumn(38, 61, ".======+=++***+ ");
		break;
	case 2:
		printColumn(29, 61, "               :-========-.");
		printColumn(30, 61, "            -=============-:");
		printColumn(31, 61, "         .================-");
		printColumn(32, 61, "        -===========++=.");
		printColumn(33, 61, "      -=======-===+++");
		printColumn(34, 61, "      -=======-===+++");
		printColumn(35, 61, "    :==========++++.");
		printColumn(36, 61, "   -=-======++++*=");
		printColumn(37, 61, " .==-====++++***.  ");
		printColumn(38, 61, ".======+=++***+ ");
		break;
	default:
		break;
	}

}

void LPUART_Print(const char message[]) {
	uint16_t iStrIdx = 0;
	while (message[iStrIdx] != 0) {
		while (!(LPUART1->ISR & USART_ISR_TXE))
			// wait for empty xmit buffer
			;
		LPUART1->TDR = message[iStrIdx]; // send this character
		iStrIdx++; // advance index to next char
	}

}

void LPUART_ESC_Print(const char *message) {
	while (!(LPUART1->ISR & USART_ISR_TXE)) { // wait for empty xmit buffer
		;
	}
	LPUART1->TDR = 0x1B; // send the ESC code
	LPUART_Print(message); // print the rest of the ESC sequence
}

void LPUART1_IRQHandler(void) {
	uint8_t charRecv;
	if (LPUART1->ISR & USART_ISR_RXNE) {
		charRecv =
		LPUART1->RDR;
		switch (charRecv) {
		case 0x1B: // if one of the arrows are pressed
			charRecv =
			LPUART1->RDR; // receive [
			break;
		case 'R':
			// when R is pressed, changes character color to red
			LPUART_ESC_Print("[31m");
			break;
		case 'G':
			// when G is pressed, changes character color to green
			LPUART_ESC_Print("[32m");
			break;
		case 'B':
			// when B is pressed, changes character color to blue
			LPUART_ESC_Print("[34m");
			break;
		case 'W':
			// when W is pressed, changes character color to white
			LPUART_ESC_Print("[37m");
			break;
		default:
			while (!(LPUART1->ISR & USART_ISR_TXE))
				; // wait for empty TX buffer
			LPUART1->TDR = charRecv; // echo char to terminal
		} // end switch
	}
}

void printColumn(int row, int column, const char text[]) {
	char escape[3000];
	if ((90 > row) | (row > 0)) {
		if ((90 > column) | (column > 0)) {
			snprintf(escape, sizeof(escape), "\x1B[%d;%dH", row, column);
			LPUART_ESC_Print("[H");
			while (!(LPUART1->ISR & USART_ISR_TXE)) { // wait for empty xmit buffer
				;
			}
			strcat(escape, text);
			//sprintf(escape, "\x1B[%d;%dH", row, column);
			LPUART_ESC_Print(escape);

		}
	}

}

void terminal(void) {
	printColumn(1, 0, "  ASL to English Translator  ");
	printColumn(2, 0, "------------------------------------------");
	printColumn(3, 0, "|			             	 |");
	printColumn(4, 0, "|			            	 |");
	printColumn(5, 0, "|			            	 |");
	printColumn(6, 0, "|			            	 |");
	printColumn(7, 0, "|			            	 |");
	printColumn(8, 0, "|			            	 |");
	printColumn(9, 0, "|			            	 |");
	printColumn(10, 0, "------------------------------------------");
}

void updateTerminal(char Letter) {
	static char Storage[6] = { 0 }; //Add reset for this
	static int counter = 0;

	if (counter < 6) {
		Storage[counter] = Letter;
		counter++;
	} else {
		memset(Storage, 0, sizeof(Storage)); //Clear it
		counter = 0;
		printColumn(5, 5, "      ");
		Storage[counter] = Letter;
	}
	char strLetter[2] = { Letter, '\0' };
	printColumn(3, 0, "| Your translated letter is: ");
	delay_us(100000);
	printColumn(3, 32, strLetter);
	delay_us(100000);
	printColumn(4, 0, "| So far you have:");
	delay_us(100000);
	printColumn(5, 5, Storage);
}

//Plan for two bytes, finger # and 8 bit value
//Read off voltage
//Imager wth 5 finger numbers
//Read the voltage conver tto 8 bit value analog reading
//Increase baud rate
//Switch to Ethans ADC LPUART
void hand(void) {

	printColumn(0, 0,
			"                                                                                                    ");
	printColumn(1, 0,
			"                                            -=====.                                                 ");
	printColumn(2, 0,
			"                                           :-======-                                                ");
	printColumn(3, 0,
			"                                           -========                                                ");
	printColumn(4, 0,
			"                             .:.           -========.                                               ");
	printColumn(5, 0,
			"                          :===+++-         -========.                                               ");
	printColumn(6, 0,
			"                         .-====+++.        -========.           -===-                               ");
	printColumn(7, 0,
			"                         .====++++:        -==++++==           ======-                              ");
	printColumn(8, 0,
			"                          -=====++-        =+++++=+=          :+======:                             ");
	printColumn(9, 0,
			"                          --====++=        ++++++++=          ========-                             ");
	printColumn(10, 0,
			"                          -=====+++        =========         .+=======:                             ");
	printColumn(11, 0,
			"                          -===+++++       .=========.        :+=======:                             ");
	printColumn(12, 0,
			"                          :=+++++++       :=========:        -+=======:                             ");
	printColumn(13, 0,
			"                          .--=-===+:      -=========-        =*+=+++==.                             ");
	printColumn(14, 0,
			"                           ----===+=      =+=======+-        +========                              ");
	printColumn(15, 0,
			"          -===-            --======+.     +====+=====       -+=======-                              ");
	printColumn(16, 0,
			"        .--=====           -=======++     +====+++++=       =========-                              ");
	printColumn(17, 0,
			"        :--=====:          -=======++.    **++===++*=      .+========:                              ");
	printColumn(18, 0,
			"         --======.         -=+=++++**-    +=========-      ++========.                              ");
	printColumn(19, 0,
			"         :--======         .+===++++*+    +=========-     .*+++++++==                               ");
	printColumn(20, 0,
			"         .--==+++*.         ===---===+.   +=========.     +++++=++++-                               ");
	printColumn(21, 0,
			"          :=+++====.        ==---====+:   ==========      +=========:                               ");
	printColumn(22, 0,
			"           ----=====        -=========:   ==========     -==========                                ");
	printColumn(23, 0,
			"           :-=-======       .=========:   =+========    .+=========:                                ");
	printColumn(24, 0,
			"            ----==+**+       =========-   =++====+**.   =+=========.                                ");
	printColumn(25, 0,
			"            .-=+=======      ==+=++++==-.-+++++++++++:.=+++=======-                                 ");
	printColumn(26, 0,
			"             .=----====.     :++====+=====================+++++==+:                                 ");
	printColumn(27, 0,
			"              ---=--===+     :*+++=========---=================++=                                  ");
	printColumn(28, 0,
			"               --===++==:    -=========---------==-======----=-===                                  ");
	printColumn(29, 0,
			"               .-=+====--- .=====----=-=--------=====----=========                                  ");
	printColumn(30, 0,
			"                -------=+*+=====-----=======---===================                                  ");
	printColumn(31, 0,
			"                .----=++=======----============++=================                   :=++++++++=.   ");
	printColumn(32, 0,
			"                 -==+--========-=----========++=================+=.               =++++++++++++==:  ");
	printColumn(33, 0,
			"                 :---------=======--======+++==========+++==++*++=.            :=+++++++++++++++-   ");
	printColumn(34, 0,
			"                 .-----------==========+++===--=====++++==+**+===*-           =+++++++++++**+.      ");
	printColumn(35, 0,
			"                 .-----===---======+++=======-===+++====++=====++=+         ==+++++===++**+         ");
	printColumn(36, 0,
			"                 .------=========++=-========+++=====+++===++=+++++.      :==+=+++++++***.          ");
	printColumn(37, 0,
			"                  ----=========+=-----=-===++=======++======++++***=     ====+++++***##+            ");
	printColumn(38, 0,
			"                  ----==+++=====---======++========++=========++++**+  .====+++****#%#.             ");
	printColumn(39, 0,
			"                  :==============-=====+====+=====++==========+++**#*=.++++++*+**##%*               ");
	printColumn(40, 0,
			"                  :-----------======++===========++==========++++*##+++++++*+++*#%#.                ");
	printColumn(41, 0,
			"                  .--------================+====++==========++++***+*++++++****##*                  ");
	printColumn(42, 0,
			"                   ---------==============++===++================++++*+++**++*##*                   ");
	printColumn(43, 0,
			"                   ------------=========++++===*+===+=======+++++++++*********#*:                   ");
	printColumn(44, 0,
			"                   :=---------==========++++++*++==+==++++++++++++++++*+++**##**                    ");
	printColumn(45, 0,
			"                   .=----------========++++++*+++++++++=++++++++++++++**+***##*.                    ");
	printColumn(46, 0,
			"                    -==--------======+++++++++++====++++====++++=++++++#*#####=                     ");
	printColumn(47, 0,
			"                    .+==------==========++=+*+============+==++==+*+****#####-                      ");
	printColumn(48, 0,
			"                     =+==-=================++=======+=====++++++++*#**#####:                        ");
	printColumn(49, 0,
			"                      =====================+===============+++******####*:                          ");
	printColumn(50, 0,
			"                      :====================+==+=======++++++******###%#=                            ");
	printColumn(51, 0,
			"                       -================+++====+++++++++++*****######*.                             ");
	printColumn(52, 0,
			"                        ==========+=++++*+*++++++++********#**######-                               ");
	printColumn(53, 0,
			"                        .========++++++++************#***#######%%*                                 ");
	printColumn(54, 0,
			"                         :++=+++++++++*++***##################%%#.                                  ");
	printColumn(55, 0,
			"                          :++++++*********#####%%%%%%%%%%%%%%#+.                                    ");
	printColumn(56, 0,
			"                           =*****######*#**###%%%%%@@@@@@@%%.                                       ");
	printColumn(57, 0,
			"                            *#################%%%%%%%%%%%%%#                                        ");
	printColumn(58, 0,
			"                            +**###############%%%#%%%%%%%%#*                                        ");
	printColumn(59, 0,
			"                            =+*###############%%%###%%%%###+                                        ");
	printColumn(60, 0,
			"                            .++*******###############%%%%##+                                        ");
	printColumn(61, 0,
			"                             =+***#***#########%###########=                                        ");
	printColumn(62, 0,
			"                             -+********####################:                                        ");
	printColumn(63, 0,
			"                              Welcome Press space to begin.                                         ");

}
