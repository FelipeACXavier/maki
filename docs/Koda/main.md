@page koda KODA

# KODA

KODA is a domain-specific language (DSL) for modelling the structure, behaviour, and
data of robotic systems. It forms the underlying modelling language of MAKI and
provides a representation that is independent of a particular execution or
verification technology.

A KODA model describes a robotic application in terms of reusable
**capabilities**, **tasks**, **data**, and **behavioural flows**. These models can
then be processed by different backends to generate executable implementations,
verification models, or other artefacts.

MAKI provides a visual environment for creating and working with these models,
while KODA provides their textual representation and compiler infrastructure.

# Core concepts

A KODA application is built from a small number of concepts:

- **Data** describe the information exchanged between components.
- **Capabilities** describe functionality provided by the robotic system.
- **Tasks** compose capabilities into application-level behaviours.
- **Flows** describe how operations are orchestrated.
- **Properties** express requirements that can be checked by verification
  backends.

Together, these concepts allow the structure, behaviour, and data of a robotic
application to be represented in a single model.

# Capabilities

Capabilities describe functionality that can be provided by a robot or one of
its components.

For example, a navigation capability may expose an operation for moving to a
position:

@code{.koda}
capability Drive(float x, float y) {
  action "/navigate_to_pose" "nav2_msgs::action::NavigateToPose::Goal" {
    trigger: void to_position(float x, float y);
    abort: void cancel();
    return: void in_position(float x, float y);
    error: void path_blocked();
  }
}
@endcode

Capabilities describe the interface expected by a task rather than a specific
robot implementation. This allows tasks to be reused with different systems
that provide compatible capabilities.

See @subpage koda_capabilities for more information.


# Tasks

Tasks describe application-level behaviour by combining capabilities.

A task declares the capabilities it requires together with its parameters,
events, local data, and behavioural strategy.

For example:

@code{.koda}
task PickAndPlace(
    drive req Drive,
    vision req Vision,
    grip req Grip)
{
  trigger: void start();
  return: void done();
  abort: void abort();
  error: void failed();

  strategy {
    main:
      drive("pick_location") -->
      vision() -->
      grip(true) -->
      drive("place_location") -->
      grip(false);
  }
}
@endcode

The task is independent of the concrete implementations of `Drive`, `Vision`,
and `Grip`. Those dependencies can be bound to components of the target robotic
system.

See @subpage koda_tasks for more information.


# Flows and strategies

The behaviour of a task is defined using **flows**.

Flows compose capability calls and other flows using orchestration constructs.
A simple sequence uses the `-->` operator:

@code{.koda}
main:
  drive("pick_location") -->
  vision() -->
  grip(true);
@endcode

More complex behaviour can be constructed using reusable flows and control-flow
constructs such as:

- sequential execution;
- parallel execution and synchronization;
- repetition;
- time-bounded execution;
- conditional behaviour;
- error and abort handling.

For example, a reusable flow can isolate part of a larger behaviour:

@code{.koda}
pick:
  vision() -->
  grip(true);

main:
  drive("pick_location") -->
  pick -->
  drive("place_location") -->
  grip(false);
@endcode

This keeps task behaviour compositional while allowing exceptional behaviour
such as errors and aborts to be handled explicitly.

See @subpage koda_flows for more information.


# Types and data

KODA models can define and exchange structured data independently of the
underlying middleware representation.

The type system contains built-in types and allows project-specific types to be
defined and reused throughout a model. Types can be used by task parameters,
capabilities, events, and shared data.

This allows behavioural orchestration to depend on explicitly modelled data
rather than on backend-specific messages or implementation details.

See @subpage koda_types for more information.


# Properties and verification

KODA properties describe requirements over observable system behaviour.

For example:

@code{.koda}
properties {
  no_parallel_motion:
    never drive is running and grip is running;

  handle_rejection:
    if drive.to_position was rejected
    eventually fstop started;
}
@endcode

Properties are expressed independently of a particular model checker. A
verification backend can translate them into the formalism required by its
target technology.

For example, a backend targeting nuXmv can translate supported properties into
LTL while generating the corresponding state-machine model from the same KODA
representation.

See @subpage koda_properties for the complete property notation.


# Code generation

KODA is designed to separate application models from the technologies used to
execute or analyse them.

The compiler transforms KODA source models into an intermediate representation
that can be consumed by different backend plugins:

@code
                    +--> execution backend
                    |
KODA --> Compiler --> IR +--> verification backend
                    |
                    +--> other generators
@endcode

This allows the same KODA model to support multiple workflows. An execution
backend can generate an implementation for a robotic system, while a
verification backend can generate a formal model for analysing properties of
the same behaviour.

See @subpage koda_codegen for more information.


# KODA and MAKI

MAKI and KODA serve complementary roles.

**KODA** defines the modelling concepts, textual language, compiler, and
intermediate representation.

**MAKI** provides the graphical development environment used to construct and
work with KODA models.

A model created visually in MAKI therefore has the same underlying concepts as
a textual KODA model. This makes it possible to combine low-code modelling with
textual editing, static analysis, code generation, and formal verification.

# Documentation

The KODA documentation is divided into the following sections:

- @subpage koda_types
- @subpage koda_capabilities
- @subpage koda_tasks
- @subpage koda_flows
- @subpage koda_properties
- @subpage koda_codegen