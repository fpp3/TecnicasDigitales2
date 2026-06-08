#include <stdint.h>

typedef volatile uint32_t register_t;

#define GPIOA_BASE_ADDRESS 0x40010800
#define GPIOA_CRL   (*(register_t *)(GPIOA_BASE_ADDRESS))         // GPIOA config register L (0-7)
#define GPIOA_CRH   (*(register_t *)(GPIOA_BASE_ADDRESS + 0x04U)) // GPIOA config register H (8-15)
#define GPIOA_ODR   (*(register_t *)(GPIOA_BASE_ADDRESS + 0x0CU)) // GPIOA Output data register
#define GPIOA_BSRR  (*(register_t *)(GPIOA_BASE_ADDRESS + 0x10U)) // GPIOA Set/Reset register (atomic)
#define GPIOA_BRR   (*(register_t *)(GPIOA_BASE_ADDRESS + 0x14U)) // GPIOA Reset register (atomic)

#define GPIOC_BASE_ADDRESS 0x40011000
#define GPIOC_CRL   (*(register_t *)(GPIOC_BASE_ADDRESS))         // GPIOC config register L (0-7)
#define GPIOC_CRH   (*(register_t *)(GPIOC_BASE_ADDRESS + 0x04U)) // GPIOC config register H (8-15)
#define GPIOC_ODR   (*(register_t *)(GPIOC_BASE_ADDRESS + 0x0CU)) // GPIOC Output data register
#define GPIOC_BSRR  (*(register_t *)(GPIOC_BASE_ADDRESS + 0x10U)) // GPIOC Set/Reset register (atomic)
#define GPIOC_BRR   (*(register_t *)(GPIOC_BASE_ADDRESS + 0x14U)) // GPIOC Reset register (atomic)

#define RCC_BASE_ADDRESS 0x40021000
#define RCC_APB2ENR (*(register_t *)(RCC_BASE_ADDRESS + 0x18U))   // Advanced Peripheral Bus 2 clock enable register
#define RCC_CFGR    (*(register_t *)(RCC_BASE_ADDRESS + 0x04U))   // Clock configuration register

// Periférico AFIO (Alternate Function I/O)
#define AFIO_BASE_ADDRESS 0x40010000 
#define AFIO_EXTICR4     (*(register_t *)(AFIO_BASE_ADDRESS + 0x14U)) // External interrupt configuration register 4

// Periférico EXTI (External Interrupt Controller)
#define EXTI_BASE_ADDRESS 0x40010400
#define EXTI_IMR         (*(register_t *)(EXTI_BASE_ADDRESS + 0x00U)) // Interrupt Mask Register
#define EXTI_FTSR        (*(register_t *)(EXTI_BASE_ADDRESS + 0x0CU)) // Falling Trigger Selection Register
#define EXTI_PR          (*(register_t *)(EXTI_BASE_ADDRESS + 0x14U)) // Pending Register

// Periférico NVIC (Cortex-M3 Internal Core Peripheral)
#define NVIC_ISER1       (*(register_t *)(0xE000E104U)) // Interrupt Set-Enable Register 1

// Periférico ADC1
#define ADC1_BASE_ADDRESS 0x40012400
#define ADC_SR    (*(register_t *)(ADC1_BASE_ADDRESS + 0x00U)) // Status Register
#define ADC_CR1   (*(register_t *)(ADC1_BASE_ADDRESS + 0x04U)) // Control Register 1
#define ADC_CR2   (*(register_t *)(ADC1_BASE_ADDRESS + 0x08U)) // Control Register 2
#define ADC_SMPR1 (*(register_t *)(ADC1_BASE_ADDRESS + 0x0CU)) // Sample Time Register 1 (Ch 10-17)
#define ADC_SMPR2 (*(register_t *)(ADC1_BASE_ADDRESS + 0x10U)) // Sample Time Register 2 (Ch 0-9)
#define ADC_SQR3  (*(register_t *)(ADC1_BASE_ADDRESS + 0x34U)) // Regular Sequence Register 3
#define ADC_DT    (*(register_t *)(ADC1_BASE_ADDRESS + 0x4CU)) // Regular Data Register (ADC_DR)

// Variable global para indicar el modo de canal
// 0: Sensor de temperatura interno (Canal 16)
// 1: Potenciómetro en PA0 (Canal 0)
volatile uint8_t channel_mode = 0;

// función para encender un número específico de LED en PA1-PA5
void set_leds(uint8_t count) {
  uint32_t mask = 0;
  for (uint8_t i = 1; i <= 5; i++) {
    if (i <= count) {
      mask |= (1 << i);       // Enciende el LED en PAi
    } else {
      mask |= (1 << (i + 16)); // Apaga el LED en PAi
    }
  }
  GPIOA_BSRR = mask;
}

