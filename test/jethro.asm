.data
  :empty
    LDSP: 16, 14, 13, sprites/jethro.png

  :j
    LDSP: 0, 0, 13, sprites/jethro.png
    LDSP: 16, 14, 13, sprites/jethro.png
  :e
    LDSP: 8, 0, 13, sprites/jethro.png
    LDSP: 16, 14, 13, sprites/jethro.png
  :t
    LDSP: 16, 0, 13, sprites/jethro.png
    LDSP: 16, 14, 13, sprites/jethro.png
  :h
    LDSP: 24, 0, 13, sprites/jethro.png
    LDSP: 16, 14, 13, sprites/jethro.png
  :r
    LDSP: 32, 0, 13, sprites/jethro.png
    LDSP: 16, 14, 13, sprites/jethro.png
  :o
    LDSP: 40, 0, 13, sprites/jethro.png
    LDSP: 16, 14, 13, sprites/jethro.png
  :jethro0
    LDSP: 0, 0, 15, sprites/jethro1.png
  :jethro1
    LDSP: 8, 0, 15, sprites/jethro1.png
  :jethro2
    LDSP: 16, 0, 15, sprites/jethro1.png
  :jethro3
    LDSP: 0, 15, 15, sprites/jethro1.png
  :jethro4
    LDSP: 8, 15, 15, sprites/jethro1.png
  :jethro5
    LDSP: 16, 15, 15, sprites/jethro1.png

.text
  :animate
    SET: v2, 1
    SET: v4, 0
    :for
      ADD: v4, 1
      ADDI: v2
      DRW: v0, v1, 13
      :for-1
        ADD: v3, 1 
      SE: v3, 5
      JP: for-1
      SET: v3, 0
      SE: v4, 13
      DRW: v0, v1, 13
    SE: v4, 13
    JP: for
  ;

  :draw-starfox
    SETI: jethro0
    DRW: v0, v1, 15
    SETI: jethro1
    ADD: v0, 8
    DRW: v0, v1, 15
    SETI: jethro2
    ADD: v0, 8
    DRW: v0, v1, 15
    SET: v0, 22
    SET: v1, 21
    SETI: jethro3
    DRW: v0, v1, 15
    SETI: jethro4
    ADD: v0, 8
    DRW: v0, v1, 15
    SETI: jethro5
    ADD: v0, 8
    DRW: v0, v1, 15
  ;

  :main
    SET: v0, 11
    SET: v1, 10
    SET: v5, 13
    SETI: empty
    CALL: animate
    SETI: j
    ADDI: v5
    ADD: v0, 6
    CALL: animate
    SETI: e
    ADDI: v5
    ADD: v0, 8
    CALL: animate
    SETI: t
    ADDI: v5
    ADD: v0, 6
    CALL: animate
    SETI: h
    ADDI: v5
    ADD: v0, 8
    CALL: animate
    SETI: r
    ADDI: v5
    ADD: v0, 6
    CALL: animate
    SET: v3, 0
    :for-2
      ADD: v3, 1 
    SE: v3, 50
    JP: for-2
    CLS:
    > starfox logo
    SET: v0, 22
    SET: v1, 6
    CALL: draw-starfox
    SET: v3, 0
    :for-3
      ADD: v3, 1 
    SE: v3, 200
    JP: for-3
    CLS:
  ;
