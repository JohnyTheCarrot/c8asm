.text
	:a
		FNT: v0
		DRW: v1, v2, 5
		ADD: v1, 5
		ADD: v3, 1
	;
	:y
		SE: v3, 12
		CALL: a
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
		SET: v0, 0xA
		SET: v1, 0
		CALL: y
		ADD: v2, 1
		SET: v3, 0
	;
	SE: v2, 2
	CALL: main