int main() {
  // 1. Habilitación del clock de los periféricos
  // Enciende clocks para GPIOA (bit 2), GPIOC (bit 4), AFIO (bit 0) y ADC1 (bit 9)
  RCC_APB2ENR |= (1 << 2) | (1 << 4) | (1 << 0) | (1 << 9);
  
  // Configura prescaler del ADC a PCLK2 / 6 (ADCPRE bits 14-15 = 0b10)
  // Requerido para mantener la frecuencia del ADC por debajo de 14 MHz
  RCC_CFGR &= ~(0x3 << 14);
  RCC_CFGR |= (0x2 << 14);

  // 2. Configuración de puertos GPIO
  // PC13 como salida push-pull (LED interno de parpadeo)
  GPIOC_CRH &= (~((register_t) 0xF << 20));
  GPIOC_CRH |= 0x3 << 20;

  // PC15 como entrada pull-up (pulsador de interrupción)
  GPIOC_CRH &= (~((register_t) 0xF << 28));
  GPIOC_CRH |= 0x8 << 28;
  GPIOC_ODR |= (1 << 15);

  // PA0 como entrada analógica (potenciómetro) y PA1-PA5 como salidas push-pull 50MHz
  GPIOA_CRL &= (~((register_t) 0xFFFFFF));
  GPIOA_CRL |= (0x33333 << 4) | (0x0 << 0);

  // 3. Configuración del ADC1
  ADC_SMPR1 |= (0x7 << 18); // Tiempo de muestreo máximo (239.5 ciclos) para canal 16 (temperatura)
  ADC_SMPR2 |= (0x7 << 0);  // Tiempo de muestreo máximo (239.5 ciclos) para canal 0 (potenciómetro)
  
  ADC_CR2 |= (1 << 23);     // Habilita el sensor de temperatura y referencia de voltaje interno (TSVREFE)
  ADC_CR2 |= (7 << 17) | (1 << 20); // Selecciona SWSTART como disparador y habilita el modo de trigger externo
  ADC_CR2 |= (1 << 0);      // Enciende el periférico ADC (ADON)

  // Calibración del ADC
  for (volatile int i = 0; i < 1000; i++); // Espera para estabilizar el encendido del ADC
  ADC_CR2 |= (1 << 2);      // Inicia calibración (CAL)
  while (ADC_CR2 & (1 << 2)); // Espera fin de calibración

  // 4. Configuración de interrupción externa (botón en PC15)
  AFIO_EXTICR4 |= 0x2 << 12; // Mapea línea EXTI15 a puerto C
  EXTI_FTSR |= 0x1 << 15;    // Disparo por flanco de bajada (al presionar botón)
  EXTI_IMR |= 0x1 << 15;     // Desenmascara interrupción de línea 15
  NVIC_ISER1 |= 0x1 << 8;    // Habilita interrupción EXTI15_10 en el NVIC (IRQ 40)

  uint32_t toggle_counter = 0;

  while (1) {
    // Determina el canal activo según la selección del botón
    uint32_t channel = (channel_mode == 0) ? 16 : 0;
    ADC_SQR3 = channel;

    // Inicia conversión por software
    ADC_CR2 |= (1 << 22); // SWSTART

    // Espera fin de conversión (EOC)
    while (!(ADC_SR & (1 << 1)));

    // Lee el resultado
    uint32_t adc_val = ADC_DT & 0xFFF;

    if (channel_mode == 0) {
      // Cálculo de temperatura en décimas de grado Celsius
      // T = ((V25 - Vsense) / Avg_Slope) + 25
      int32_t vsense = (adc_val * 3300) / 4095; // en mV
      int32_t temp = ((1430 - vsense) * 100) / 43 + 250; // temp en décimas de grado
      
      // Mapea la temperatura a los 5 LEDs
      if (temp < 260) set_leds(1);        // < 26 °C
      else if (temp < 300) set_leds(2);   // 26 - 30 °C
      else if (temp < 340) set_leds(3);   // 30 - 34 °C
      else if (temp < 380) set_leds(4);   // 34 - 38 °C
      else set_leds(5);                   // >= 38 °C
    } else {
      // Mapea el valor del potenciómetro (0 - 4095) a los 5 LEDs
      if (adc_val < 800) set_leds(1);
      else if (adc_val < 1600) set_leds(2);
      else if (adc_val < 2400) set_leds(3);
      else if (adc_val < 3200) set_leds(4);
      else set_leds(5);
    }

    // Parpadeo periódico del LED interno en PC13
    toggle_counter++;
    if (toggle_counter >= 50) {
      GPIOC_ODR ^= (1 << 13);
      toggle_counter = 0;
    }

    // Pequeño retardo entre lecturas
    for (volatile int i = 0; i < 10000; i++);
  }

  return 0;
}

void EXTI15_10_IRQHandler(void) {
  if (EXTI_IMR & (1 << 15)) {
    // Alterna el modo de canal (0: sensor interno, 1: potenciómetro)
    channel_mode = !channel_mode;
    
    // Limpia bandera pendiente de interrupción en el EXTI
    EXTI_PR = (1 << 15);
  }
}
