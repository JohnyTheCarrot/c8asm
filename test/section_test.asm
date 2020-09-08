>
>	PLEASE NOTE:
>	This piece of assembly is NOT meant to be efficient or short,
>	it's purpose is to test the splitting of sections over the document,
>	so that one may put multiple text or data sections spread over the document.
>

.text
	:main
		CALL: draw_b
		CALL: draw_r
		CALL: draw_u
        CALL: draw_h
        CALL: newline
        CALL: draw_m
        CALL: draw_o
        CALL: draw_m
        CALL: draw_e
        CALL: draw_n
        CALL: draw_t
	;

 > b
.data
	:b
		LDSP: 0, 0, 10, sprites/spritemap.png

.text
	:draw_b
		SETI: b
		DRW: v0, v1, 10
	;

> r
.data
	:r
		LDSP: 8, 0, 10, sprites/spritemap.png

.text
	:draw_r
		ADD: v0, 8
		SETI: r
		DRW: v0, v1, 10
	;

> u
.data
	:u
		LDSP: 16, 0, 10, sprites/spritemap.png

.text
	:draw_u
		ADD: v0, 6
		SETI: u
		DRW: v0, v1, 10
	;

> h
.data
	:h
		LDSP: 24, 0, 10, sprites/spritemap.png

.text
	:draw_h
		ADD: v0, 8
		SETI: h
		DRW: v0, v1, 10
	;

> newline
.text
	:newline
		SET: v0, 0
		SET: v1, 10
	;

> m
.data
	:m
		LDSP: 0, 11, 11, sprites/spritemap.png

.text
	:draw_m
		SETI: m
		DRW: v0, v1, 10
	;

> o
.data
	:o
		LDSP: 8, 11, 11, sprites/spritemap.png

.text
	:draw_o
		ADD: v0, 10
		SETI: o
		DRW: v0, v1, 10
		ADD: v0, 8
	;

> e
.data
	:e
		LDSP: 16, 11, 11, sprites/spritemap.png

.text
	:draw_e
		ADD: v0, 10
		SETI: e
		DRW: v0, v1, 10
	;

> n
.data
	:n
		LDSP: 24, 11, 11, sprites/spritemap.png

.text
	:draw_n
		ADD: v0, 8
		SETI: n
		DRW: v0, v1, 10
	;

> t
.data
	:t
		LDSP: 32, 11, 11, sprites/spritemap.png

.text
	:draw_t
		ADD: v0, 8
		SETI: t
		DRW: v0, v1, 10
	;
