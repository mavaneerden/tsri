FROM ubuntu:22.04

ARG USER_ID=1000
ARG GROUP_ID=1000

ARG DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y build-essential git
RUN apt-get install -y unzip
# Codegen
RUN apt-get install -y python3-pip && python3 -m pip install -U cmsis-svd jinja2
# Formatting & checks
RUN apt-get install -y cppcheck wget lsb-release software-properties-common gnupg

# Create a new user
RUN addgroup --gid $GROUP_ID user

# Make the user able to do sudo without a password
RUN adduser --uid $USER_ID --gid $GROUP_ID --disabled-password --gecos '' user

#  Add new user docker to sudo group
RUN adduser user sudo

# Ensure sudo group users are not
# asked for a password when using
# sudo command by ammending sudoers file
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# Remove the sudo message
RUN touch /home/user/.sudo_as_admin_successful

# Change the working directory
WORKDIR /home/user

# Install latest version of cmake.
# This was required for modules, but it's good to use the latest version anyways.
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null
RUN echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ jammy main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null
RUN apt update
RUN apt install -y cmake

# Install newer version of clang-format & clangd
RUN wget https://apt.llvm.org/llvm.sh
RUN chmod +x llvm.sh
RUN ./llvm.sh 22
RUN apt install -y clang-format-22 clangd-22 clang-22
RUN rm llvm.sh

RUN ln -s $(which clang-22) /usr/bin/clang
RUN ln -s $(which clang++-22) /usr/bin/clang++
RUN ln -s $(which llvm-objcopy-22) /usr/bin/llvm-objcopy
RUN ln -s $(which llvm-objdump-22) /usr/bin/llvm-objdump

# Change the user
USER user
