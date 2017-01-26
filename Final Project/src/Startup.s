; <<< Use Configuration Wizard in Context Menu >>>
;******************************************************************************
;
; startup_rvmdk.S - Startup code for use with Keil's uVision.
;
; Copyright (c) 2005-2011 Texas Instruments Incorporated.  All rights reserved.
; Software License Agreement
; 
; Texas Instruments (TI) is supplying this software for use solely and
; exclusively on TI's microcontroller products. The software is owned by
; TI and/or its suppliers, and is protected under applicable copyright
; laws. You may not combine this software with "viral" open-source
; software in order to form a larger program.
; 
; THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
; NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
; NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
; A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
; CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
; DAMAGES, FOR ANY REASON WHATSOEVER.
; 
; This is part of revision 8264 of the EK-LM3S1968 Firmware Package.
;
;******************************************************************************

;******************************************************************************
;
; <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
;
;******************************************************************************
Stack   EQU     0x00001000

;******************************************************************************
;
; <o> Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
;
;******************************************************************************
Heap    EQU     0x00001000

;******************************************************************************
;
; Allocate space for the stack.
;
;******************************************************************************
        AREA    STACK, NOINIT, READWRITE, ALIGN=3
StackMem
        SPACE   Stack
__initial_sp

;******************************************************************************
;
; Allocate space for the heap.
;
;******************************************************************************
        AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
HeapMem
        SPACE   Heap
__heap_limit

;******************************************************************************
;
; Indicate that the code in this file preserves 8-byte alignment of the stack.
;
;******************************************************************************
        PRESERVE8

;******************************************************************************
;
; Place code into the reset code section.
;
;******************************************************************************
        AREA    RESET, CODE, READONLY
        THUMB

;******************************************************************************
;
; External declaration for the interrupt handler used by the application.
;
;******************************************************************************
		EXTERN SYSTICKIntHandler
		EXTERN TimerAIntHandler
		EXTERN UART0IntHandler
		EXTERN UART2IntHandler
		EXTERN UART5IntHandler

;******************************************************************************
;
; The vector table.
;
;******************************************************************************
        EXPORT  __Vectors
