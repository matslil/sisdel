FROM bitnami/minideb:buster

ARG LLVMVER=12

ADD https://github.com/Kitware/CMake/releases/download/v3.20.0/cmake-3.20.0-linux-x86_64.sh /

RUN chmod +x /cmake-3.20.0-linux-x86_64.sh && /cmake-3.20.0-linux-x86_64.sh --skip-license --prefix=/usr/local && rm /cmake-3.20.0-linux-x86_64.sh

# Make image podman friendly, move nogroup to lower part of gid
RUN OLD_GID=$(getent group sys | cut -d: -f3) && \
    groupmod -g 200 nogroup && \
    find / -mount -group ${OLD_GID} -exec chgrp -h nogroup {} \;

# Install non-python build tools
RUN install_packages python3-minimal python3-venv python3-pip doxygen graphviz lsb-release wget gnupg software-properties-common make ninja-build coreutils lcov rpm openjdk-11-jre-headless curl

# Install PlantUML
RUN curl --location --output /usr/local/bin/plantuml.jar http://sourceforge.net/projects/plantuml/files/plantuml.jar/download
RUN printf '%s\n%s' '#!/usr/bin/env -S sh -e' 'java -Djava.awt.headless=true -jar /usr/local/bin/plantuml.jar -tsvg -failfast2 "$@"' > /usr/local/bin/plantuml
RUN chmod +x /usr/local/bin/plantuml

# Install llvm/Clang
RUN echo "deb http://apt.llvm.org/$(lsb_release -cs)/ llvm-toolchain-$(lsb_release -cs)-${LLVMVER} main" > /etc/apt/sources.list.d/llvm.list
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
RUN apt-get update
RUN apt-get install -y libllvm-${LLVMVER}-ocaml-dev libllvm12 llvm-${LLVMVER} llvm-${LLVMVER}-dev llvm-${LLVMVER}-runtime clang-${LLVMVER} clang-tools-${LLVMVER} libclang-common-${LLVMVER}-dev libclang-${LLVMVER}-dev libclang1-${LLVMVER} clang-format-${LLVMVER} libfuzzer-${LLVMVER}-dev lldb-${LLVMVER} lld-${LLVMVER} libc++-${LLVMVER}-dev libc++abi-${LLVMVER}-dev python3-clang-${LLVMVER} clangd-${LLVMVER} && apt-get clean

# Setup virtual environment for Python
RUN python3 -m venv /venv

# Install python build tools
COPY requirements.txt /tmp
RUN . /venv/bin/activate && pip install --upgrade pip
RUN . /venv/bin/activate && pip install -r /tmp/requirements.txt
RUN rm /tmp/requirements.txt
COPY entrypoint /entrypoint

# Install gitversion
ENV DOTNET_ROOT="/usr/local/bin"
ADD https://dot.net/v1/dotnet-install.sh /tmp
RUN chmod +x /tmp/dotnet-install.sh
RUN /tmp/dotnet-install.sh --install-dir "${DOTNET_ROOT}"
RUN rm /tmp/dotnet-install.sh
RUN "${DOTNET_ROOT}/dotnet" tool install --tool-path "${DOTNET_ROOT}" GitVersion.Tool

# Install Conan libraries
ENV CONAN_USER_HOME=/usr/cmake
ENV CC=/usr/bin/clang-${LLVMVER}
ENV CXX=/usr/bin/clang++-${LLVMVER}
COPY conanfile.txt profile-Linux.txt /conan/
RUN mkdir -p "${CONAN_USER_HOME}"
RUN . /venv/bin/activate && cd "${CONAN_USER_HOME}" && conan install --build missing --profile /conan/profile-Linux.txt /conan/conanfile.txt && conan remove --builds --src --force '*'

# Set entrypoint
ENTRYPOINT ["/entrypoint"]

