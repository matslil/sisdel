FROM bitnami/minideb:buster

ADD https://github.com/Kitware/CMake/releases/download/v3.20.0/cmake-3.20.0-linux-x86_64.sh /

RUN chmod +x /cmake-3.20.0-linux-x86_64.sh && /cmake-3.20.0-linux-x86_64.sh --skip-license --prefix=/usr && rm /cmake-3.20.0-linux-x86_64.sh

# Make image podman friendly, move nogroup to lower part of gid
RUN OLD_GID=$(getent group sys | cut -d: -f3) && \
    groupmod -g 200 nogroup && \
    find / -mount -group ${OLD_GID} -exec chgrp -h nogroup {} \;

# Install non-python build tools
RUN install_packages clang clang-format clang-tidy lldb llvm python3-minimal python3-venv python3-pip ninja-build libboost-all-dev doxygen graphviz pkg-config libgmp-dev liblttng-ust-dev lcov git libmpfr-dev

RUN git clone https://github.com/catchorg/Catch2.git && \
    cd Catch2 && \
    cmake -G Ninja -Bbuild -H. -DBUILD_TESTING=OFF -DCMAKE_INSTALL_PREFIX=/usr && \
    cmake --build build/ --target install && \
    cd .. && \
    rm -rf Catch2

# Setup virtual environment for Python
RUN python3 -m venv /venv

# Install python build tools
RUN . /venv/bin/activate && pip install --upgrade pip
RUN . /venv/bin/activate && pip install conan sphinx breathe