__Vectors       DCD     __initial_sp              		; Top of Stack
                DCD     Reset_Handler            		; Reset Handler
                DCD     NmiSR               			; NMI Handler
                DCD     FaultISR         				; Hard Fault Handler
                DCD     IntDefaultHandler         		; MPU Fault Handler
                DCD     IntDefaultHandler          		; Bus Fault Handler
                DCD     IntDefaultHandler        		; Usage Fault Handler
                DCD     0                         		; Reserved
                DCD     0                         		; Reserved
                DCD     0                         		; Reserved
                DCD     0                         		; Reserved
                DCD     IntDefaultHandler               ; SVCall Handler
                DCD     IntDefaultHandler          		; Debug Monitor Handler
                DCD     0                         		; Reserved
                DCD     IntDefaultHandler            	; PendSV Handler
                DCD     SYSTICKIntHandler           	; SysTick Handler

                ; External Interrupts

                DCD     IntDefaultHandler             	;   0: GPIO Port A
                DCD     IntDefaultHandler             	;   1: GPIO Port B
                DCD     IntDefaultHandler             	;   2: GPIO Port C
                DCD     IntDefaultHandler             	;   3: GPIO Port D
                DCD     IntDefaultHandler             	;   4: GPIO Port E
                DCD     UART0IntHandler             	;   5: UART0 Rx and Tx
                DCD     IntDefaultHandler             	;   6: UART1 Rx and Tx
                DCD     IntDefaultHandler              	;   7: SSI0 Rx and Tx
                DCD     IntDefaultHandler              	;   8: I2C0 Master and Slave
                DCD     IntDefaultHandler        		;   9: PWM Fault
                DCD     IntDefaultHandler            	;  10: PWM Generator 0
                DCD     IntDefaultHandler            	;  11: PWM Generator 1
                DCD     IntDefaultHandler            	;  12: PWM Generator 2
                DCD     IntDefaultHandler              	;  13: Quadrature Encoder 0
                DCD     IntDefaultHandler           	;  14: ADC Sequence 0
                DCD     IntDefaultHandler           	;  15: ADC Sequence 1
                DCD     IntDefaultHandler           	;  16: ADC Sequence 2
                DCD     IntDefaultHandler           	;  17: ADC Sequence 3
                DCD     IntDefaultHandler		        ;  18: Watchdog timer
                DCD     TimerAIntHandler           		;  19: Timer 0 subtimer A
                DCD     IntDefaultHandler           	;  20: Timer 0 subtimer B
                DCD     IntDefaultHandler           	;  21: Timer 1 subtimer A
                DCD     IntDefaultHandler           	;  22: Timer 1 subtimer B
                DCD     IntDefaultHandler           	;  23: Timer 2 subtimer A
                DCD     IntDefaultHandler           	;  24: Timer 2 subtimer B
                DCD     IntDefaultHandler             	;  25: Analog Comparator 0
                DCD     IntDefaultHandler             	;  26: Analog Comparator 1
                DCD     IntDefaultHandler             	;  27: Analog Comparator 2
                DCD     IntDefaultHandler            	;  28: System Control (PLL, OSC, BO)
                DCD     IntDefaultHandler             	;  29: FLASH Control
                DCD     IntDefaultHandler             	;  30: GPIO Port F
                DCD     IntDefaultHandler             	;  31: GPIO Port G
                DCD     IntDefaultHandler             	;  32: GPIO Port H
                DCD     UART2IntHandler             	;  33: UART2 Rx and Tx
                DCD     IntDefaultHandler              	;  34: SSI1 Rx and Tx
                DCD     IntDefaultHandler           	;  35: Timer 3 subtimer A
                DCD     IntDefaultHandler           	;  36: Timer 3 subtimer B
                DCD     IntDefaultHandler              	;  37: I2C1 Master and Slave
                DCD     IntDefaultHandler              	;  38: Quadrature Encoder 1
                DCD     IntDefaultHandler              	;  39: CAN0
                DCD     IntDefaultHandler              	;  40: CAN1
                DCD     IntDefaultHandler              	;  41: CAN2
                DCD     0                         		;  42: Reserved
                DCD     IntDefaultHandler               ;  43: Hibernate
                DCD     IntDefaultHandler              	;  44: USB0
                DCD     IntDefaultHandler            	;  45: PWM Generator 3
                DCD     IntDefaultHandler              	;  46: uDMA Software Transfer
                DCD     IntDefaultHandler           	;  47: uDMA Error
                DCD     IntDefaultHandler           	;  48: ADC1 Sequence 0
                DCD     IntDefaultHandler           	;  49: ADC1 Sequence 1
                DCD     IntDefaultHandler           	;  50: ADC1 Sequence 2
                DCD     IntDefaultHandler           	;  51: ADC1 Sequence 3
                DCD     0                         		;  52: Reserved
                DCD     0                         		;  53: Reserved
                DCD     IntDefaultHandler             	;  54: GPIO Port J
                DCD     IntDefaultHandler             	;  55: GPIO Port K
                DCD     IntDefaultHandler             	;  56: GPIO Port L
                DCD     IntDefaultHandler              	;  57: SSI2 Rx and Tx
                DCD     IntDefaultHandler              	;  58: SSI3 Rx and Tx
                DCD     IntDefaultHandler             	;  59: UART3 Rx and Tx
                DCD     IntDefaultHandler             	;  60: UART4 Rx and Tx
                DCD     UART5IntHandler             	;  61: UART5 Rx and Tx
                DCD     IntDefaultHandler             	;  62: UART6 Rx and Tx
                DCD     IntDefaultHandler             	;  63: UART7 Rx and Tx
                DCD     0                         		;  64: Reserved
                DCD     0                         		;  65: Reserved
                DCD     0                         		;  66: Reserved
                DCD     0                         		;  67: Reserved
                DCD     IntDefaultHandler              	;  68: I2C2 Master and Slave
                DCD     IntDefaultHandler              	;  69: I2C3 Master and Slave
                DCD     IntDefaultHandler           	;  70: Timer 4 subtimer A
                DCD     IntDefaultHandler           	;  71: Timer 4 subtimer B
                DCD     0                         		;  72: Reserved
                DCD     0                         		;  73: Reserved
                DCD     0                        		;  74: Reserved
                DCD     0                         		;  75: Reserved
                DCD     0                         		;  76: Reserved
                DCD     0                         		;  77: Reserved
                DCD     0                         		;  78: Reserved
                DCD     0                         		;  79: Reserved
                DCD     0                         		;  80: Reserved
                DCD     0                         		;  81: Reserved
                DCD     0                         		;  82: Reserved
                DCD     0                         		;  83: Reserved
                DCD     0                         		;  84: Reserved
                DCD     0                         		;  85: Reserved
                DCD     0                         		;  86: Reserved
                DCD     0                         		;  87: Reserved
                DCD     0                         		;  88: Reserved
                DCD     0                         		;  89: Reserved
                DCD     0                         		;  90: Reserved
                DCD     0                         		;  91: Reserved
                DCD     IntDefaultHandler           	;  92: Timer 5 subtimer A
                DCD     IntDefaultHandler           	;  93: Timer 5 subtimer B
                DCD     IntDefaultHandler          		;  94: Wide Timer 0 subtimer A
                DCD     IntDefaultHandler          		;  95: Wide Timer 0 subtimer B
                DCD     IntDefaultHandler          		;  96: Wide Timer 1 subtimer A
                DCD     IntDefaultHandler          		;  97: Wide Timer 1 subtimer B
                DCD     IntDefaultHandler         		;  98: Wide Timer 2 subtimer A
                DCD     IntDefaultHandler          		;  99: Wide Timer 2 subtimer B
                DCD     IntDefaultHandler          		; 100: Wide Timer 3 subtimer A
                DCD     IntDefaultHandler          		; 101: Wide Timer 3 subtimer B
                DCD     IntDefaultHandler          		; 102: Wide Timer 4 subtimer A
                DCD     IntDefaultHandler          		; 103: Wide Timer 4 subtimer B
                DCD     IntDefaultHandler          		; 104: Wide Timer 5 subtimer A
                DCD     IntDefaultHandler          		; 105: Wide Timer 5 subtimer B
                DCD     IntDefaultHandler               ; 106: FPU
                DCD     0                         		; 107: Reserved
                DCD     0                        	 	; 108: Reserved
                DCD     IntDefaultHandler              	; 109: I2C4 Master and Slave
                DCD     IntDefaultHandler              	; 110: I2C5 Master and Slave
                DCD     IntDefaultHandler             	; 111: GPIO Port M
                DCD     IntDefaultHandler             	; 112: GPIO Port N
                DCD     IntDefaultHandler              	; 113: Quadrature Encoder 2
                DCD     0                         		; 114: Reserved
                DCD     0                         		; 115: Reserved
                DCD     IntDefaultHandler            	; 116: GPIO Port P (Summary or P0)
                DCD     IntDefaultHandler            	; 117: GPIO Port P1
                DCD     IntDefaultHandler            	; 118: GPIO Port P2
                DCD     IntDefaultHandler            	; 119: GPIO Port P3
                DCD     IntDefaultHandler            	; 120: GPIO Port P4
                DCD     IntDefaultHandler            	; 121: GPIO Port P5
                DCD     IntDefaultHandler            	; 122: GPIO Port P6
                DCD     IntDefaultHandler            	; 123: GPIO Port P7
                DCD     IntDefaultHandler            	; 124: GPIO Port Q (Summary or Q0)
                DCD     IntDefaultHandler            	; 125: GPIO Port Q1
                DCD     IntDefaultHandler            	; 126: GPIO Port Q2
                DCD     IntDefaultHandler            	; 127: GPIO Port Q3
                DCD     IntDefaultHandler            	; 128: GPIO Port Q4
                DCD     IntDefaultHandler            	; 129: GPIO Port Q5
                DCD     IntDefaultHandler            	; 130: GPIO Port Q6
                DCD     IntDefaultHandler            	; 131: GPIO Port Q7
                DCD     IntDefaultHandler             	; 132: GPIO Port R
                DCD     IntDefaultHandler             	; 133: GPIO Port S
                DCD     IntDefaultHandler            	; 134: PWM 1 Generator 0
                DCD     IntDefaultHandler            	; 135: PWM 1 Generator 1
                DCD     IntDefaultHandler            	; 136: PWM 1 Generator 2
                DCD     IntDefaultHandler            	; 137: PWM 1 Generator 3
                DCD     IntDefaultHandler        		; 138: PWM 1 Fault

