#include "stm32f103xb.h"
#include "stm32f1xx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define USART_RX_BUF_SIZE 256
#define USART_TX_BUF_SIZE 256

const char *initMsg[] = {"Terminal Bluepill\nSeleccione una de las opciones:\n A. Parpadeo GPIOC13\n B. Echo\nOpcion: "};

volatile uint32_t ticks = 0;

volatile char rxRingBuffer[USART_RX_BUF_SIZE];
volatile uint16_t rxHead = 0;
volatile uint16_t rxTail = 0;

volatile char txBuffer[USART_TX_BUF_SIZE];
volatile uint16_t txCount = 0;
volatile uint16_t txIndex = 0;

#define S0_STATE_MENU 0
#define S1_STATE_TOGGLE_BLINK 1
#define S2_STATE_ECHO 2
#define S3_STATE_RETURN 4

uint8_t state = 0;
uint8_t blink = 0;

void USART1_SendString(const char *string, const uint16_t size);
void __USART1_LoadNextByte(void);
uint16_t USART1_ReadString(char *buffer, uint16_t size, const char terminator);

int main() {
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;

  GPIOA->CRH &= ~(GPIO_CRH_CNF9 | GPIO_CRH_MODE9 | GPIO_CRH_CNF10 | GPIO_CRH_MODE10);
  GPIOA->CRH |= GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1 | GPIO_CRH_CNF10_1; // GPIOA9 output alternate function push-pull, GPIOA10 input pull up/down
  GPIOA->ODR |= GPIO_ODR_ODR10; // GPIOA10 pull-up
  
  GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
  GPIOC->CRH |= GPIO_CRH_MODE13_0 | GPIO_CRH_MODE13_1;

  // Configurar BRR (9600 baudios a 8 MHz)
  USART1->BRR = 0x341;
  USART1->CR1 = USART_CR1_TCIE | USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE;
  USART1->CR1 |= USART_CR1_UE; // Activa el periférico

  // Configuración del SysTick para interrupciones cada 1ms
  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / 1000);

  NVIC_EnableIRQ(USART1_IRQn);

  USART1_SendString(*initMsg, 88);

  char buffer[16];

  while(1) {
    switch(state) {
      case S0_STATE_MENU:
        if (USART1_ReadString(buffer, 16, '\n')) {
            switch(buffer[0]) {
              case 'a':
              case 'A':
                state = S1_STATE_TOGGLE_BLINK;
                break;
              case 'b':
              case 'B':
                state = S2_STATE_ECHO;
                USART1_SendString("\nEscriba SALIR para volver. MAX 16 CHARS\n", 42);
                break;
              default:
                USART1_SendString("\nOpcion no reconocida\n", 22);
                state = S3_STATE_RETURN;
                break;
          }
        }
        break;
      case S1_STATE_TOGGLE_BLINK:
        blink ^= 1;
        USART1_SendString("\n-- Blink ", 11);
        USART1_SendString(blink ? "ON " : "OFF", 4);
        USART1_SendString(" --\n", 4);
        state = S3_STATE_RETURN;
        break;
      case S2_STATE_ECHO:
        {
          uint8_t size;
          size = USART1_ReadString(buffer, 16, '\n');
          if (size) {
            if (strncmp(buffer, "SALIR", 5) == 0) {
              state = S3_STATE_RETURN;
              USART1_SendString("Saliendo...\n", 12);
            } else {
              USART1_SendString("ECHO: ", 7);
              USART1_SendString(buffer, size);
              USART1_SendString("\n", 1);
            }
          }
        }
        break;
      case S3_STATE_RETURN:
        USART1_SendString(&((*initMsg)[17]), 71);
        state = S0_STATE_MENU;
    }

    if (blink) {
      static uint32_t tickTimestamp = 0;
      if (ticks > tickTimestamp + 500) {
        if (GPIOC->IDR & GPIO_IDR_IDR13) {
          GPIOC->BSRR = GPIO_BSRR_BR13;
        } else {
          GPIOC->BSRR = GPIO_BSRR_BS13;
        }
        tickTimestamp = ticks;
      }
    }

  }
  return 0;
}

void SysTick_Handler(void) {
    ticks++;
}

void USART1_IRQHandler(void) {
  if (USART1->SR & USART_SR_TC) { // Transmisión completa
    __USART1_LoadNextByte();
  }
  if (USART1->SR & USART_SR_RXNE) { // Recepción
    char byte = USART1->DR;
    uint16_t nextHead = (rxHead + 1) % USART_RX_BUF_SIZE;
    if (nextHead != rxTail) { // Si el búfer no está lleno, guardamos el byte
      rxRingBuffer[rxHead] = byte;
      rxHead = nextHead;
    }
  }
}

void delay_ms(uint32_t ms) {
    uint32_t start = ticks;
    while ((ticks - start) < ms);
}

void USART1_SendString(const char *string, const uint16_t size) {
  uint16_t bytes_to_send = (size < USART_TX_BUF_SIZE) ? size : USART_TX_BUF_SIZE;

  // Esperar a que termine cualquier transmisión previa activa
  while (txIndex < txCount);

  // Deshabilitar interrupción temporalmente para proteger la sección crítica
  NVIC_DisableIRQ(USART1_IRQn);

  memcpy((void *)txBuffer, string, bytes_to_send);
  txCount = bytes_to_send;
  txIndex = 0;

  if (bytes_to_send > 0) {
    __USART1_LoadNextByte(); // Cargar primer byte
  }

  NVIC_EnableIRQ(USART1_IRQn);
}

void __USART1_LoadNextByte(void) {
  if (txIndex < txCount) {
    USART1->DR = txBuffer[txIndex];
    txIndex++;
  } else {
    txIndex = 0;
    txCount = 0;
    USART1->SR &= ~USART_SR_TC; // Limpiar bandera de transmisión completa
  }
}

uint16_t USART1_ReadString(char *buffer, uint16_t size, const char terminator) {
  uint16_t len = 0;
  uint16_t tempTail = rxTail;
  uint16_t foundTerminator = 0;

  // Deshabilitar interrupción temporalmente para proteger de actualizaciones de rxHead
  NVIC_DisableIRQ(USART1_IRQn);

  // Buscar si hay un delimitador de nueva línea (\n) en los datos recibidos
  while (tempTail != rxHead) {
    len++;
    if (rxRingBuffer[tempTail] == terminator) {
      foundTerminator = 1;
      break;
    }
    tempTail = (tempTail + 1) % USART_RX_BUF_SIZE;
  }

  // Si encontramos el terminador de línea, procedemos a leer la cadena
  if (foundTerminator) {
    uint16_t bytes_to_copy = (len < size) ? len : size;

    for (uint16_t i = 0; i < bytes_to_copy; i++) {
      buffer[i] = rxRingBuffer[rxTail];
      rxTail = (rxTail + 1) % USART_RX_BUF_SIZE;
    }

    // Si el mensaje en el anillo supera el tamaño del búfer de lectura, descartar el sobrante
    if (len > size) {
      for (uint16_t i = 0; i < (len - size); i++) {
        rxTail = (rxTail + 1) % USART_RX_BUF_SIZE;
      }
    }

    NVIC_EnableIRQ(USART1_IRQn);
    return bytes_to_copy;
  }

  NVIC_EnableIRQ(USART1_IRQn);
  return 0; // No hay una línea completa todavía
}
