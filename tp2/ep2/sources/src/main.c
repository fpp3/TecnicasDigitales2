#include <stdint.h>

typedef volatile uint32_t register_t;

#define GPIOC_BASE_ADDRESS 0x40011000
#define GPIOC_CRL   (*(register_t *)(GPIOC_BASE_ADDRESS))         // GPIOC cfg register L (0-7)
#define GPIOC_CRH   (*(register_t *)(GPIOC_BASE_ADDRESS + 0x04U)) // GPIOC cfg register H (8-15)
#define GPIOC_ODR   (*(register_t *)(GPIOC_BASE_ADDRESS + 0x0CU)) // GPIOC Output data reg
#define GPIOC_BSRR  (*(register_t *)(GPIOC_BASE_ADDRESS + 0x10U)) // GPIOC Set/Reset reg (atomic)
#define GPIOC_BRR   (*(register_t *)(GPIOC_BASE_ADDRESS + 0x14U)) // GPIOC Reset reg (atomic)

#define RCC_BASE_ADDRESS 0x40021000
#define RCC_APB2ENR (*(register_t *)(RCC_BASE_ADDRESS + 0x18U))   // Advanced Peripheral Bus 2 clk en reg
#define RCC_CFGR      (*(register_t *)(RCC_BASE + 0x04)) // Configuración de Relojes (Prescalers)
                                                         // para ADC

// Periférico AFIO (Alternate Function I/O)
#define AFIO_BASE_ADDRESS 0x40010000 
// Registro de configuración de interrupciones externas 4 (maneja pines 12 a 15)
#define AFIO_EXTICR4     (*(register_t *)(AFIO_BASE_ADDRESS + 0x14U))

// Periférico EXTI (External Interrupt Controller)
#define EXTI_BASE_ADDRESS 0x40010400
#define EXTI_IMR         (*(register_t *)(EXTI_BASE_ADDRESS + 0x00U)) // Interrupt Mask Register
#define EXTI_FTSR        (*(register_t *)(EXTI_BASE_ADDRESS + 0x0CU)) // Falling Trigger Selection Register
#define EXTI_PR          (*(register_t *)(EXTI_BASE_ADDRESS + 0x14U)) // Pending Register

// Periférico NVIC (Cortex-M3 Internal Core Peripheral)
// El NVIC no está en el bus común de periféricos, está dentro del propio núcleo ARM
#define NVIC_ISER1       (*(register_t *)(0xE000E104U)) // Interrupt Set-Enable Register 1

#define ADC1_BASE_ADDRESS 0x40012400
#define ADC_SR (*(register_t *)(ADC1_BASE_ADDRESS + 0x00)) // Status Register
#define ADC_CR1 (*(register_t *)(ADC1_BASE_ADDRESS + 0x04)) // control Register 1
#define ADC_CR2 (*(register_t *)(ADC1_BASE_ADDRESS + 0x08)) // Control Register 2
#define ADC_SMPR2 (*(register_t *)(ADC1_BASE_ADDRESS + 0x10)) // Sample Time Register 2
#define ADC_SQR3 (*(register_t *)(ADC1_BASE_ADDRESS + 0x34)) // Regular Sequence Register 3
#define ADC_DT (*(register_t *)(ADC1_BASE_ADDRESS + 0x4C)) // Data Register

int main() {
  // encendido de los clocks
  RCC_APB2ENR |= (1 << 4) | (1 << 0) | (1 << 9); // encience clock de la GPIO,
                                                 // de la AFIO y del ADC1
  
  // configuración de la GPIOC 13 como salida (led interno)
  GPIOC_CRH &= (~((register_t) 0xF << 20)); // Mascara para escribir los bits de config del GPIOC_13
  GPIOC_CRH |= 0x3 << 20; // Mode: 0b11: output mode 50MHz. Config: 0b00: GPO P-P

  // configuración de la GPIOC 15 como entrada pull up para botón de interrupción
  GPIOC_CRH &= (~((register_t) 0xF << 28)); // pone en cero todo registro del puerto GPIOC_15
  GPIOC_CRH |= 0x8 << 28; //Mode: input pullup/pulldown
  GPIOC_ODR |= (1 << 15); //define GPIO_C15 explícitamente la entrada como pullup

  // configuración de la GPIOC 14 como entrada analógica
  GPIOC_CRH &= (~((register_t) 0xF << 24)); // pone en cero todo registro del pierto GPIOC_14
  GPIOC_CRH |= 0x0 << 24; //Mode: input Analog 

  // configuración del ADC
  ADC_CR2 |= 0x1 << 0; // enciende ADC
  ADC_CR2 |= 0x1 << 23; // habilita sensor de temperatura interno 
  ADC_CR2 |= 0x1 << 2; //calibración

  // configuración de la interrupción
  AFIO_EXTICR4 |= 0x2 << 12; // indica que puerto C pin 15 será interrupción
  EXTI_FTSR |= 0x1 << 15; // establece que el flanco de bajada activará la IQR
  EXTI_IMR |= 0x1 << 15; // desenmascara la interrupción en el pin correspondiente
  NVIC_ISER1 |= 0x1 << 8; // habilita interrupción en el NVIC

  /*
  while (1) {
    GPIOC_BSRR = 1 << 13;  // +0 es el offset para la parte de SET del registro
    for (int i = 0; i < 500000; i++); // delay arbitrario
    GPIOC_BSRR = 1 << (13 + 16); // +16 es el offset para la parte de RESET del registro
    for (int i = 0; i < 500000; i++); // delay arbitrario
  }
  */

  while(1);

  return 0;
}

void EXTI15_10_IRQHandler(void) {
    // 1. Verificar si la interrupción fue realmente causada por el pin 15
    if (EXTI_IMR & (1 << 15)) { // Opcional, pero buena práctica si compartes pines
        GPIOC_ODR ^= 1 << 13; // toggle led
        /* 
        3. LIMPIAR LA BANDERA (FLAG) DE INTERRUPCIÓN
        En el STM32, el registro EXTI_PR se limpia escribiendo un '1' lógico en el bit.
        Si no haces esto, al salir de la función el micro volverá a entrar
        inmediatamente */
        EXTI_PR = (1 << 15);
    }

