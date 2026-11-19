(define (domain robot-navigation)
  (:requirements :strips :typing)

  (:types room object)

  (:predicates
    (at ?r - room)
    (connected ?from ?to - room)
    (object-at ?r - room ?obj - object)
    (carrying ?obj - object)
    (handempty)
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

  (:action pick
    :parameters (?obj - object ?r - room)
    :precondition
      (and
        (at ?r)
        (object-at ?obj ?r)
        (handempty)
      )
    :effect
      (and
        (not (object-at ?obj ?r))
        (not (handempty))
        (carrying ?obj)
      )
  )

  (:action place
    :parameters (?obj - object ?r - room)
    :precondition
      (and
        (at ?r)
        (carrying ?obj)
      )
    :effect
      (and
        (not (carrying ?obj))
        (object-at ?obj ?r)
        (handempty)
      )
  )
)
