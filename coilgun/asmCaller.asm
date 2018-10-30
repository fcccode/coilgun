_TEXT SEGMENT


PUBLIC asmCall

asmCall PROC
; RCX holds the func pointer
; RDX holds the args array
; R8  holds the arg amount
push rbp ; prologue
mov rbp, rsp; prologue continues

; preserve args

push r10;
push r11; those will be restored at the end
push r12;
mov r10, rcx; now r10 func pointer
mov r11, rdx; r11 arg array
mov r12, r8	; r12 argc

test r8,r8	; check if argc == 0
jz noArgs	
cmp r8, 4	; check if argc <= 4
jbe lessOrEqualFour
jmp moreThanFour

noArgs LABEL NEAR
call r10; 
jmp restoreregs

lessOrEqualFour LABEL NEAR
; general registers should be preserved by the caller, so we dont need to restore them
cmp r12,1;
je  loadone; load rcx
cmp r12,2;
je	loadtwo; load rdx
cmp r12,3;
je  loadthree; load r8
jmp loadfour

loadfour LABEL NEAR ; will go through each label
mov r9,  [r11+24]
mov r9, [r9]
loadthree LABEL NEAR
mov r8,  [r11+16]
mov r8, [r8]
loadtwo LABEL NEAR
mov rdx, [r11+8]
mov rdx, [rdx]
loadone LABEL NEAR
mov rcx, [r11]
mov rcx, [rcx]
call r10;
jmp restoreregs;



moreThanFour LABEL NEAR
; load four regs
mov rcx, [r11]
mov rcx, [rcx]
mov rdx, [r11+8]
mov rdx, [rdx]
mov r8,  [r11+16]
mov r8, [r8]
mov r9,  [r11+24]
mov r9, [r9]
mov rsi, 4

continueloop LABEL NEAR
cmp rsi,12 ; if counter == argc, then exit
je exitloop
inc rsi ; increment counter
mov rdi, [r11+rsi*8]
push [rdi]
jmp continueloop


exitloop LABEL NEAR
call r10
sub rsi, 4 ; substract 4, because they were loaded in regs
imul rsi, 8;
sub rsp, rsi; clean stack
jmp restoreregs


restoreregs LABEL NEAR
pop r12	;
pop r11	;
pop r10	;
jmp finalize

finalize LABEL NEAR
mov rsp, rbp
pop rbp
ret

asmCall ENDP

PUBLIC callShellcode

callShellcode PROC
; RCX holds the shellcode addr

push rbp ; 
mov rbp, rsp;
call rcx ; call shellcode
mov rsp, rbp
pop rbp
ret

callShellcode ENDP

_TEXT ENDS

END
