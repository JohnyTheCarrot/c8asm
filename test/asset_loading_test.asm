.data
	:b
		LDSP: 0, 0, 10, sprites/spritemap.png
	:r
		LDSP: 8, 0, 10, sprites/spritemap.png
	:u
		LDSP: 16, 0, 10, sprites/spritemap.png
	:h
		LDSP: 24, 0, 10, sprites/spritemap.png
	:m
		LDSP: 0, 11, 11, sprites/spritemap.png
	:o
		LDSP: 8, 11, 11, sprites/spritemap.png
	:e
		LDSP: 16, 11, 11, sprites/spritemap.png
	:n
		LDSP: 24, 11, 11, sprites/spritemap.png
	:t
		LDSP: 32, 11, 11, sprites/spritemap.png
.text
	:main
		SETI: b
		DRW: v0, v1, 10
		ADD: v0, 8
		SETI: r
		DRW: v0, v1, 10
		ADD: v0, 6
		SETI: u
		DRW: v0, v1, 10
		ADD: v0, 8
		SETI: h
		DRW: v0, v1, 10
		SET: v0, 0
		SET: v1, 10
		SETI: m
		DRW: v0, v1, 11
		ADD: v0, 10
		SETI: o
		DRW: v0, v1, 11
		ADD: v0, 8
		SETI: m
		DRW: v0, v1, 10
		ADD: v0, 10
		SETI: e
		DRW: v0, v1, 10
		ADD: v0, 8
		SETI: n
		DRW: v0, v1, 10
		ADD: v0, 8
		SETI: t
		DRW: v0, v1, 10
	;
