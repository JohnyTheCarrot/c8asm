.data
	:r
		0b11110000
		0b10010000
		0b11100000
		0b10010000
		0b10010000
	:p
		0b11110000
		0b10010000
		0b11110000
		0b10000000
		0b10000000
	:s
		0b11110000
		0b10000000
		0b11110000
		0b00010000
		0b11110000
	:n
		0b10010000
		0b11010000
		0b10110000
		0b10010000
		0b10010000
	:t
		0b11111000
		0b00100000
		0b00100000
		0b00100000
		0b00100000
.text
	:o
		SET: v0, 0
		FNT: v0
		ADD: v1, 5
		DRW: v1, v2, 5
	;
	:droop
		> D
		SET: v0, 0xD
		FNT: v0
		DRW: v1, v2, 5
		> R
		SETI: r
		ADD: v1, 5
		DRW: v1, v2, 5
		> O * 2
		CALL: o
		CALL: o
		> P
		SETI: p
		ADD: v1, 5
		DRW: v1, v2, 5
		> \n
		SET: v1, 0
		SET: v2, 6
		> S
		SETI: s
		DRW: v1, v2, 5
		> N
		SETI: n
		ADD: v1, 5
		DRW: v1, v2, 5
		> O * 2
		CALL: o
		CALL: o
		> T
		SETI: t
		ADD: v1, 5
		DRW: v1, v2, 5
	;
	:main
		CLS:
		CALL: droop
	;
