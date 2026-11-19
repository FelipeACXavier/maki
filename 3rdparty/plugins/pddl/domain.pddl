(define (domain robot-navigation)
  (:requirements :strips :typing)

  (:types room)

  (:predicates
    (at ?r - room)
    (connected ?from ?to - room)
  )

  (:action move
    :parameters (?from ?to - room)
    :precondition
      (and
        (at ?from)
        (connected ?from ?to)
      )
    :effect
      (and
        (not (at ?from))
        (at ?to)
      )
  )
)
