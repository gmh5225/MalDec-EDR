FROM debian:stable

RUN apt update && \
    apt install -y \
        g++ \
        cmake \
        libyara-dev \
        binutils \
        libsqlite3-dev \
        libsystemd-dev \
        
COPY . /app

WORKDIR /app

RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    make
