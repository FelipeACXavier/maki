@page building Building MAKI

To build and install the application, follow the instructions below:

> [!IMPORTANT]
> MAKI does not currently build or install ROS packages outside the docker image.
> If you are running MAKI outside the docker container, you need to setup ROS yourself.

<ol start="1"> <li>Clone this repository and move into it.</li>

```bash
git clone https://github.com/FelipeACXavier/maki.git && cd maki
```
</ol>

<ol start="2"> <li>Then clone the submodules:</li>

```bash
git submodule update --init --recursive
```
</ol>

<ol start="3"> <li>After this step, we follow OS specific instructions:</li>
  - @subpage building_windows
  - @subpage building_linux
</ol>