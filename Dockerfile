# Base image with GCC and necessary tools
FROM gcc:latest

# Install SFML dependencies
RUN apt-get update && apt-get install -y \
    libsfml-dev \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory inside the container
WORKDIR /app

# Copy the source code to the container
COPY server.cpp .

# Compile the C++ program and link SFML libraries
RUN g++ -o server server.cpp -lsfml-network -lsfml-system

# Expose any necessary ports (update based on your server's needs)
EXPOSE 8080

# Command to run the compiled server
CMD ["./server"]
