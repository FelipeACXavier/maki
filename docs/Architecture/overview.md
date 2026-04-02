@page architecture_overview Architecture

# Use cases

The use cases capture the main interactions supported by the platform from the perspective of its users and external stakeholders. They provide a high-level view of the expected behaviour of the system, helping define its scope and the value it offers in practice. By identifying the core tasks and workflows, this view serves as the starting point for both the functional and architectural design.

\image html ./Figures/use_cases.webp width=75%

<hr>

# Functional decomposition

The functional decomposition breaks the platform down into its main responsibilities and logical subsystems. Rather than focusing on implementation details, it highlights which core functions the system must provide and how these functions relate to one another. This view is useful for reasoning about separation of concerns, maintainability, and the overall internal organisation of the platform.

\image html ./Figures/decomposition.webp width=75%

<hr>

# Reference architecture

The reference architecture presents the abstract architectural structure that guides the design of the platform. It identifies the main layers, components, and relationships that are expected to remain stable across different implementations or application scenarios. In this sense, it acts as a reusable blueprint that connects the system requirements to a more general architectural vision.

MAKI follows a modular architecture and is broken into three layers:

<!-- \image html ./Figures/reference_architecture.webp width=50% -->

<table border="0" cellspacing="0" cellpadding="0">
<tr>
<td width="50%" valign="center">
<h2>View layer</h2>
<p>
This layer is responsible for rendering information and capturing user interaction.
It contains the visual editor surfaces and panels that present the platform document and allow users to manipulate it.
</p>
<p><b>Typical responsibilities:</b></p>
<ul>
  <li>Visualisation of the orchestration</li>
  <li>User interaction handling</li>
  <li>Emitting user intents</li>
  <li>Applying user preferences</li>
</ul>
<h2>Application layer</h2>
<p>
This layer coordinates editor behaviour and workflows.
It interprets user intents, applies edits to the document, and executes higher-level use cases.
</p>
<p><b>Typical responsibilities:</b></p>
<ul>
  <li>Command handling</li>
  <li>Coordination of views and document updates</li>
  <li>Project loading/saving</li>
  <li>Execution of workflows</li>
  <li>Plugin management and interaction coordination</li>
</ul>
<h2>Document layer</h2>
<p>
This layer contains the core orchestration knowledge and is the single source of truth.
It can be viewed as a data layer.
</p>
<p><b>Typical responsibilities:</b></p>
<ul>
  <li>Orchestration graph structure</li>
  <li>Capability definitions</li>
  <li>Invariants and validation rules</li>
  <li>Project-level semantic configuration</li>
</ul>
</td>
<td width="50%" valign="center">
\image html ./Figures/reference_architecture.webp width=100%
</td>
</tr>
</table>

# Solution architecture
<hr>

The solution architecture shows how the reference architecture is realised in the concrete implementation of the platform. It maps the abstract architectural concepts onto specific modules, technologies, and runtime elements, making the design actionable from an engineering perspective. This view helps clarify how the system is actually built and how its different parts collaborate in practice.

\image html ./Figures/solution_architecture.webp width=75%