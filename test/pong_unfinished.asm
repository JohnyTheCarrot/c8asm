.data
	:player
		0b10000000
		0b10000000
		0b10000000
		0b10000000
		0b10000000

.text
	:player0
		GETK: v2
		CLS:
		SE: v2, 4
		JP: check-down-p0
		SET: v0, 1
		SUB: v1, v0
		SET: v0, 0
		JP: update_players
		:check-down-p0
			SE: v2, 8
			;
			ADD: v1, 1
		CALL: update_players
	;

	:update_players
		CALL: update_player0
		CALL: update_player1
	;

	:update_player0
		SET: v0, 0
		DRW: v0, v1, 5
	;

	:player1
		GETK: v2
		CLS:
		SE: v2, 5
		JP: check-down-p1
		SET: v0, 1
		SUB: v3, v0
		SET: v0, 0
		JP: update_players
		:check-down-p1
			SE: v2, 9
			;
			ADD: v3, 1
		CALL: update_players
	;
	
	:update_player1
		SET: v0, 0xFF
		DRW: v0, v3, 5
	;

	:main
		SETI: player
		CALL: update_player0
		CALL: update_player1
		:loop
			CALL: player0
			CALL: player1
		JP: loop
	;
