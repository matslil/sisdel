FROM bitnami/minideb:buster

# Make image podman friendly, move nogroup to lower part of gid
RUN OLD_GID=$(getent group sys | cut -d: -f3) && \
    groupmod -g 200 nogroup && \
    find / -mount -group ${OLD_GID} -exec chgrp -h nogroup {} \;

# Install non-python build tools
RUN install_packages clang clang-format clang-tidy lldb llvm python3-minimal python3-venv python3-pip cmake ninja-build libboost-all-dev doxygen graphviz pkg-config libgmp-dev liblttng-ust-dev

# Setup virtual environment for Python
RUN python3 -m venv /venv

# Install python build tools
RUN . /venv/bin/activate && pip install --upgrade pip
RUN . /venv/bin/activate && pip install conan sphinx breathe

