@page koda_properties KODA Property Definitions

# Overview

KODA property definitions allow temporal requirements to be specified directly
alongside a task. Properties use a small controlled-language notation designed
to describe the observable behaviour of KODA models without exposing the
notation of a particular verification backend.

For example:

<code>
properties {
    no_parallel_motion:
        never drive is running and grip is running;

    handle_rejection:
        if drive.to_position was rejected
        eventually fstop started;

    charging_terminates:
        if batterymonitoring.charge started
        eventually batterymonitoring.charge stopped;
}
</code>

Property definitions are backend independent. A verification backend may
translate them to an appropriate formalism, such as LTL for nuXmv or a
fixed-point logic for another verification backend.


# Observations

Properties are defined over observations of flows, capabilities, and capability
events.

The following observations are available:

<table>
<tr>
  <th>Notation</th>
  <th>Meaning</th>
</tr>
<tr>
  <td><tt>x is running</tt></td>
  <td>x is currently executing.</td>
</tr>
<tr>
  <td><tt>x started</tt></td>
  <td>The trigger of x was accepted successfully.</td>
</tr>
<tr>
  <td><tt>x was rejected</tt></td>
  <td>The trigger of x was rejected or failed to start.</td>
</tr>
<tr>
  <td><tt>x stopped</tt></td>
  <td>The execution of x terminated.</td>
</tr>
<tr>
  <td><tt>x succeeded</tt></td>
  <td>The running execution of x completed successfully.</td>
</tr>
<tr>
  <td><tt>x failed</tt></td>
  <td>The running execution of x completed with failure.</td>
</tr>
<tr>
  <td><tt>x was aborted</tt></td>
  <td>The execution of x was successfully aborted.</td>
</tr>
</table>

Not every observation is valid for every KODA entity. For example, trigger
rejection is meaningful for a capability event but not for a capability as a
whole. KODA checks observation compatibility during semantic analysis.


# Logical and temporal operators

Property expressions can be combined using logical and temporal operators.

<table>
<tr>
  <th>KODA</th>
  <th>Meaning</th>
</tr>
<tr>
  <td><tt>not P</tt></td>
  <td>P does not hold.</td>
</tr>
<tr>
  <td><tt>P and Q</tt></td>
  <td>P and Q both hold.</td>
</tr>
<tr>
  <td><tt>P or Q</tt></td>
  <td>P or Q holds.</td>
</tr>
<tr>
  <td><tt>P implies Q</tt></td>
  <td>If P holds, Q also holds.</td>
</tr>
<tr>
  <td><tt>always P</tt></td>
  <td>P must hold throughout execution.</td>
</tr>
<tr>
  <td><tt>never P</tt></td>
  <td>P must never hold.</td>
</tr>
<tr>
  <td><tt>eventually P</tt></td>
  <td>P must eventually hold.</td>
</tr>
<tr>
  <td><tt>next P</tt></td>
  <td>P must hold at the next observable KODA step.</td>
</tr>
<tr>
  <td><tt>P until Q</tt></td>
  <td>P must continue to hold until Q occurs.</td>
</tr>
</table>


# Property helpers

KODA provides additional constructs for common temporal requirements.

## If

The <tt>if</tt> construct specifies a requirement that must hold whenever an
observation occurs.

@code{.koda}
if drive.to_position was rejected
eventually fstop started
@endcode

This means that whenever <tt>drive.to_position</tt> is rejected,
<tt>fstop</tt> must eventually start.

For an LTL backend, this corresponds to:

@code
G(rejected(drive.to_position) -> F(started(fstop)))
@endcode


## Between

The <tt>between</tt> construct specifies a property that must hold after one
observation and until another observation occurs.

@code{.koda}
between drive.to_position started
and grip.handle started
not batterymonitoring.charge is running
@endcode

This requires charging to remain inactive after
<tt>drive.to_position</tt> starts and until <tt>grip.handle</tt> starts.

The terminating observation is required to occur. In an LTL backend, the
construct therefore uses strong until:

@code
G(started(drive.to_position) ->
  (!running(batterymonitoring.charge) U started(grip.handle)))
@endcode

Helpers can be nested. For example:

@code{.koda}
if fdrive_loop started
between drive.to_position started
and grip.handle started
not batterymonitoring.charge is running
@endcode


# Example

A complete property block may look as follows:

@code{.koda}
properties {
    no_grip_and_drive:
        never drive is running and grip is running;

    handle_drive_rejection:
        if drive.to_position was rejected
        eventually fstop started;

    charging_terminates:
        if batterymonitoring.charge started
        eventually batterymonitoring.charge stopped;

    abort_stops_drive:
        if fdrive_loop was aborted
        next not drive is running;

    no_charging_during_pickup:
        between drive.to_position started
        and grip.handle started
        not batterymonitoring.charge is running;
}
@endcode


# Verification

KODA properties describe model behaviour rather than the syntax of a specific
model checker. The KODA compiler resolves property references and validates
their observations before they are passed to a verification backend.

A backend is responsible for translating the resulting property representation
to the formalism it supports. For example, the nuXmv backend can translate
KODA temporal properties to LTL.

This separation allows the same KODA property notation to be used with
different verification backends while preserving the meaning of observations
defined by KODA.
