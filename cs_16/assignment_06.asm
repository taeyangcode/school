; Assignment #6

; Program Description: This assignment practices the use of macros, as well as strings and arrays.
; Author: Corey Mostero 2566652
; Creation Date: 06 November 2023
; Revisions: N/A
; Date: 17 November 2023

.386
.model flat, stdcall
.stack 4096
ExitProcess PROTO, dwExitCode: DWORD

INCLUDE Irvine32.inc

.data

targetString BYTE "ABCDE", 10 DUP(0)
sourceString BYTE "FGH", 0

.code

DifferentInputs PROTO,
	value_1: DWORD,
	value_2: DWORD,
	value_3: DWORD

StrConcat PROTO,
	source: PTR BYTE,
	target: PTR BYTE

main PROC
	INVOKE StrConcat, ADDR sourceString, ADDR targetString

    INVOKE ExitProcess, 0
main ENDP

part_01 PROC
	; rows = 10
	; columns = 5

	; .data

	; iVal = 10

	; REPEAT rows * columns
	; 	DWORD iVal
	; 	iVal = iVal + 10
	; ENDM
	
	; The last integer to be generated would be: 510
part_01 ENDP

DifferentInputs PROC USES ebx ecx edx, 
	value_1: DWORD,
	value_2: DWORD,
	value_3: DWORD

	mov ebx, value_1
	mov ecx, value_2
	mov edx, value_3

	cmp ebx, ecx
	jne NotEqual

	cmp ecx, edx
	jne NotEqual

	mov eax, 1
	ret

NotEqual:
	mov eax, 0
	ret
DifferentInputs ENDP

test_DifferenceInputs PROC
	INVOKE DifferentInputs, 0, 0, 0

	INVOKE DifferentInputs, 1, 0, 0

	INVOKE DifferentInputs, 0, 1, 0

	INVOKE DifferentInputs, 0, 0, 1

	INVOKE DifferentInputs, 1, 0, 1
test_DifferenceInputs ENDP

StrConcat PROC USES eax ebx esi edi,
	source: PTR BYTE,
	target: PTR BYTE

	mov esi, 0
	mov edi, 0

find_target_end:
	cmp source[esi], 0
	jz append_source

	add esi, TYPE BYTE
	jmp find_target_end

append_source:
	cmp target[edi], 0
	jz concat_done

	mov eax, esi
	add eax, edi

	mov ebx, source[edi]

	mov target[eax], ebx

	add esi, TYPE BYTE
	add edi, TYPE BYTE

	jmp append_source

concat_done:
	ret
StrConcat ENDP

END main
