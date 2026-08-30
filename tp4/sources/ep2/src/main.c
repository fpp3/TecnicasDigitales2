#include <stdio.h>
#include "stm32f103xb.h"
#include "stm32f1xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

QueueHandle_t xADC1Queue;
SemaphoreHandle_t xADC1Semaphore;

void vUSARTInit(void);
void vADC1Init(void);
void vBlink(void *pvParameters);
void vSendTelemetry(void *pvParameters);
void vReadADC(void *pvParameters);

int main(void) {

    SystemCoreClockUpdate();

    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    GPIOC->CRH &= ~GPIO_CRH_CNF13;
    GPIOC->CRH |= GPIO_CRH_MODE13_0;

    vUSARTInit();
    vADC1Init();

    BaseType_t res;

    res = xTaskCreate(vBlink,     //funcion de la tarea
                      "Blink",    //nombre
                      256,        //tamaño dl stack en palabras
                      NULL,       //parametro pasado a la tarea
                      3,          //prioridad
                      NULL);      //manejador de tarea
    if (res != pdPASS) {
        while(1); // si falla creación de tarea, detener
    }

    res = xTaskCreate(vSendTelemetry,     //funcion de la tarea
                      "Telemetry",    //nombre
                      256,        //tamaño dl stack en palabras
                      NULL,       //parametro pasado a la tarea
                      1,          //prioridad
                      NULL);      //manejador de tarea
    if (res != pdPASS) {
        while(1); // si falla creación de tarea, detener
    }

    res = xTaskCreate(vReadADC,     //funcion de la tarea
                      "ReadADC",    //nombre
                      256,        //tamaño dl stack en palabras
                      NULL,       //parametro pasado a la tarea
                      2,          //prioridad
                      NULL);      //manejador de tarea
    if (res != pdPASS) {
        while(1); // si falla creación de tarea, detener
    }

    xADC1Queue = xQueueCreate(1, sizeof(uint32_t));
    xADC1Semaphore = xSemaphoreCreateBinary();

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

void vADC1Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN | RCC_APB2ENR_IOPAEN;
    // PA0 como entrada analógica
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
    GPIOA->CRL |= GPIO_CRL_MODE1_0 | GPIO_CRL_MODE1_1;

    // Tiempo de muestreo de 239.5 ciclos
    ADC1->SMPR2 |= (0x7 << ADC_SMPR2_SMP0_Pos);

    ADC1->CR1 |= ADC_CR1_EOCIE;
    ADC1->CR2 |= ADC_CR2_EXTTRIG | (7 << ADC_CR2_EXTSEL_Pos);
    ADC1->CR2 |= ADC_CR2_ADON;      // Enciende el periférico ADC (ADON)

    // Calibración del ADC
    for (volatile int i = 0; i < 1000; i++); // Espera para estabilizar el encendido del ADC
    ADC1->CR2 |= ADC_CR2_CAL;      // Inicia calibración
    while (ADC1->CR2 & ADC_CR2_CAL); // Espera fin de calibración
    NVIC_EnableIRQ(ADC1_IRQn);
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
    char *string = "Lectura ADC: ";
    while (1) {
        char buffer[50];
        uint32_t ulADC1Value;
        xQueueReceive(xADC1Queue, &ulADC1Value, 0);
        uint32_t length = sprintf(buffer, "%s%lu\r\n", string, ulADC1Value);
        for (uint16_t i = 0; i < length; i++) {
            while ((USART1->SR & USART_SR_TXE) == 0);
            USART1->DR = buffer[i];
        }
        vTaskDelayUntil(&xLastTickEntry, pdMS_TO_TICKS(2000));
    }
}

void vReadADC(void *pvParameters) {
    TickType_t xLastTickEntry = xTaskGetTickCount();
    ADC1->SQR3 = 0;
    while (1) {
        ADC1->CR2 |= ADC_CR2_SWSTART;
        xSemaphoreTake(xADC1Semaphore, 0); // espera infinitamente por datos nuevos
        vTaskDelayUntil(&xLastTickEntry, pdMS_TO_TICKS(100));
    }
}

void ADC1_2_IRQHandler(void) {
    BaseType_t xHigherPriorityTaskWoken;
    uint32_t ulADC1Value = ADC1->DR & ADC_DR_DATA_Msk;
    xQueueSendFromISR(xADC1Queue, &ulADC1Value, &xHigherPriorityTaskWoken);
    xSemaphoreGiveFromISR(xADC1Semaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

