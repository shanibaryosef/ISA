	add $t2, $zero, $imm, 1					# $t2 = 1
	sll $sp, $t2, $imm, 11					# set $sp = 1 << 11 = 2048
	lw $a0, $imm, $zero, 0x100				# get n from address 0x100
	lw $a1, $imm, $zero, 0x101				# get k from address 0x101
	jal $ra, $imm, $zero, binom				# calc $v0 = binom(n, k)
	sw $v0, $zero, $imm, 0x102				# store binom(n, k) in 0x102
	halt $zero, $zero, $zero, 0				# halt
binom:
	add $sp, $sp, $imm, -4					# adjust stack for 4 items
	sw $s0, $sp, $imm, 3					# save $s0
	sw $ra, $sp, $imm, 2					# save return address
	sw $a1, $sp, $imm, 1					# save argument
	sw $a0, $sp, $imm, 0					# save argument
	beq $imm, $a1, $zero, L1				# jump to L1 if  k==0
	beq $imm, $a0, $a1, L1					# jump to L1 if  n==k
	sub $a0, $a0, $imm, 1					# calculate n - 1
	jal $ra, $imm, $zero, binom				# calc $v0=binom(n-1,k)
	add $s0, $v0, $zero, 0					# $s0 = binom(n-1,k)
	lw $a0, $sp, $imm, 0					# restore $a0 = n
	lw $a1, $sp, $imm, 1					# restore $a1 = k
	sub $a0, $a0, $imm, 1					# calculate n - 1
	sub $a1, $a1, $imm, 1					# calculate k - 1
	jal $ra, $imm, $zero, binom				# calc binom(n-1, k-1)
	add $v0, $v0, $s0, 0					# $v0 = binom(n-1,k) + binom(n-1, k-1)
	lw $a0, $sp, $imm, 0					# restore $a0
	lw $a1, $sp, $imm, 1					# restore $a1
	lw $ra, $sp, $imm, 2					# restore $ra
	lw $s0, $sp, $imm, 3					# restore $s0
	beq $imm, $zero, $zero, L2				# jump to L2
L1:
	add $v0, $zero, $imm, 1					# binom(n,k) = 1
L2:
	add $sp, $sp, $imm, 4					# pop 4 items from stack
	beq $ra, $zero, $zero, 0				# and return
	.word 0x100 5
	.word 0x101 2
