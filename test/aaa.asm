.text
	:a
		FONT: v0
		DRAW: v1, v2, 5
		ADD: v1, 5
		ADD: v3, 1
	;
	:y
		SKIP_EQ: v3, 12
		CALL: a
		SKIP_NE: v3, 12
		ADD: v2, 6
		SKIP_NE: v2, 30
		;
		SKIP_NE: v3, 12
		SET: v1, 0
		SKIP_NE: v3, 12
		SET: v3, 0
		SKIP_NE: v3, 12
		;
		CALL: y
	;
	:main
		SET: v0, 50
		SET: v1, 0
		CALL: y
		ADD: v2, 1
		SET: v3, 0
	;
	SKIP_EQ: v2, 2
	CALL: main
