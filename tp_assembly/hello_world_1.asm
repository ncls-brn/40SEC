bits 32
extern printf
global main

section .data
Hello db "hello world!", 10, 0

section .text
main:
    push ebp                
    mov ebp, esp            

    sub esp, 16             
    lea eax, [Hello]        
    push eax                
    call printf             
    add esp, 16             

    xor eax, eax           
    leave                   
    ret             
