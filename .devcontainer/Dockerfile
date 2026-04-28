FROM ubuntu:25.10

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    gcc-15 \
    g++-15 \
    cmake \
    make \
    gdb \
    git \
    ca-certificates \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Ustaw gcc-15/g++-15 jako domyślny kompilator
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-15 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-15 100 \
    && update-alternatives --install /usr/bin/cc  cc  /usr/bin/gcc-15 100 \
    && update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-15 100

WORKDIR /workspace
