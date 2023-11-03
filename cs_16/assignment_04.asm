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

INCLUDE Irvine32.inc
.data
array SWORD 50 DUP(?)
sentinel SWORD 0FFFFh

val1 DWORD ?

grade_score DWORD ?

.code
main PROC
	call test_CalcGrade

	INVOKE ExitProcess, 0
main ENDP

part_one PROC
	mov esi, OFFSET array
	mov ecx, LENGTHOF array

L1:
	cmp WORD PTR [esi], 0			; check for zero
	jnz NonZero				; zero flag not set -> located first nonzero value at esi

	add esi, TYPE array			; increment counter
	loop L1					; loop
	
	mov esi, DWORD PTR sentinel		; iteration complete && nonzero value not found -> point esi to sentinel's value
	ret					; nonzero value not found, exit function

NonZero:
	ret					; nonzero value found, exit function
part_one ENDP

part_two PROC
 	cmp ebx, ecx				; compare values of registers ebx and ecx
	ja Return				; ebx > ecx -> jump to ret, else perform listed instructions
 	
 	mov eax, 5
 	mov edx, 6

Return: 	
	ret
part_two ENDP

part_three PROC
	cmp ebx, ecx				; compare values of ebx & ecx
	jnle Return				; ebx !<= ecx -> return early

	cmp ecx, edx				; compare values of ecx & edx
	jng Return				; ecx !> edx -> return early

	mov eax, 5				; neither of the above conditions are true -> perform instructions
	mov edx, 6

Return:
	ret
part_three ENDP

part_four PROC
while_loop:
	cmp ebx, val1				; compare values of ebx & val1
	jnle Return				; ebx !<= val1 -> return early

	add ebx, 5				; ebx += 5
	dec val1				; --val1

	jmp while_loop

	ret

Return:
	ret
part_four ENDP

CalcGrade PROC
	cmp grade_score, 90			; compare grade_score & 90 (>= 90 -> A)
	jae GradeA				; grade_score >= 90 -> assign 'A' to al
	
	cmp grade_score, 80			; compare grade_score & 80 (>= 80 -> B)
	jae GradeB          			; grade_score >= 80 -> assign 'B' to al
	
	cmp grade_score, 70			; compare grade_score & 70 (>= 70 -> C)
	jae GradeC          			; grade_score >= 70 -> assign 'C' to al
	
	cmp grade_score, 60			; compare grade_score & 60 (>= 60 -> D)
	jae GradeD          			; grade_score >= 60 -> assign 'D' to al

	jmp GradeF				; else -> assign 'F' to al
GradeA:
	mov al, 'A'
	ret

GradeB:
	mov al, 'B'
	ret

GradeC:
	mov al, 'C'
	ret

GradeD:
	mov al, 'D'
	ret

GradeF:
	mov al, 'F'
	ret

	ret
CalcGrade ENDP

test_CalcGrade PROC
	call Randomize			; reseed randomizer

	mov eax, 51				; move random range into eax [0, 51) or [0, 50]
	call RandomRange			; generate random integer
	add eax, 50				; add 50 to range to get integer in [50, 101) or [50, 100]

	mov grade_score, eax			; initialize grade_score to score in respective range
	call CalcGrade				; call CalcGrade procedure which should initialize al to the respective letter grade

	mov eax, 51
	call RandomRange
	add eax, 50

	mov grade_score, eax
	call CalcGrade

	mov eax, 51
	call RandomRange
	add eax, 50

	mov grade_score, eax
	call CalcGrade

	mov eax, 51
	call RandomRange
	add eax, 50

	mov grade_score, eax
	call CalcGrade

	mov eax, 51
	call RandomRange
	add eax, 50

	mov grade_score, eax
	call CalcGrade

	mov eax, 51
	call RandomRange
	add eax, 50

	mov grade_score, eax
	call CalcGrade

	mov eax, 51
	call RandomRange
	add eax, 50

	mov grade_score, eax
	call CalcGrade

	mov eax, 51
	call RandomRange
	add eax, 50

	mov grade_score, eax
	call CalcGrade

	mov eax, 51
	call RandomRange
	add eax, 50

	mov grade_score, eax
	call CalcGrade

	mov eax, 51
	call RandomRange
	add eax, 50

	mov grade_score, eax
	call CalcGrade
	
	ret
test_CalcGrade ENDP

END main
