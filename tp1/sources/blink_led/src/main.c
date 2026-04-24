#include <stdint.h>

typedef volatile uint32_t register_t;

#define GPIOC_BASE_ADDRESS 0x40011000
#define GPIOC_CRL   (*(register_t *)(GPIOC_BASE_ADDRESS))          // GPIOC configuration register L (0-7)
#define GPIOC_CRH   (*(register_t *)(GPIOC_BASE_ADDRESS + 0x04U))  // GPIOC configuration register H (8-15)
#define GPIOC_ODR   (*(register_t *)(GPIOC_BASE_ADDRESS + 0x0CU))  // GPIOC Output data register
#define GPIOC_BSRR  (*(register_t *)(GPIOC_BASE_ADDRESS + 0x10U))  // GPIOC Set/Reset register (atomic)
#define GPIOC_BRR   (*(register_t *)(GPIOC_BASE_ADDRESS + 0x14U))  // GPIOC Reset register (atomic)

#define RCC_BASE_ADDRESS 0x40021000
#define RCC_APB2ENR (*(register_t *)(RCC_BASE_ADDRESS + 0x18U))    // Advanced Peripheral Bus 2 clock enable register

int main() {
  RCC_APB2ENR = 1 << 4; // Como el valor despues de reset es 0x0, no deberia haber problema en hacer una asignacion
  GPIOC_CRH &= (~((register_t) 0xF << 20)); // Mascara para escribir los bits de config del GPIOC_13
  GPIOC_CRH |= 0x3 << 20; // Mode: 0b11: output mode 50MHz. Config: 0b00: GPO P-P

  while (1) {
    GPIOC_BSRR = 1 << 13;  // +0 es el offset para la parte de SET del registro
    for (int i = 0; i < 500000; i++); // delay arbitrario
    GPIOC_BSRR = 1 << (13 + 16); // +16 es el offset para la parte de RESET del registro
    for (int i = 0; i < 500000; i++); // delay arbitrario
  }

  return 0;
}
