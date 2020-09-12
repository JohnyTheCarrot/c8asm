.text
	:reset
		SET: v2, 0
		SET: v3, 0
	;

	:draw
		FNT: v2
		DRW: v0, v1, 5
		ADD: v0, 5
	;

	:reference
		> draw the expected numbers as a reference
		SET: v2, 9
		CALL: draw
		SET: v2, 4
		CALL: draw
		SET: v2, 1
		CALL: draw
		SET: v2, 2
		CALL: draw
		SET: v2, 0
		CALL: draw
		SET: v2, 4
		CALL: draw
		SET: v2, 8
		CALL: draw
		> add 6 so that the test is on the next line
		SET: v1, 6
		> reset v0 back to 0 so that the actual test is on the same x position
		SET: v0, 0
	;

	:main
		> the following code should print this to the screen,
		> if not, the assembler is malfunctioning
		> 9 4 1 2 0 4 8
		CALL: reference
		SET: v2, 8
		SET: v3, 1
		OR: v2, v3
		CALL: draw
		CALL: reset
		SET: v2, 12
		SET: v3, 5
		AND: v2, v3
		CALL: draw
		CALL: reset
		SET: v2, 9
		SET: v3, 8
		XOR: v2, v3
		CALL: draw
		ADDR: v2, v2
		CALL: draw
		SUBR: v2, v2
		CALL: draw
		SET: v2, 8
		SHRX: v2
		CALL: draw
		SHLX: v2
		CALL: draw
	;
