(define (problem reach-room-c)
  (:domain robot-navigation)

  (:objects
    a b c - room
  )

  (:init
    (at a)
    (connected a b)
    (connected b c)
  )

  (:goal
    (at c)
  )
)
