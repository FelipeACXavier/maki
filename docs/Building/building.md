@page building Building MAKI

To build and install the application, follow the instructions below:

<ol start="1"> <li>Clone this repository and move into it.</li></ol>

```bash
git clone https://github.com/FelipeACXavier/maki.git && cd maki
```
<ol start="2"> <li>Then clone the submodules:</li></ol>

```bash
git submodule update --init --recursive
```
<ol start="3"> <li>After this step, we follow OS specific instructions:</li></ol>

  - @subpage building_windows
  - @subpage building_linux