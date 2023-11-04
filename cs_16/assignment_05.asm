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

DECIMAL_OFFSET=5

.data

decimal_one BYTE "100123456789765"
decimal_two BYTE "123456"
decimal_three BYTE "12345"

.code

main PROC
    call test_WriteScaled

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

WriteScaled PROC
    mov ebx, ecx
    sub ebx, DECIMAL_OFFSET

write_string:
    cmp WORD PTR [edx], 0
    jz end_loop

    cmp ebx, 0
    jnz write_char

    mov al, '.'
    call WriteChar

    dec ebx

    jmp write_string

write_char:
    mov al, BYTE PTR [edx]
    call WriteChar

    dec ebx
    add edx, TYPE BYTE

    loop write_string

end_loop:
    call CrlF
    ret
WriteScaled ENDP

test_WriteScaled PROC
    mov edx, OFFSET decimal_one
    mov ecx, LENGTHOF decimal_one
    mov ebx, OFFSET DECIMAL_OFFSET

    call WriteScaled

    mov edx, OFFSET decimal_two
    mov ecx, LENGTHOF decimal_two
    mov ebx, OFFSET DECIMAL_OFFSET

    call WriteScaled

    mov edx, OFFSET decimal_three
    mov ecx, LENGTHOF decimal_three
    mov ebx, OFFSET DECIMAL_OFFSET

    call WriteScaled
test_WriteScaled ENDP

END main
