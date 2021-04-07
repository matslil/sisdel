FROM bitnami/minideb:buster

ARG buildroot

ADD https://github.com/Kitware/CMake/releases/download/v3.20.0/cmake-3.20.0-linux-x86_64.sh /

RUN chmod +x /cmake-3.20.0-linux-x86_64.sh && /cmake-3.20.0-linux-x86_64.sh --skip-license --prefix=/usr && rm /cmake-3.20.0-linux-x86_64.sh

# Make image podman friendly, move nogroup to lower part of gid
RUN OLD_GID=$(getent group sys | cut -d: -f3) && \
    groupmod -g 200 nogroup && \
    find / -mount -group ${OLD_GID} -exec chgrp -h nogroup {} \;

# Install non-python build tools
RUN install_packages python3-minimal python3-venv python3-pip doxygen graphviz lsb-release wget gnupg software-properties-common make ninja-build coreutils lcov rpm

# Install llvm/Clang
ADD https://apt.llvm.org/llvm.sh /tmp/
RUN chmod +x /tmp/llvm.sh
RUN /tmp/llvm.sh 12
RUN rm /tmp/llvm.sh
RUN for bin in $( ls /usr/bin/llvm*-12 /usr/bin/clang*-12 ); do update-alternatives --install "${bin%-12}" $(basename "${bin%-12}") "${bin}" 200; done

# Setup virtual environment for Python
RUN python3 -m venv /venv

# Install python build tools
RUN . /venv/bin/activate && pip install --upgrade pip
RUN . /venv/bin/activate && pip install conan sphinx breathe sphinx-rtd-theme exhale

ENV CONAN_USER_HOME=/conan
ENV CC=/usr/bin/clang
ENV CXX=/usr/bin/clang++

COPY conanfile.txt profile-Linux.txt /conan/

RUN . /venv/bin/activate && cd "${CONAN_USER_HOME}" && conan install --build missing --profile /conan/profile-Linux.txt /conan/conanfile.txt && conan remove --builds --src --force '*'

