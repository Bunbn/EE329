/*
 * LPUART.h
 *
 *  Created on: May 28, 2024
 *      Author: adash
 */
#ifndef INC_LPUART_H_
#define INC_LPUART_H_

void LPUART1_GPIO_Init(void);
/* USER CODE END Includes */
void LPUART_init(void);
void LPUART_Print(const char message[]);
void LPUART1_IRQHandler( void  );
void LPUART_ESC_Print( const char *message );
//void LPUART1_ESC(const char message[]);
void readVoltage(uint16_t finger1, uint16_t finger2, uint16_t finger3, uint16_t finger4,
		uint16_t finger5);
void terminal(void);
char createWord(int volt1, int volt2, int volt3, int volt4, int volt5);
void handReact(int finger1, int finger2, int finger3, int finger4, int finger5);
void updateTerminal(char Letter);
void translateBit(uint16_t bit1, uint16_t voltage1);
void hand(void) ;
void bottomHand(void);
void printColumn(int row, int column, const char text[]);
void LPUART_Print_Image( uint8_t row, uint8_t arrSize, char *image[] );
#endif /* INC_LPUART_H_ */
