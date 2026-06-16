#include "stm32f103xb.h"
#include <stdint.h>

typedef volatile uint32_t register_t;

// Variable global para indicar el modo de canal
// 0: Sensor de temperatura interno (Canal 16)
// 1: Potenciómetro en PA0 (Canal 0)
volatile uint8_t channel_mode = 0;
volatile uint32_t ticks = 0;

void SysTick_Handler(void) {
    ticks++;
}

void delay_ms(uint32_t ms) {
    uint32_t start = ticks;
    while ((ticks - start) < ms);
}

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
  GPIOA->BSRR = mask;
}

int main() {
  // Enciende clocks 
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_ADC1EN;
  
  // Configura prescaler del ADC a PCLK2 / 6 (ADCPRE bits 14-15 = 0b10)
  // Requerido para mantener la frecuencia del ADC por debajo de 14 MHz
  RCC->CFGR &= RCC_CFGR_ADCPRE;
  RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;

  // PC13 como salida push-pull
  GPIOC->CRH &= (~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13));
  GPIOC->CRH |=  GPIO_CRH_MODE13_0 | GPIO_CRH_MODE13_1;

  // PC15 como entrada pull-up
  GPIOC->CRH &= (~(GPIO_CRH_MODE15 | GPIO_CRH_CNF15));
  GPIOC->CRH |= GPIO_CRH_CNF15_1;
  GPIOC->ODR |= GPIO_ODR_ODR15; // Equivalente a prender el bit. Pull-Up

  // PA0 como entrada analógica y PA1-PA5 como salidas
  GPIOA->CRL &= (~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0 | \
                   GPIO_CRL_MODE1 | GPIO_CRL_CNF1 | \
                   GPIO_CRL_MODE2 | GPIO_CRL_CNF2 | \
                   GPIO_CRL_MODE3 | GPIO_CRL_CNF3 | \
                   GPIO_CRL_MODE4 | GPIO_CRL_CNF4 | \
                   GPIO_CRL_MODE5 | GPIO_CRL_CNF5));
  GPIOA->CRL |= GPIO_CRL_MODE1_0 | GPIO_CRL_MODE1_1 | \
                GPIO_CRL_MODE2_0 | GPIO_CRL_MODE2_1 | \
                GPIO_CRL_MODE3_0 | GPIO_CRL_MODE3_1 | \
                GPIO_CRL_MODE4_0 | GPIO_CRL_MODE4_1 | \
                GPIO_CRL_MODE5_0 | GPIO_CRL_MODE5_1;

  // Tiempo de muestreo de 239.5 ciclos para ambos canales
  ADC1->SMPR1 |= (0x7 << ADC_SMPR1_SMP16_Pos);
  ADC1->SMPR2 |= (0x7 << ADC_SMPR2_SMP0_Pos);

  ADC1->CR2 |= ADC_CR2_TSVREFE | ADC_CR2_EXTTRIG | (7 << ADC_CR2_EXTSEL_Pos);
  ADC1->CR2 |= ADC_CR2_ADON;      // Enciende el periférico ADC (ADON)

  // Calibración del ADC
  for (volatile int i = 0; i < 1000; i++); // Espera para estabilizar el encendido del ADC
  ADC1->CR2 |= ADC_CR2_CAL;      // Inicia calibración
  while (ADC1->CR2 & ADC_CR2_CAL); // Espera fin de calibración

  // 4. Configuración de interrupción externa
  AFIO->EXTICR[3] |= AFIO_EXTICR4_EXTI15_PC; // Mapea línea EXTI15 a puerto C
  EXTI->FTSR |= EXTI_FTSR_TR15;    // Disparo por flanco de bajada
  EXTI->IMR |= EXTI_IMR_MR15;     // Desenmascara interrupción
  NVIC_EnableIRQ(40);    // Habilita interrupción

  // Configuración del SysTick para interrupciones cada 1ms
  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / 1000);

  uint32_t toggle_counter = 0;

  while (1) {
    // Determina el canal activo según la selección del botón
    uint32_t channel = (channel_mode == 0) ? 16 : 0;
    ADC1->SQR3 = channel;

    // Inicia conversión por software
    ADC1->CR2 |= ADC_CR2_SWSTART;

    // Espera fin de conversión
    while (!(ADC1->SR & ADC_SR_EOS));

    // Lee el resultado
    uint32_t adc_val = ADC1->DR & ADC_DR_DATA_Msk;

    if (channel_mode == 0) {
      // Cálculo de temperatura en décimas de grado Celsius
      // T = ((V25 - Vsense) / Avg_Slope) + 25
      int32_t vsense = (adc_val * 3300) / 4095;
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
      GPIOC->ODR ^= GPIO_ODR_ODR13;
      toggle_counter = 0;
    }

    // Pequeño retardo entre lecturas
    delay_ms(10);
  }

  return 0;
}

void EXTI15_10_IRQHandler(void) {
  if (EXTI->IMR & EXTI_IMR_IM15) {
    // Alterna el modo de canal (0: sensor interno, 1: potenciómetro)
    channel_mode = !channel_mode;

    // Limpia bandera pendiente de interrupción en el EXTI
    EXTI->PR = EXTI_PR_PR15;
  }
}
