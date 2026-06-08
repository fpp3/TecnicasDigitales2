#include <stdint.h>

typedef volatile uint32_t register_t;

// Periféricos GPIO
#define GPIOB_BASE_ADDRESS 0x40010C00
#define GPIOB_CRL   (*(register_t *)(GPIOB_BASE_ADDRESS))         // GPIOB cfg register L (0-7)
#define GPIOB_ODR   (*(register_t *)(GPIOB_BASE_ADDRESS + 0x0CU)) // GPIOB Output data reg

#define GPIOC_BASE_ADDRESS 0x40011000
#define GPIOC_CRH   (*(register_t *)(GPIOC_BASE_ADDRESS + 0x04U)) // GPIOC cfg register H (8-15)
#define GPIOC_ODR   (*(register_t *)(GPIOC_BASE_ADDRESS + 0x0CU)) // GPIOC Output data reg
#define GPIOC_BSRR  (*(register_t *)(GPIOC_BASE_ADDRESS + 0x10U)) // GPIOC Set/Reset reg (atomic)

#define RCC_BASE_ADDRESS 0x40021000
#define RCC_APB2ENR (*(register_t *)(RCC_BASE_ADDRESS + 0x18U))   // APB2 clk en reg

// Periférico AFIO (Alternate Function I/O)
#define AFIO_BASE_ADDRESS 0x40010000 
#define AFIO_EXTICR1     (*(register_t *)(AFIO_BASE_ADDRESS + 0x08U)) // Mapea EXTI0 a EXTI3
#define AFIO_EXTICR4     (*(register_t *)(AFIO_BASE_ADDRESS + 0x14U)) // Mapea EXTI12 a EXTI15

// Periférico EXTI (External Interrupt Controller)
#define EXTI_BASE_ADDRESS 0x40010400
#define EXTI_IMR         (*(register_t *)(EXTI_BASE_ADDRESS + 0x00U)) // Interrupt Mask Register
#define EXTI_FTSR        (*(register_t *)(EXTI_BASE_ADDRESS + 0x0CU)) // Falling Trigger Selection Register
#define EXTI_PR          (*(register_t *)(EXTI_BASE_ADDRESS + 0x14U)) // Pending Register

// Periférico NVIC
#define NVIC_ISER0       (*(register_t *)(0xE000E100U)) // Interrupt Set-Enable Register 0 (IRQ 0-31)
#define NVIC_ISER1       (*(register_t *)(0xE000E104U)) // Interrupt Set-Enable Register 1 (IRQ 32-63)
#define NVIC_IPR1        (*(register_t *)(0xE000E404U)) // Interrupt Priority Register 1 (IRQ 4-7)
#define NVIC_IPR10       (*(register_t *)(0xE000E428U)) // Interrupt Priority Register 10 (IRQ 40-43)

int main() {
  // 1. Habilitación de clocks
  // Enciende clock de GPIOC (bit 4), GPIOB (bit 3) y AFIO (bit 0)
  RCC_APB2ENR |= (1 << 4) | (1 << 3) | (1 << 0);
  
  // 2. Configuración de pines de entrada y salida
  GPIOC_CRH &= (~((register_t) 0xF << 20)); // PC13 (led interno)
  GPIOC_CRH |= 0x3 << 20; // Output push-pull 50MHz
  
  GPIOC_CRH &= (~((register_t) 0xF << 24)); // PC14 (led externo 1)
  GPIOC_CRH |= 0x3 << 24; // Output push-pull 50MHz

  GPIOC_CRH &= (~((register_t) 0xF << 28)); // PC15 (pulsador 1)
  GPIOC_CRH |= 0x8 << 28; // Input pull-up/pull-down
  GPIOC_ODR |= (1 << 15); // Define entrada como pull-up

  GPIOB_CRL &= (~((register_t) 0xFF << 0)); // PB0 (pulsador 2) y PB1 (led externo 2)
  GPIOB_CRL |= (0x3 << 4) | (0x8 << 0);  // PB1 output 50MHz, PB0 input pull-up/down
  GPIOB_ODR |= (1 << 0);                 // Define entrada como pull-up

  // 3. Configuración de interrupciones externas (EXTI)
  // Mapea línea EXTI15 a Puerto C
  AFIO_EXTICR4 |= 0x2 << 12;
  
  // Mapea línea EXTI0 a Puerto B
  AFIO_EXTICR1 &= ~(0xFU << 0);
  AFIO_EXTICR1 |= (0x1U << 0);

  // Configura flanco de bajada para EXTI15 y EXTI0
  EXTI_FTSR |= (1 << 15) | (1 << 0);
  
  // Desenmascara las líneas EXTI15 y EXTI0
  EXTI_IMR |= (1 << 15) | (1 << 0);

  // 4. Configuración de prioridades en el NVIC
  // EXTI15_10 (IRQ 40) -> Prioridad alta: 0x40 (bits 4-7 de NVIC_IPR10)
  NVIC_IPR10 &= ~(0xFFU << 0);
  NVIC_IPR10 |= (0x40U << 0);

  // EXTI0 (IRQ 6) -> Prioridad baja: 0x80 (bits 20-23 de NVIC_IPR1)
  NVIC_IPR1 &= ~(0xFFU << 16);
  NVIC_IPR1 |= (0x80U << 16);

  // Habilita las interrupciones en el NVIC
  NVIC_ISER1 |= (1 << 8); // Habilita IRQ 40 (EXTI15_10)
  NVIC_ISER0 |= (1 << 6); // Habilita IRQ 6 (EXTI0)

  while (1) {
    // Parpadeo normal del led interno en PC13
    GPIOC_BSRR = 1 << 13;
    for (volatile int i = 0; i < 500000; i++);
    GPIOC_BSRR = 1 << (13 + 16);
    for (volatile int i = 0; i < 500000; i++);
  }

  return 0;
}

// ISR para EXTI15_10 (Button A en PC15, controla LED 1 en PC14)
void EXTI15_10_IRQHandler(void) {
  if (EXTI_PR & (1 << 15)) {
    // Parpadea LED 1 (PC14) rápidamente 15 veces (30 conmutaciones)
    for (int j = 0; j < 30; j++) {
      GPIOC_ODR ^= (1 << 14);
      for (volatile int i = 0; i < 200000; i++); // Retardo observable
    }
    EXTI_PR = (1 << 15); // Limpia flag
  }
}

// ISR para EXTI0 (Button B en PB0, controla LED 2 en PB1)
void EXTI0_IRQHandler(void) {
  if (EXTI_PR & (1 << 0)) {
    // Parpadea LED 2 (PB1) rápidamente 15 veces (30 conmutaciones)
    for (int j = 0; j < 30; j++) {
      GPIOB_ODR ^= (1 << 1);
      for (volatile int i = 0; i < 200000; i++); // Retardo observable
    }
    EXTI_PR = (1 << 0); // Limpia flag
  }
}
