# Use an older Ubuntu that has better support for 32-bit legacy tools
FROM i386/ubuntu:18.04

# Install build essentials and dependencies for the lab
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc-multilib \
    make \
    python \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory inside the container
WORKDIR /lab

# When the container starts, drop into a bash shell
CMD ["/bin/bash"]