__Vectors_End

;******************************************************************************
;
; This is the code that gets called when the processor first starts execution
; following a reset event.
;
;******************************************************************************
        EXPORT  Reset_Handler
Reset_Handler
        ;
        ; Call the C library enty point that handles startup.  This will copy
        ; the .data section initializers from flash to SRAM and zero fill the
        ; .bss section.
        ;
        IMPORT  __main
		B       __main

;******************************************************************************
;
; This is the code that gets called when the processor receives a NMI.  This
; simply enters an infinite loop, preserving the system state for examination
; by a debugger.
;
;******************************************************************************
NmiSR
        B       NmiSR

;******************************************************************************
;
; This is the code that gets called when the processor receives a fault
; interrupt.  This simply enters an infinite loop, preserving the system state
; for examination by a debugger.
;
;******************************************************************************
FaultISR
        B       FaultISR

;******************************************************************************
;
; This is the code that gets called when the processor receives an unexpected
; interrupt.  This simply enters an infinite loop, preserving the system state
; for examination by a debugger.
;
;******************************************************************************
IntDefaultHandler
        B       IntDefaultHandler

;******************************************************************************
;
; Make sure the end of this section is aligned.
;
;******************************************************************************
        ALIGN

;******************************************************************************
;
; Some code in the normal code section for initializing the heap and stack.
;
;******************************************************************************
        AREA    |.text|, CODE, READONLY


