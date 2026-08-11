#include "stm32f1xx.h"

volatile uint32_t ms_ticks = 0;

void _init(void) {}

void SysTick_Handler(void) {
    ms_ticks++;
}

void delay_ms(uint32_t ms) {
    uint32_t start = ms_ticks;
    while ((ms_ticks - start) < ms);
}

int main(void) {
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_TIM1EN;

    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
    GPIOC->CRH |= (2 << GPIO_CRH_MODE13_Pos);

    GPIOA->CRH &= ~(GPIO_CRH_MODE8 | GPIO_CRH_CNF8);
    GPIOA->CRH |= (3 << GPIO_CRH_MODE8_Pos) | (2 << GPIO_CRH_CNF8_Pos);

    TIM1->PSC = 7;
    TIM1->ARR = 999;
    TIM1->CCR1 = 0;

    TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM1->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE;

    TIM1->CCER &= ~TIM_CCER_CC1P;
    TIM1->CCER |= TIM_CCER_CC1E;

    TIM1->BDTR |= TIM_BDTR_MOE;

    TIM1->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN;
    TIM1->EGR |= TIM_EGR_UG;

    uint32_t last_blink = ms_ticks;
    uint32_t last_fade = ms_ticks;
    int16_t brightness = 0;
    int8_t step = 5;

    while (1) {
        uint32_t now = ms_ticks;

        if ((now - last_blink) >= 200) {
            last_blink = now;
            GPIOC->ODR ^= GPIO_ODR_ODR13;
        }

        if ((now - last_fade) >= 10) {
            last_fade = now;

            TIM1->CCR1 = brightness;
            brightness += step;

            if (brightness <= 0 || brightness >= 1000) {
                step = -step;
            }
        }
    }

    return 0;
}
