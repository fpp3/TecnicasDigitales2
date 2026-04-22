#include <stdint.h>

#define GPIOC_BASE_ADDRESS 0x40011000
#define GPIOC_CRL   *(volatile uint32_t *)(GPIOC_BASE_ADDRESS)
#define GPIOC_CRH   *(volatile uint32_t *)(GPIOC_BASE_ADDRESS + 0x04U)
#define GPIOC_IDR   *(volatile uint32_t *)(GPIOC_BASE_ADDRESS + 0x08U)
#define GPIOC_ODR   *(volatile uint32_t *)(GPIOC_BASE_ADDRESS + 0x0CU)
#define GPIOC_BSRR  *(volatile uint32_t *)(GPIOC_BASE_ADDRESS + 0x10U)
#define GPIOC_BRR   *(volatile uint32_t *)(GPIOC_BASE_ADDRESS + 0x14U)

#define RCC_BASE_ADDRESS 0x40021000
#define RCC_CR      *(volatile uint32_t *)(RCC_BASE_ADDRESS)
#define RCC_CFGR    *(volatile uint32_t *)(RCC_BASE_ADDRESS + 0x04U)
#define RCC_CIR     *(volatile uint32_t *)(RCC_BASE_ADDRESS + 0x08U)
#define RCC_APB2RSTR *(volatile uint32_t *)(RCC_BASE_ADDRESS + 0x0CU)
#define RCC_APB1RSTR *(volatile uint32_t *)(RCC_BASE_ADDRESS + 0x10U)
#define RCC_AHBENR  *(volatile uint32_t *)(RCC_BASE_ADDRESS + 0x14U)
#define RCC_APB2ENR *(volatile uint32_t *)(RCC_BASE_ADDRESS + 0x18U)
#define RCC_APB1ENR *(volatile uint32_t *)(RCC_BASE_ADDRESS + 0x1CU)
#define RCC_BDCR    *(volatile uint32_t *)(RCC_BASE_ADDRESS + 0x20U)
#define RCC_CSR     *(volatile uint32_t *)(RCC_BASE_ADDRESS + 0x24U)
#define RCC_AHBRSTR *(volatile uint32_t *)(RCC_BASE_ADDRESS + 0x28U)
#define RCC_CFGR2   *(volatile uint32_t *)(RCC_BASE_ADDRESS + 0x2CU


int main() {

  return 0;
}
