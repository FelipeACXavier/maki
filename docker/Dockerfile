FROM ubuntu:24.04 AS builder

# -----------------------------------------------------------------------------------------------------------
# Arguments
ARG QT_VERSION=6.8.3
ARG QT_BUILD_TYPE=debug
ARG NODE_VERSION=23.8.0
ARG NVM_VERSION=v0.40.2

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
  g++-13 \
  mingw-w64 \
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
WORKDIR /home/ubuntu

# Let's install node, just in case...
RUN wget -O - https://raw.githubusercontent.com/nvm-sh/nvm/${NVM_VERSION}/install.sh | bash
ENV NVM_DIR=/home/ubuntu/.nvm
RUN bash -c "source $NVM_DIR/nvm.sh && nvm install $NODE_VERSION"

# -----------------------------------------------------------------------------------------------------------
# We now need to copy and build QT
# Download QT 6.8
WORKDIR /tmp
RUN wget https://download.qt.io/official_releases/qt/6.8/6.8.3/single/qt-everywhere-src-${QT_VERSION}.tar.xz

# Extract the archive
RUN tar xf qt-everywhere-src-${QT_VERSION}.tar.xz

# Build QT for linux
RUN mkdir -p ~/qt-linux
WORKDIR /home/ubuntu/qt-linux

RUN /tmp/qt-everywhere-src-${QT_VERSION}/configure \
    -prefix /home/ubuntu/Qt6-Linux \
    -${QT_BUILD_TYPE} \
    -nomake tests \
    -nomake examples \
    -skip qt3d \
    -skip qtcharts \
    -skip qtconnectivity \
    -skip qtdatavis3d \
    -skip qtdoc \
    -skip qtgamepad \
    -skip qtgraphicaleffects \
    -skip qtlocation \
    -skip qtmultimedia \
    -skip qtnetworkauth \
    -skip qtopcua \
    -skip qtpositioning \
    -skip qtdeclarative \
    -skip qtquick3d \
    -skip qtquick3dphysics \
    -skip qtquicktimeline \
    -skip qtquickeffectmaker \
    -skip qtlottie \
    -skip qtmqtt \
    -skip qtremoteobjects \
    -skip qtscxml \
    -skip qtsensors \
    -skip qtserialbus \
    -skip qtserialport \
    -skip qtspeech \
    -skip qtvirtualkeyboard \
    -skip qtwayland \
    -skip qtwebchannel \
    -skip qtwebengine \
    -skip qtwebsockets \
    -skip qtwebview \
    -skip qtgraphs \
    -skip qtgrpc \
    -qt-freetype \
    -qt-harfbuzz \
    -qt-libpng

RUN cmake --build . --parallel 4
RUN cmake --install .

# Build QT for windows
RUN mkdir -p ~/qt-windows
WORKDIR /home/ubuntu/qt-windows
COPY cmake/toolchain-mingw64.cmake  ./
RUN /tmp/qt-everywhere-src-${QT_VERSION}/configure \
    -prefix /home/ubuntu/Qt6-Windows \
    -${QT_BUILD_TYPE} \
    -nomake tests \
    -nomake examples \
    -skip qt3d \
    -skip qtcharts \
    -skip qtconnectivity \
    -skip qtdatavis3d \
    -skip qtdoc \
    -skip qtgamepad \
    -skip qtgraphicaleffects \
    -skip qtlocation \
    -skip qtmultimedia \
    -skip qtnetworkauth \
    -skip qtopcua \
    -skip qtpositioning \
    -skip qtdeclarative \
    -skip qtquick3d \
    -skip qtquick3dphysics \
    -skip qtquicktimeline \
    -skip qtquickeffectmaker \
    -skip qtlottie \
    -skip qtmqtt \
    -skip qtremoteobjects \
    -skip qtscxml \
    -skip qtsensors \
    -skip qtserialbus \
    -skip qtserialport \
    -skip qtspeech \
    -skip qtvirtualkeyboard \
    -skip qtwayland \
    -skip qtwebchannel \
    -skip qtwebengine \
    -skip qtwebsockets \
    -skip qtwebview \
    -skip qtgraphs \
    -skip qtgrpc \
    -qt-freetype \
    -qt-harfbuzz \
    -qt-libpng \
    -D CMAKE_TOOLCHAIN_FILE=toolchain-mingw64.cmake

RUN cmake --build . --parallel 4
RUN cmake --install .

# -----------------------------------------------------------------------------------------------------------
# Cd to the maki directory and end
WORKDIR /home/ubuntu/MAKI
CMD ["bash"]
