# Document:
# https://github.com/picoruby/prk_firmware/wiki/Docker

FROM --platform=amd64 ruby:3.1.2-slim AS build

RUN apt-get update && apt-get install --no-install-recommends -y \
  cmake \
  gcc-arm-none-eabi \
  libnewlib-arm-none-eabi \
  libstdc++-arm-none-eabi-newlib \
  ca-certificates \
  git \
  gcc \
  make \
  g++ \
  zip \
  python3

RUN git clone https://github.com/raspberrypi/pico-sdk.git
RUN cd /pico-sdk/lib && git submodule update --init ./
ENV PICO_SDK_PATH "/pico-sdk"

ENV BUILD_HOME /check_hid
VOLUME "${BUILD_HOME}"
WORKDIR "${BUILD_HOME}/build"
COPY . .

