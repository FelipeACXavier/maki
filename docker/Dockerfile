FROM ubuntu:24.04 AS builder

# -----------------------------------------------------------------------------------------------------------
# Arguments
ARG QT_VERSION=6.8.3
ARG QT_BUILD_TYPE=debug
ARG NODE_VERSION=23.8.0
ARG NVM_VERSION=v0.40.2
ARG HOME=/home/ubuntu
ARG TOOLCHAIN_FILE=toolchain-mingw64.cmake

# -----------------------------------------------------------------------------------------------------------
# Essential packages
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
  build-essential \
  cmake \
  git \
  ca-certificates \
  wget \
  ninja-build \
  python3.12 \
  python3-pip \
  python3-html5lib \
  python3.12-venv \
  g++-13 \
  # mingw-w64 \
  ccache \
  libasio-dev \
  libgl1-mesa-dri \
  libgl1-mesa-dev \
  libx11-dev libx11-xcb-dev libxrender-dev libxi-dev \
  libxcb1 libx11-xcb1 '^libxcb.*-dev' \
  libxcb-cursor0 \
  libxkbcommon-x11-0 libxkbcommon-dev libxkbcommon-x11-dev \
  pkg-config \
  locales \
  language-pack-en-base language-pack-en \
  language-pack-nl-base language-pack-nl \
  fontconfig \
  fonts-dejavu-core \
  libfreetype6-dev libfontconfig1-dev libharfbuzz-dev \
  && rm -rf /var/lib/apt/lists/*

# -----------------------------------------------------------------------------------------------------------
# Niceties
# Make sure the fonts are loaded
RUN fc-cache -f

# Set the locale
RUN git clone https://github.com/PanderMusubi/locale-en-nl.git
RUN cd ./locale-en-nl && ./add.sh

ENV LC_ALL=en_NL.UTF-8
ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US.UTF-8

# Make sure we are signed in as ubuntu
USER ubuntu
WORKDIR ${HOME}

# Let's install node, just in case...
RUN wget -O - https://raw.githubusercontent.com/nvm-sh/nvm/${NVM_VERSION}/install.sh | bash
ENV NVM_DIR=${HOME}/.nvm
RUN bash -c "source $NVM_DIR/nvm.sh && nvm install $NODE_VERSION"

# -----------------------------------------------------------------------------------------------------------
# We now need to install qt
USER ubuntu
RUN python3 -m venv env
RUN ${HOME}/env/bin/pip install --no-cache-dir aqtinstall

RUN mkdir -p ${HOME}/Qt && \
    ${HOME}/env/bin/python3 -m aqt install-qt \
      linux \
      desktop \
      "${QT_VERSION}" \
      linux_gcc_64 \
      -m qtwebengine qtpdf qtwebchannel qtpositioning \
      -O ${HOME}/Qt

# -----------------------------------------------------------------------------------------------------------\
# Install extra packages
USER root
RUN apt-get update && apt-get install -y --no-install-recommends \
  zip \
  unzip \
  gdb \
  bison \
  flex \
  gperf \
  libgl1-mesa-dev \
  libgles2-mesa-dev \
  mesa-common-dev \
  libnss3-dev \
  libglib2.0-dev \
  libharfbuzz-dev \
  libudev-dev \
  libpng-dev \
  libtiff-dev \
  libicu-dev \
  libopus-dev \
  libvpx-dev \
  libavutil-dev \
  libavcodec-dev \
  libavformat-dev \
  libswresample-dev \
  libswscale-dev \
  libre2-dev \
  libopenh264-dev \
  zlib1g-dev \
  libssl-dev \
  libdbus-1-dev \
  libxcomposite-dev \
  libxdamage-dev \
  libxrandr-dev \
  libxtst-dev \
  libasound2-dev \
  libxkbfile-dev \
  && rm -rf /var/lib/apt/lists/*

# Install dezyne
WORKDIR ${HOME}
USER ubuntu
COPY scripts/install_dezyne.sh ${HOME}/
RUN ${HOME}/install_dezyne.sh

# -----------------------------------------------------------------------------------------------------------
# Cd to the maki directory and end
WORKDIR ${HOME}/maki
CMD ["bash"]
