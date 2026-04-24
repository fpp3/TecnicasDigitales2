.syntax unified
.cpu cortex-m3
.thumb
.global _estack
.word _estack
.word _reset
.thumb_func
_reset:
    bl main
    b .
