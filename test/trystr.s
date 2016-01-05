.set noreorder
.set volatile
.set noat
#include<regdef.h>
.data                                   
.text
.align 3
.globl f
.ent f
f:
		ldgp	gp,0(pv)                           
		lda	sp,-48(sp)                          
		.frame fp, 48, ra , 0                   
		stq	ra,16(sp)                           
		stq	fp,24(sp)                           
		stq	s0,32(sp)                           
		bis	sp,sp,fp                            
		.prologue 1                             
		lda	s0,16(sp)                           
		bis	$31,8198160,$0                      // 8198160
		subq	s0, 8,s0                           
		stq	$0,0(s0)                            
		lda	$0,40(fp)                           //cp
		subq	s0, 8,s0                           
		stq	$0,0(s0)                            
		ldq	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldq	t1,0(s0)                            
		stq	t1,0(t0)                            
		                                        
		ldl	$0,0(s0)                            
		addq	s0, 8,s0                           
		bis	$31,48,$0                           // '0'
		subq	s0, 8,s0                           
		stl	$0,0(s0)                            
		ldq	t0,0(s0)                            
		addq	s0, 8,s0                           
		ldl	t1,0(s0)                            
		ldq_u	t2,0(t0)maskbl	t2,t0,t2insbl	t1,t0,t1bis	t1,t2,t1stq_u	t1,0(t0)
		ldl	$0,0(s0)                            
		addq	s0, 8,s0                           
		$1kkk:                                  
		bis	sp,sp,fp                            
		ldq	s0,32(sp)                           
		ldq	ra,16(sp)                           
		ldq	fp,24(sp)                           
		addq	sp, 48,sp                          
		ret $31, (ra), 1                        
.end f
                                        
