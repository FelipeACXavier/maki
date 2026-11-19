(define (problem stacked-boxes)
  (:domain robot-navigation)

  (:objects
    a b c - room
    box1 box2 box3 - object
  )

  (:init
    (at a)
    (handempty)

    (object-at box1 a)
    (object-at box2 a)
    (object-at box3 b)

    (on-floor box1)
    (on box2 box1)
    (clear box2)

    (on-floor box3)
    (clear box3)

    ; The robot may move back and forth through the rooms
    (connected a b)
    (connected b a)
    (connected b c)
    (connected c b)
  )

  (:goal
    (and
      (object-at box1 c)
      (on-floor box1)
      (object-at box2 c)
      (object-at box3 c)
      (on box2 box3)
      (handempty)
    )
  )
)
