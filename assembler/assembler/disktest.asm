	add $t2, $zero, $imm, 1					# $t2 = 1
	out $t2, $zero, $imm, 1					# enable irq1
	add $t1, $zero, $imm, handler			# $t2 = address of handler
	out $t1, $zero, $imm, 6					# set irqhandler as handler
	add $t0, $zero, $zero, 0 				# $t0 = 0
	add $t1, $imm, $zero, 0x100 			# $t1 = 0x100
	out $t1, $zero, $imm, 16				# set the disk buffer to 0x100
read_sector:	
	out $t0, $zero, $imm, 15				# set the disk sector to t0
	out $t2, $zero, $imm, 14				# start reading from disk
wait_for_sector:	
	in $t1, $zero, $imm, 17					# read the status of the disk
	bne $imm, $t1, $zero, wait_for_sector	# check if finished reading sector
	add $t1, $zero, $zero, 0				# start iteration for sector accumulation
accumulate:
	lw $a0, $t1, $imm, 0x100				# read the t1 member from the sector in 0x100
	lw $a1, $t1, $imm, 0x180				# read the t1 member from the sector in 0x180
	add $a1, $a1, $a0, 0 					# sum the values of a1 and a0
	sw $a1, $t1, $imm, 0x180				# write the t1 member of the sector in 0x180
	add $t1, $t1, $imm, 1					# accumulate t1
	add $a2, $zero, $imm, 128 				# $a2 = 128
	blt $imm, $t1, $a2, accumulate			# if didn't pass 128 values return 
	add $a2, $zero, $imm, 8 				# $a2 = 128
	blt $imm, $t0, $a2, read_sector			# if t0 didn't reach 8, read the next sector
	add $t1, $imm, $zero, 0x180 			# $t1 = 0x180
	out $t1, $zero, $imm, 16				# set the disk buffer to 0x180
	out $t0, $zero, $imm, 15				# set the disk sector to t0
	add $t2, $zero, $imm, 2					# $t2 = 2
	out $t2, $zero, $imm, 14				# start writing to disk
wait_for_sector2:	
	in $t1, $zero, $imm, 17					# read the status of the disk
	bne $imm, $t1, $zero, wait_for_sector2	# check if finished writing sector
	halt $zero, $zero, $zero, 0				# halt
	
handler:
	add $t0, $t0, $imm, 1					# increment t0
	out $zero, $zero, $imm, 4				# clear irq1 status
	reti $zero, $zero, $zero, 0				# return from interrupt