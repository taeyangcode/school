; Assignment #5

; Program Description: This assignment has you work with bit-shifting operations, as well as writing your first procedures in assembly language (aside from the main procedure).
; Author: Corey Mostero 2566652
; Creation Date: 03 November 2023
; Revisions: N/A
; Date: 10 November 2023

.386
.model flat, stdcall
.stack 4096
ExitProcess PROTO, dwExitCode: DWORD

INCLUDE Irvine32.inc

.data

.code

main PROC
    INVOKE ExitProcess, 0
main ENDP

part_01 PROC
    mov bx, ax				; get multiplicand
    shl bx, 4				; multiply by factor 16
    push bx				; save factor 16

    mov bx, ax				; get multiplicand
    shl bx, 3				; multiply by factor 8

    shl ax, 1				; multiply by factor 2
    add ax, bx				; add factor 8
    pop bx				; recover factor 16
    add ax, bx				; add factor 16
part_01 ENDP
