.data
	:stages
		LDSP: 0, 0, 8, sprites/dissolve1.png
		LDSP: 9, 0, 8, sprites/dissolve1.png
		LDSP: 18, 0, 8, sprites/dissolve1.png
		LDSP: 27, 0, 8, sprites/dissolve1.png
		LDSP: 36, 0, 8, sprites/dissolve1.png
.text
	:fill-screen
		:for-y
			SET: v2, 0
			SET: v1, 0
			:for-x
				ADD: v1, 1
				DRW: v2, v3, 8
				ADD: v2, 8
			SE: v1, 8
			JP: for-x
			ADD: v0, 1
			ADD: v3, 8
		SE: v0, 4
		JP: for-y
		SET: v0, 0
		SET: v1, 0
	;
	:main
		SETI: stages
		SET: v5, 8
		:next-stage
			CALL: fill-screen
			ADD: v4, 1
			ADDI: v5
		SE: v4, 5
		JP: next-stage
		CLS:
	;
