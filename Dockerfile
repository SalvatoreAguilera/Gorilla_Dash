# Start with an appropriate base image
FROM ubuntu:20.04

# Set the maintainer label
LABEL maintainer="your_email@example.com"

# Update and install essential dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    wget \
    git \
    gcc \
    clang \
    g++ \
    libegl1-mesa \
    libgl1-mesa-dev \
    libglu1-mesa \
    libglu1-mesa-dev \
    libsfml-dev \  
    libgtest-dev \
    binutils-dev \
    libtool \
    lua5.2 \
    liblua5.2-dev \
    liblua5.2-0 \
    graphviz \
    doxygen \
    && rm -rf /var/lib/apt/lists/*

# Optionally, install CMake (if you need a specific version)
RUN wget http://www.cmake.org/files/v3.6/cmake-3.6.2.tar.gz
RUN tar -xvf cmake-3.6.2.tar.gz
RUN cd cmake-3.6.2 && \
    ./bootstrap && \
    make && \
    make install

# Set the working directory to where the project is located
WORKDIR /app

# Copy only the server.cpp file to the container
COPY server.cpp .

# Compile server.cpp using g++ with SFML libraries
RUN g++ server.cpp -Wall -lsfml-network -lsfml-system -o server

# Expose any necessary ports for networking (if your game is multiplayer)
EXPOSE 8080

# Start the application
CMD ["./server"]
