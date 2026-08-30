#include <stdio.h>
#include "stm32f1xx.h"
#include "FreeRTOS.h"
#include "task.h"

volatile uint32_t ulIdleCount = 0;

void vUSARTInit(void);
void vBlink(void *pvParameters);
void vSendTelemetry(void *pvParameters);

int main(void) {

    SystemCoreClockUpdate();

    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    GPIOC->CRH &= ~GPIO_CRH_CNF13;
    GPIOC->CRH |= GPIO_CRH_MODE13_0;

    vUSARTInit();

    BaseType_t res;

    // Stack: 256 palabras (1024 bytes), prioridad 1
    res = xTaskCreate(vBlink,     //funcion de la tarea
                      "Blink",    //nombre
                      256,        //tamaño dl stack en palabras
                      NULL,       //parametro pasado a la tarea
                      3,          //prioridad
                      NULL);      //manejador de tarea
    if (res != pdPASS) {
        while(1); // si falla creación de tarea, detener
    }

    // Stack: 256 palabras (1024 bytes), prioridad 1
    res = xTaskCreate(vSendTelemetry,     //funcion de la tarea
                      "Telemetry",    //nombre
                      256,        //tamaño dl stack en palabras
                      NULL,       //parametro pasado a la tarea
                      2,          //prioridad
                      NULL);      //manejador de tarea
    if (res != pdPASS) {
        while(1); // si falla creación de tarea, detener
    }

    vTaskStartScheduler();

    return 0;
}

void vUSARTInit(void) {
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN;
    GPIOA->CRH &= ~(GPIO_CRH_CNF9 | GPIO_CRH_MODE9 | GPIO_CRH_CNF10 | GPIO_CRH_MODE10);
    GPIOA->CRH |= GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1 | GPIO_CRH_CNF10_1; // GPIOA9 output alternate function push-pull, GPIOA10 input pull up/down
    GPIOA->ODR |= GPIO_ODR_ODR10; // GPIOA10 pull-up

    USART1->BRR = SystemCoreClock/115200;
    USART1->CR1 = /*USART_CR1_TCIE | USART_CR1_RXNEIE | USART_CR1_RE |*/ USART_CR1_TE;
    USART1->CR1 |= USART_CR1_UE; // Activa el periférico
}

void vBlink(void *pvParameters) {
    TickType_t xLastTickEntry = xTaskGetTickCount();
    while (1) {
        GPIOC->ODR ^= GPIO_ODR_ODR13;
        vTaskDelayUntil(&xLastTickEntry, pdMS_TO_TICKS(100));
    }
}

void vSendTelemetry(void *pvParameters) {
    TickType_t xLastTickEntry = xTaskGetTickCount();
    char *string = "Sistema Operativo Ejecutando. idle: ";
    while (1) {
        char buffer[50];
        uint32_t length = sprintf(buffer, "%s%lu\r\n", string, ulIdleCount);
        for (uint16_t i = 0; i < length; i++) {
            while ((USART1->SR & USART_SR_TXE) == 0);
            USART1->DR = buffer[i];
        }
        vTaskDelayUntil(&xLastTickEntry, pdMS_TO_TICKS(2000));
    }
}

void vApplicationIdleHook(void) {
    ulIdleCount++;
}

