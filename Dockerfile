FROM debian:bookworm-slim

# Install dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    gcc \
    libc6-dev \
    ffmpeg && \
    rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code and input parameters
COPY ising2d.c .
COPY input .

# Compile simulation
RUN gcc -O3 -std=gnu11 ising2d.c -o ising -lm

# Run simulation
CMD ["./ising"]
