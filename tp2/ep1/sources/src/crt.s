.syntax unified 
.cpu cortex-m3
.thumb
.extern _esstack

.section .text.manejadores 

// Función por defecto si ocurre una interrupción no implementada
.thumb_func 
.weak Default_Handler
Default_Handler:
    b .             

// Definición de vectores como débiles (weak)
.weak NMI_Handler
.thumb_set NMI_Handler, Default_Handler
.weak HardFault_Handler
.thumb_set HardFault_Handler, Default_Handler
.weak EXTI15_10_IRQHandler
.thumb_set EXTI15_10_IRQHandler, Default_Handler

// Tabla de vectores de interrupción
.section .isr_vector,"a",%progbits
.word _esstack               // Posición 0: Puntero de pila
.word _reset                 // Posición 1: Vector de Reset
.word NMI_Handler            // Posición 2: NMI
.word HardFault_Handler      // Posición 3: Falla de hardware

// Rellena las posiciones de excepciones del núcleo restantes (de la 4 a la 15)
.rept 12
.word 0
.endr

// Rellena las primeras interrupciones de periféricos del STM32 (de la 16 a la 55)
// Esto cubre los vectores desde WWDG hasta la interrupción número 39
.rept 40
.word 0
.endr
.word EXTI15_10_IRQHandler // Posición 56, dirección física exacta para la línea EXTI15_10

// Sección de código de Reset
.section .text.reset
.thumb_func
.global _reset
_reset:
    bl main
    b .

