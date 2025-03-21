	add $t0, $zero, $imm, 1				# $t0 = 1
	add $t1, $zero, $imm, 15			# $t1 = 15
begin_iter:		
	add $t0, $zero, $zero, 0			# $t0 = 0
	add $t2, $zero, $zero, 0			# $t2 = 0
for_loop:	
	lw $a0, $t2, $imm, 0x100			# read the t2 value from array in 0x100
	lw $a1, $t2, $imm, 0x101			# read next value from array in 0x100
	ble $imm, $a0, $a1, end_for_loop	# if no need to swap, end_loop
	sw $a1, $t2, $imm, 0x100			# write a1 to the t2 value in array in 0x100
	sw $a0, $t2, $imm, 0x101			# write a0 to the next value in array in 0x100
	add $t0, $zero, $imm, 1				# set the t0 to 1 to insicate swap happened
end_for_loop:
	add $t2, $t2, $imm, 1				# increment t2
	blt $imm, $t2, $t1, for_loop		# if t2 smaller than 15, repeat loop
	bne $imm, $t0, $zero, begin_iter	# if finished iterations, and indicator for swap is high, start again
	halt $zero, $zero, $zero, 0			# halt
	.word 0x100 15
	.word 0x101 14
	.word 0x102 13
	.word 0x103 12
	.word 0x104 11
	.word 0x105 10
	.word 0x106 9
	.word 0x107 8
	.word 0x108 7
	.word 0x109 6
	.word 0x10A 5
	.word 0x10B 4
	.word 0x10C 3
	.word 0x10D 2
	.word 0x10E 1
	.word 0x10F 0