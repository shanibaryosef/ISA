	lw $s0, $zero, $imm, 0x100 				# s0 = mem[0x100]
	lw $s1, $zero, $imm, 0x101 				# s1 = mem[0x101]
	lw $s2, $zero, $imm, 0x102 				# s2 = mem[0x102]
	and $t0, $s0, $imm, 0xff				# t0 is s0 LSByte
	and $t1, $s2, $imm, 0xff				# t1 is s2 LSByte
	sub $a0, $t1, $t0, 0					# a0 is the diff between t0 and t1
	srl $t1, $s0, $imm, 8					# t1 is s0 MSByte
	srl $t2, $s2, $imm, 8					# t2 is s2 MSByte
	sub $a1, $t2, $t1, 0					# a1 is the diff between t1 and t2
	add $a2, $zero, $imm, 256				# a2 is 256
	add $a3, $zero, $imm, 256				# a3 is 256
	add $s0, $zero, $imm, 0x10000			# t0 is 2^16
find_coef1:		
	mul $t1, $a2, $a1, 0					# t1 is a2*a1
	mul $t0, $a3, $a0, 0					# t0 is a3*a0
	bge $imm, $t1, $t0, abs_diff1			# if t1 >= a0, compute different diff
	sub $t1, $t0, $t1, 0					# t1 = t0-t1
	beq $imm, $zero, $zero, compute_min1	# compute minimum value
abs_diff1:	
	sub $t1, $t1, $t0, 0					# t1 = t1-t0
compute_min1:	
	bgt $imm, $t1, $s0, end_loop1			# if t1>s0 end loop
	add $s0, $t1, $zero, 0					# else define new min
	add $s1, $a2, $zero, 0					# save a2 val of minimum - the fitting slope for y
	add $s2, $a3, $zero, 0					# save a3 val of minimum - the fitting slope for x
end_loop1:	
	add $a2, $a2, $imm, -1					# decrement a2
	bgt $imm, $a2, $zero, find_coef1		# if a2>0 continue loop
	add $a3, $a3, $imm, -1					# decrement a3
	add $a2, $zero, $imm, 256				# a2 is 256
	bgt $imm, $a3, $zero, find_coef1		# if a3>0 continue loop
	lw $s0, $zero, $imm, 0x100 				# s0 = mem[0x100]
	and $t0, $s0, $imm, 0xff				# get x of A
	srl $t1, $s0, $imm, 8					# get y of A
	sub $t1, $zero, $t1, 0					# get -y1
	mul $t0, $s2, $t0, 0					# get m1*x1
	mul $t1, $s1, $t1, 0					# get -m2*y1
	add $s0, $t1, $t0, 0					# get m1*x1 - m2*y1
fill_monitor:
	lw $a2, $zero, $imm, 0x101 				# a2 = mem[0x101]
	and $a3, $a2, $imm, 0xff				# Save x value of the left leg
	srl $a2, $a2, $imm, 8					# Save y value of the bottom leg
	add $t0, $zero, $zero, 0				# t0 = 0 (y index)
	add $t1, $zero, $zero, 0				# t1 = 0 (x index)
monitor_loop:
	add $a0, $zero, $zero, 0				# data to print is black initially
	bgt $imm, $t0, $a2, end_loop			# if y is out of range continue
	blt $imm, $t1, $a3, end_loop			# if x is out of range continue
	mul $t2, $s1, $t0, 0 					# compute m2*y
	add $t2, $t2, $s0, 0					# compute m2*y + b
	mul $a1, $s2, $t1, 0					# compute m1*x
	bgt $imm, $a1, $t2, end_loop			# if x passed the slope continue
	add $a0, $zero, $imm, 255				# data to print is white
end_loop:
	sll $t2, $t0, $imm, 8					# put y in the MSByte
	add $t2, $t2, $t1, 0					# put x in the LSByte
	out $t2, $imm, $zero, 20				# out monitor address according to y,x
	out $a0, $imm, $zero, 21				# out monitor data according to a0
	add $t2, $imm, $zero, 1					# t2 = 1
	out $t2, $imm, $zero, 22				# write to monitor
	add $t1, $t1, $imm, 1					# increment t1
	add $t2, $imm, $zero, 256				# t2 = 256
	blt $imm, $t1, $t2, monitor_loop		# if t1 smaller than 256 start another loop
	add $t1, $zero, $zero, 0				# t1 = 0
	add $t0, $t0, $imm, 1					# increment t0
	blt $imm, $t0, $t2, monitor_loop		# if t0 smaller than 256 start another loop
	halt $zero, $zero, $zero, 0				# halt
	.word 0x100 0x505
	.word 0x101 0x9005
	.word 0x102 0x90FE
	
	
	
	