@mainpage MAKI

<img align="right" src="./maki_icon_only.png" height="150">

[![Build dev Docker image](https://github.com/FelipeACXavier/maki/actions/workflows/build-dev-image.yaml/badge.svg)](https://github.com/FelipeACXavier/maki/actions/workflows/build-dev-image.yaml)
[![CI](https://github.com/FelipeACXavier/maki/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/FelipeACXavier/maki/actions/workflows/ci.yml)

MAKI is a low-code platform that targets robotic systems. It is build on top of [KODA](https://github.com/FelipeACXavier/KODA) and aims to simplify the creation of formally verified ROS2 systems. MAKI is still in development so expect breaking changes now and then. For more information, check out the [documentation](https://felipeacxavier.github.io/maki/) and these papers:

- <a href="https://doi.org/10.1145/3786179.3788321" target="_blank">Verification-Centered Low-Code for Autonomous Robots: A Contract-Based Reference Architecture Approach</a>
- <a href="https://doi.org/10.1145/3786159.3788475" target="_blank">From Robotic to IoT Systems: Exploring the Reuse of a Robotic Orchestration DSL in the IoT domain</a>

## Overview

MAKI is a Qt-based low-code environment for composing robotic systems using visual pipelines and DSL-based generators.

The platform focuses on:

- Plugin-based code generators
- ROS integration
- Behaviour tree orchestration
- Simulation and verification pipelines

\image html ./Figures/top_level_architecture.png width=75%

## Documentation

- @subpage architecture_overview
- @subpage building
- @subpage customization
- @subpage examples