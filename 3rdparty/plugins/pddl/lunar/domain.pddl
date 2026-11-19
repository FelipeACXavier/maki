(define (domain robot-navigation)
  (:requirements :strips :typing :equality)

  (:types room object)

  (:predicates
    (at ?r - room)
    (connected ?from ?to - room)
    (object-at ?obj - object ?r - room)
    (carrying ?obj - object)
    (handempty)
    ; This are concepts from the world at large
    ; they would be defined in the environment editor
    (on ?top ?bottom - object)
    (clear ?b - object)
    (on-floor ?b - object)
    ; Before picking, the robot also needs to be able to identify the object
    (detects ?obj - object)
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

  (:action detect
    :parameters (?obj - object ?r - room)
    :precondition
      (and
        (at ?r)
        (object-at ?obj ?r)
        (not (detects ?obj))
      )
    :effect
      (and
        (detects ?obj)
      )
  )

  (:action pick
    :parameters (?obj - object ?r - room)
    :precondition
      (and
        (at ?r)
        (object-at ?obj ?r)
        (on-floor ?obj)
        (clear ?obj)
        (detects ?obj)
        (handempty)
      )
    :effect
      (and
        (carrying ?obj)
        (not (object-at ?obj ?r))
        (not (on-floor ?obj))
        (not (handempty))
        (not (detects ?obj))
      )
  )

  ; Place puts the boxes directly on the floor
  ; The robot may also choose to stack directly
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
        (on-floor ?obj)
        (clear ?obj)
        (handempty)
      )
  )

  (:action stack
    :parameters (?top ?bottom - object ?r - room)
    :precondition
      (and
        ; Robot has object in the room
        (at ?r)
        (carrying ?top)
        ; Other object is also in the room, and there is nothing on it
        (object-at ?bottom ?r)
        (clear ?bottom)
        ; The objects are different
        (not (= ?top ?bottom))
      )
    :effect
      (and
        (on ?top ?bottom)
        (object-at ?top ?r)
        (not (carrying ?top))
        (not (clear ?bottom))
        (handempty)
      )
  )

  (:action unstack
    :parameters (?top ?bottom - object ?r - room)
    :precondition
      (and
        ; Robot is in the room and it is empty-handed
        (at ?r)
        (handempty)
        ; Both boxes are in the room and the top one is clear
        (object-at ?top ?r)
        (object-at ?bottom ?r)
        (on ?top ?bottom)
        (clear ?top)
        ; The objects are different
        (not (= ?top ?bottom))
      )
    :effect
      (and
        (not (on ?top ?bottom))
        (not (object-at ?top ?r))
        (clear ?bottom)
        (carrying ?top)
        (not (handempty))
      )
  )
)
