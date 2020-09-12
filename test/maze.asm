.data
  :drawing0
    0b10000000
    0b01000000
    0b00100000
    0b00010000
  :drawing1
    0b00100000
    0b01000000
    0b10000000
    0b00010000

.text
  :main
    SETI: drawing0
    RND: v2, 1
    SE: v2, 1
    SETI: drawing1
    DRW: v0, v1, 4
    ADD: v0, 4
    SE: v0, 0x40
    JP: main
    SET: v0, 0
    ADD: v1, 4
    SE: v1, 0x20
    JP: main
  ;
