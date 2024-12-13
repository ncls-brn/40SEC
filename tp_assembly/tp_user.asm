section .data
  input_msg db "Enter your first name?", 0xA    ; 
  input_msg_len equ $ - input_msg ; 
  msg db "Hello, "
  msg_len equ $ - msg ; 
  newline db 0xA       ; 
  newline_len equ $ - newline

section .bss
    buffer resb 128          ; 

section .text
    global _start

_start:
  ; Ask for input
  mov rax, 1              ; 
  mov rdi, 1              ;
  lea rsi, [input_msg]
  mov rdx, input_msg_len
  syscall

  ; Read input from the terminal (stdin)
  mov rax, 0              ; 
  mov rdi, 0              ; 
  lea rsi, [buffer]       ; 
  mov rdx, 128            ;
  syscall                 ; 

  ; Add exclamation mark
  lea rbx, [buffer]        ; 
find_newline:
  cmp byte [rbx], 10       ; 
  je replace_with_excl     ; 
  inc rbx                  ; 
  jmp find_newline         ; 

replace_with_excl:
  mov byte [rbx], '!'      ; 
  inc rbx                  ; 
  mov byte [rbx], 0        ; 

  ; Write message
  mov rax, 1              ; 
  mov rdi, 1              ; 
  lea rsi, [msg]
  mov rdx, msg_len
  syscall

  ; Write the input back to the terminal (stdout)
  mov rax, 1              ; 
  mov rdi, 1              ; 
  lea rsi, [buffer]       ; 
  mov rdx, 128
  syscall                 ; 

  ; Write newline
  mov rax, 1              ; 
  mov rdi, 1              ; 
  lea rsi, [newline]      ; 
  mov rdx, newline_len    ; 
  syscall

  ; Exit the program
  mov rax, 60             ; 
  mov rdi, 0            ; 
  syscall                 ; 
