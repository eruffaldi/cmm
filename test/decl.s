.set noreorder
.set volatile
.set noat
#include<regdef.h>
.data                                   
.globl cptr                             
.globl arrayfunc                        
.globl iarray                           
.globl arrayptr                         
.globl arrayfunc2                       
.globl funcptr                          
.globl fvar                             
.globl ivar                             
.globl iarray2                          
.data                                   
.align 3                                
cptr:                                   
.space 8                                
.align 3                                
arrayfunc:                              
.space 320                              
.align 3                                
iarray:                                 
.space 400                              
.align 3                                
arrayptr:                               
.space 8                                
.align 3                                
arrayfunc2:                             
.space 320                              
.align 3                                
funcptr:                                
.space 8                                
.align 3                                
fvar:                                   
.space 4                                
.align 3                                
ivar:                                   
.space 4                                
.align 3                                
iarray2:                                
.space 65109376                         
.text
