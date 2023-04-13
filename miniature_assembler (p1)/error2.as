	lw	1,0,lbl1	
	addi	1,1,2
	halt
lbl1	.fill 	4
lbl1	.fill	1	# Error: Duplicate label
