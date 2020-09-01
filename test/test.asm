.text:
	:clear_screen
		> TODO: fix the bug that forces you to end every command with a ':',
		> even if it has no parameters
		CLRSCRN:
	.
	:draw_character
		FONT: v0
		DRAW: v1, v2, 5
		SKIP_NE: v0, 75
		;
		ADD: v0, 15
		CLRSCRN:
		CALL: draw_character
	;
	:main
		CALL: draw_character
	;
