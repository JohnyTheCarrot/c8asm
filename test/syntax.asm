.text
	:test
		SET: v0, 2
		SET: v1, 0xF
	;
	> The : here is because of a bug in the split_string code,
	> it will not recognize CLRSCRN as a command unless it has a : at the end
	> this applies to all commands without params. Please remove this comment when this is fixed.
	CLRSCRN:
	SET: v0, 1
	CALL: test
