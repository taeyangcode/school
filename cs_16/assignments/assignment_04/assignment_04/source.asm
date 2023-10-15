; Assignment #4

; Program Description: This assignment has you practice writing code for conditional processing in assembly language.
; Author: Corey Mostero 2566652
; Creation Date: 14 October 2023
; Revisions: N/A
; Date: 22 October 2023

.386
.model flat, stdcall
.stack 4096
ExitProcess PROTO, dwExitCode: DWORD

.data
array SWORD 50 DUP(?)
sentinel SWORD 0FFFFh

val1 DWORD ?

.code
main PROC
	mov ebx, 0
	mov val1, 24

	call part_four

	INVOKE ExitProcess, 0
main ENDP

part_one PROC
	mov esi, OFFSET array
	mov ecx, LENGTHOF array

L1:
	cmp WORD PTR [esi], 0			; check for zero
	jnz NonZero						; zero flag not set -> located first nonzero value at esi

	add esi, TYPE array				; increment counter
	loop L1							; loop
	
	mov esi, DWORD PTR sentinel		; iteration complete && nonzero value not found -> point esi to sentinel's value
	ret								; nonzero value not found, exit function

NonZero:
	ret								; nonzero value found, exit function
part_one ENDP

part_two PROC
 	cmp ebx, ecx					; compare values of registers ebx and ecx
	ja Return						; ebx > ecx -> jump to ret, else perform listed instructions
 	
 	mov eax, 5				
 	mov edx, 6

Return: 	
	ret
part_two ENDP

part_three PROC
	cmp ebx, ecx					; compare values of ebx & ecx
	jnle Return						; ebx !<= ecx -> return early

	cmp ecx, edx					; compare values of ecx & edx
	jng Return						; ecx !> edx -> return early

	mov eax, 5						; neither of the above conditions are true -> perform instructions
	mov edx, 6

Return:
	ret
part_three ENDP

part_four PROC
while_loop:
	cmp ebx, val1					; compare values of ebx & val1
	jnle Return						; ebx !<= val1 -> return early

	add ebx, 5						; ebx += 5
	dec val1						; --val1

	jmp while_loop

	ret

Return:
	ret
part_four ENDP

CalcGrade PROC


	ret
CalcGrade ENDP

END main