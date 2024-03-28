FROM ubuntu:22.04

LABEL org.opencontainers.image.source="https://github.com/xinu-os/xinu"
LABEL authors="Alex Gebhard"
LABEL maintainer="alexander.gebhard@marquette.edu"

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install -y autoconf automake autotools-dev curl python3 python3-pip libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev ninja-build git cmake libglib2.0-dev wget xz-utils

WORKDIR /opt/
RUN wget https://ftp.gnu.org/gnu/binutils/binutils-2.42.tar.gz
RUN tar xvzf binutils-2.42.tar.gz
WORKDIR /opt/binutils-2.42/
RUN ./configure --prefix=/opt/mipsel-dev --target=mipsel
RUN make
RUN make install
RUN mkdir -p /opt/mipsel-dev/mipsel/usr
RUN ln -s /usr/include /opt/mipsel-dev/mipsel/usr/include

WORKDIR /opt/
RUN git clone https://github.com/riscv-collab/riscv-gnu-toolchain /opt/riscv-gnu-toolchain --recurse-submodules
WORKDIR /opt/riscv-gnu-toolchain
RUN ./configure --prefix=/opt/riscv
RUN make linux

WORKDIR /opt/
RUN wget https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz
RUN tar xf arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz

RUN SNIPPET="export PROMPT_COMMAND='history -a' && export HISTFILE=/commandhistory/.bash_history" \
    && echo "$SNIPPET" >> "/root/.bashrc"

ENV PATH="${PATH}:/opt/riscv/bin/:/opt/mipsel-dev/bin/:/opt/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/bin"
CMD ["echo", "Xinu build docker image created!"]