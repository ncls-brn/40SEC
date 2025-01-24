section .data
    hello_msg db "Hello, World!", 0xA 
    hello_len equ $ - hello_msg       

section .text
    global _start

_start:
     
    mov rax, 1          
    mov rdi, 1          
    lea rsi, [hello_msg] 
    mov rdx, hello_len  
    syscall             

     
    mov rax, 60         
    xor rdi, rdi        
    syscall             
