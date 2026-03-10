# MAKI

<img align="right" src="./app/assets/app_icon/maki_icon.png" height="150">

[![Build dev Docker image](https://github.com/FelipeACXavier/maki/actions/workflows/build-dev-image.yaml/badge.svg)](https://github.com/FelipeACXavier/maki/actions/workflows/build-dev-image.yaml)
[![CI](https://github.com/FelipeACXavier/maki/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/FelipeACXavier/maki/actions/workflows/ci.yml)

MAKI is a low-code platform that targets robotic systems. It is build on top of [KODA](https://github.com/FelipeACXavier/KODA) and aims to simplify the creation of formally verified ROS2 systems. MAKI is still in development so expect breaking changes now and then. For more information, check out the [documentation](./docs) **(add ink)** and these papers **(add links)**.

## Installation

To build and install the application, follow the instructions below:

1. Clone this repository and move into it

```bash
git clone https://github.com/FelipeACXavier/maki.git && cd maki
```

2. Then clone the submodules:

```bash
git submodule update --init --recursive
```

3. After this step, we follow OS specific instructions:

  - [Linux](./docs/Building/build_linux.md)
  - [Windows](./docs/Building/build_windows.md)


## Examples:

  Some examples are available in the [./examples](./examples/) folder. These can be loaded directly into MAKI. Each example has a correlated generated KODA file for those interested in the textual DSL. More information can be found in the KODA [repository](https://github.com/FelipeACXavier/KODA).

## Styling ideas

### Colors

- Red Inferno: [\#bb2c1e](https://colorkit.co/color/bb2c1e/)
- Flame of Prometheus: [\#d73800](https://colorkit.co/color/d73800/)
- Freshly Baked: [\#e5c185](https://colorkit.co/color/e5c185/)
- Fondue: [\#fbf2c4](https://colorkit.co/color/fbf2c4/)
- Neptune Green: [\#7ebb9c](https://colorkit.co/color/7ebb9c/)
- Emperor Jade: [\#018373](https://colorkit.co/color/018373/)
- Windows 95 Desktop: [\#008585](https://colorkit.co/color/008585/)

### Fonts

To keep the same aesthetic across different operating systems, MAKI uses predefined fonts. All of these are open-source:

- [JetBrains Mono](https://www.jetbrains.com/lp/mono/)
- [Geist](https://fonts.google.com/specimen/Geist)
- [Can robots say I'm not a robot?](https://www.1001fonts.com/can-robots-say-i-m-not-a-robot-font.html)
