.set noreorder
.set volatile
.set noat
#include<regdef.h>
.data                                   
.globl p                                
.globl glo                              
.globl hello                            
.globl ww                               
.globl pippo                            
.globl a                                
.data                                   
.align 3                                
p:                                      
.space 8                                
.align 3                                
glo:                                    
.space 4                                
.align 3                                
hello:                                  
.space 4                                
.align 3                                
ww:                                     
.space 4                                
.align 3                                
pippo:                                  
.space 8                                
.align 3                                
a:                                      
.space 44                               
.text
.align 3
.globl q
.ent q
q:
		ldgp	gp,0(pv)                           
		lda	sp,-64(sp)                          
		.frame fp, 64, ra , 0                   
		stq	ra,24(sp)                           
		stq	fp,32(sp)                           
		stq	s0,40(sp)                           
		bis	sp,sp,fp                            
		.prologue 1                             
		stl	a0,56(fp)                           
		lda	s0,24(sp)                           
		ldl	$0,56(fp)                           //j
		subq	s0, 8,s0                           
		stl	$0,0(s0)                            
		lda	$0,glo                              //glo//glo
		subq	s0, 8,s0                           
		stq	$0,0(s0)                            
		ldq	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldl	t1,0(s0)                            
		stl	t1,0(t0)                            
		                                        
		ldl	$0,0(s0)                            
		addq	s0, 8,s0                           
		bis	$31,40,$0                           // 40
		subq	s0, 8,s0                           
		stl	$0,0(s0)                            
		ldl	$0,56(fp)                           //j
		subq	s0, 8,s0                           
		stl	$0,0(s0)                            
		ldl	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldl	t1,0(s0)                            
		addq	s0, 8,s0                           
		mull	t0,t1,t0                           
		subq	s0, 8,s0                           
		stl	t0,0(s0)                            
		ldl	$0,56(fp)                           //j
		subq	s0, 8,s0                           
		stl	$0,0(s0)                            
		bis	$31,2,$0                            // 2
		subq	s0, 8,s0                           
		stl	$0,0(s0)                            
		ldl	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldl	t1,0(s0)                            
		addq	s0, 8,s0                           
		mull	t0,t1,t0                           
		subq	s0, 8,s0                           
		stl	t0,0(s0)                            
		ldl	$0,56(fp)                           //j
		subq	s0, 8,s0                           
		stl	$0,0(s0)                            
		ldl	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldl	t1,0(s0)                            
		addq	s0, 8,s0                           
		subl	t0,t1,t0                           
		subq	s0, 8,s0                           
		stl	t0,0(s0)                            
		ldl	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldl	t1,0(s0)                            
		addq	s0, 8,s0                           
		addl	t0,t1,t0                           
		subq	s0, 8,s0                           
		stl	t0,0(s0)                            
		lda	$0,56(fp)                           //j
		subq	s0, 8,s0                           
		stq	$0,0(s0)                            
		ldq	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldl	t1,0(s0)                            
		stl	t1,0(t0)                            
		                                        
		ldl	$0,0(s0)                            
		addq	s0, 8,s0                           
		ldl	$0,56(fp)                           //j
		subq	s0, 8,s0                           
		stl	$0,0(s0)                            
		ldl	$0,0(s0)                            
		addq	s0, 8,s0                           
		$1kkk:                                  
		bis	sp,sp,fp                            
		ldq	s0,40(sp)                           
		ldq	ra,24(sp)                           
		ldq	fp,32(sp)                           
		addq	sp, 64,sp                          
		ret $31, (ra), 1                        
.end q
                                        
