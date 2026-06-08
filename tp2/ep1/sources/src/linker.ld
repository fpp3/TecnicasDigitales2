MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 64K
  RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 20K
}

/* Punto de entrada al arrancar */
ENTRY(_reset) 

/* Dirección final de la RAM para inicializar la pila */
_esstack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
  /* Asegura la colocación de la tabla de vectores al inicio de la FLASH */
  .isr_vector :
  {
    KEEP(*(.isr_vector))  
  } > FLASH

  /* Código del programa y constantes */
  .text :
  {
    *(.text)             
    *(.text*)            
    *(.rodata)           
    *(.rodata*)
  } > FLASH

  /* Direcciones de control para variables globales (en caso de usarse en el futuro) */
  _sidata = LOADADDR(.data);

  .data :
  {
    _sdata = .;
    *(.data)
    *(.data*)
    _edata = .;
  } > RAM AT> FLASH     

  .bss :
  {
    _sbss = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > RAM               
}
