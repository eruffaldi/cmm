.386P
if @Version gt 510
.model FLAT
else
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT DWORD USE32 PUBLIC 'BSS'
_BSS	ENDS
_TLS	SEGMENT DWORD USE32 PUBLIC 'TLS'
_TLS	ENDS
FLAT	GROUP _DATA, CONST, _BSS
	ASSUME	CS: FLAT, DS: FLAT, SS: FLAT
endif
PUBLIC _main                            ;main
EXTRN _printf:NEAR                      ;printf
EXTRN _getchar:NEAR                     ;getchar
_TEXT SEGMENT
_nl$ = -4
_nw$ = -8
_EOF$ = -12
_state$ = -16
_c$ = -20
_OUT$ = -24
_nc$ = -28
_IN$ = -32
_main PROC NEAR                         ;main
		push	ebp                                
		mov	ebp, esp                            
		sub	esp, 32                             
		push	1                                  ; 1
		mov	eax, DWORD PTR [esp]                
		mov	DWORD PTR _IN$[ebp], eax            
		pop	eax                                 
		push	-1                                 ; -1
		mov	eax, DWORD PTR [esp]                
		mov	DWORD PTR _EOF$[ebp], eax           
		pop	eax                                 
		push	0                                  ; 0
		mov	eax, DWORD PTR [esp]                
		mov	DWORD PTR _OUT$[ebp], eax           
		pop	eax                                 
		push	DWORD PTR _OUT$[ebp]               
		mov	eax, DWORD PTR [esp]                
		mov	DWORD PTR _state$[ebp], eax         
		pop	eax                                 
		push	0                                  ; 0
		mov	eax, DWORD PTR [esp]                
		mov	DWORD PTR _nc$[ebp], eax            
		mov	eax, DWORD PTR [esp]                
		mov	DWORD PTR _nw$[ebp], eax            
		mov	eax, DWORD PTR [esp]                
		mov	DWORD PTR _nl$[ebp], eax            
		pop	eax                                 
		$2:                                     
		call	_getchar                           
		add	esp, 0                              
		push	eax                                
		mov	eax, DWORD PTR [esp]                
		mov	DWORD PTR _c$[ebp], eax             
		push	DWORD PTR _EOF$[ebp]               
		pop	eax                                 
		pop	ebx                                 
		cmp	eax, ebx                            
		je	$1                                   
		$0:                                     
		push	DWORD PTR _nc$[ebp]                
		push	1                                  ; 1
		pop	eax                                 
		add	DWORD PTR [esp], eax                
		mov	eax, DWORD PTR [esp]                
		mov	DWORD PTR _nc$[ebp], eax            
		pop	eax                                 
		push	DWORD PTR _c$[ebp]                 
		push	10                                 ; 10
		pop	eax                                 
		pop	ebx                                 
		cmp	eax, ebx                            
		jne	$3                                  
		push	DWORD PTR _nl$[ebp]                
		push	1                                  ; 1
		pop	eax                                 
		add	DWORD PTR [esp], eax                
		mov	eax, DWORD PTR [esp]                
		mov	DWORD PTR _nl$[ebp], eax            
		pop	eax                                 
		$3:                                     
		jmp	$2                                  
		$1:                                     
		lea	eax, DWORD PTR _nc$[ebp]            
		push	eax                                
		                                        
		lea	eax, DWORD PTR _nw$[ebp]            
		push	eax                                
		                                        
		lea	eax, DWORD PTR _nl$[ebp]            
		push	eax                                
		                                        
		push	OFFSET FLAT:$SG0                   
		call	_printf                            
		add	esp, 16                             
		$uscita:                                
		mov	esp, ebp                            
		pop	ebp                                 
		ret                                     
_main ENDP                              ;main
                                        
_TEXT ENDS
_DATA	SEGMENT
$SG0	DB	  '%d %d %d', 0AH, 00H
_DATA	ENDS
END