;******************************************************************************
;
; Useful functions.
;
;******************************************************************************
        EXPORT  DisableInterrupts
        EXPORT  EnableInterrupts
        EXPORT  StartCritical
        EXPORT  EndCritical
        EXPORT  WaitForInterrupt

;*********** DisableInterrupts ***************
; disable interrupts
; inputs:  none
; outputs: none
DisableInterrupts
        CPSID  I
        BX     LR

;*********** EnableInterrupts ***************
; disable interrupts
; inputs:  none
; outputs: none
EnableInterrupts
        CPSIE  I
        BX     LR

;*********** StartCritical ************************
; make a copy of previous I bit, disable interrupts
; inputs:  none
; outputs: previous I bit
StartCritical
        MRS    R0, PRIMASK  ; save old status
        CPSID  I            ; mask all (except faults)
        BX     LR

;*********** EndCritical ************************
; using the copy of previous I bit, restore I bit to previous value
; inputs:  previous I bit
; outputs: none
EndCritical
        MSR    PRIMASK, R0
        BX     LR

;*********** WaitForInterrupt ************************
; go to low power mode while waiting for the next interrupt
; inputs:  none
; outputs: none
WaitForInterrupt
        WFI
        BX     LR
		
    EXPORT    getHeapBase
    EXPORT    getStackBase
    EXPORT    getHeapSize

;******************************************************************************
; Returns the address of the heap base
;******************************************************************************
getHeapBase
    MOV32        R0, __heap_base
    BX        LR

;******************************************************************************
; Returns the address of the heap base
;******************************************************************************
getHeapSize
    MOV32        R0, __heap_base
    MOV32        R1, __heap_limit
    SUB            R0, R1, R0
    BX        LR
    
;******************************************************************************
; Returns the address of the heap base
;******************************************************************************
getStackBase
    MOV32        R0, StackMem
    BX        LR
    
;******************************************************************************
;
; The function expected of the C library startup code for defining the stack
; and heap memory locations.  For the C library version of the startup code,
; provide this function so that the C library initialization code can find out
; the location of the stack and heap.
;
;******************************************************************************
    IF :DEF: __MICROLIB
        EXPORT  __initial_sp
        EXPORT  __heap_base
        EXPORT  __heap_limit
    ELSE
        IMPORT  __use_two_region_memory
        EXPORT  __user_initial_stackheap
__user_initial_stackheap
        LDR     R0, =HeapMem
        LDR     R1, =(StackMem + Stack)
        LDR     R2, =(HeapMem + Heap)
        LDR     R3, =StackMem
        BX      LR
    ENDIF

;******************************************************************************
;
; Make sure the end of this section is aligned.
;
;******************************************************************************
        ALIGN

;******************************************************************************
;
; Tell the assembler that we're done.
;
;******************************************************************************
        END
