(define (problem transport-box)
  (:domain robot-navigation)
  (:objects
    a b c - room
    box1 - object
  )
  (:init
    (at a)
    (object-at box1 a)
    (handempty)
    (connected a b)
    (connected b c)
  )
  (:goal
    (object-at box1 c)
  )
)
