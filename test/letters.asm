.text
	:reset
		CLS:
		SET: v1, 0
		SET: v2, 0
		SET: v3, 0
	;
	:char
		FNT: v0
		DRW: v1, v2, 5
		ADD: v1, 5
		ADD: v3, 1
	;
	:y
		SE: v3, 12
		CALL: char
		SNE: v3, 12
		ADD: v2, 6
		SNE: v2, 30
		;
		SNE: v3, 12
		SET: v1, 0
		SNE: v3, 12
		SET: v3, 0
		SNE: v3, 12
		;
		JP: y
	;
	:main
		SET: v0, 0x0
		SET: v1, 0
		CALL: y
		ADD: v2, 1
		SET: v3, 0
		:another-one
			CALL: reset
			SE: v0, 0xF
			ADD: v0, 1
			SE: v2, 2
			CALL: y
			SNE: v0, 0xF
			SET: v0, 0
		SE: v0, 0xF
		JP: another-one
	;
