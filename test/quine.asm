.data
	:code
		0b00100010
		0b00000110
		0b00010010
		0b00000100
		0b00010010
		0b00000010
		0b11111111
		0b11111111
		0b11111111
		0b11111111
		0b11111111
		0b11111111
		0b11111111
.text
	:draw
		DRW: v1, v2, 1
	;
	:next_line
		SET: v1, 0
		ADD: v2, 1
	;
	:draw_code
		CALL: draw
		ADDI: v0
		SE: v1, 30
		ADD: v1, 5
		SNE: v1, 30
		CALL: next_line
		JP: draw_code
	;
	:main
		SET: v0, 1
		SETI: code
		CALL: draw_code
	;