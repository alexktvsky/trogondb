# Build stage
FROM debian:bookworm AS builder

RUN apt update && apt install -y \
    clang \
    cmake \
    git \
    python3 \
    python3-pip \
    python3-venv \
    build-essential \
    curl

RUN python3 -m pip install conan --break-system-package

ENV CC=clang
ENV CXX=clang++

WORKDIR /app

COPY . .

RUN mkdir -p build

WORKDIR /app/build

RUN conan profile detect --force && \
    conan install .. --profile:all ../conan/profiles/linux.x86_64.clang.release --build=missing

RUN cmake .. \
        -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_COMPILER=clang \
        -DCMAKE_CXX_COMPILER=clang++ && \
    cmake --build . --config Release --parallel


# Runtime stage
FROM debian:bookworm-slim

RUN apt update && apt install -y \
    libstdc++6 \
    && apt clean && rm -rf /var/lib/apt/lists/*

RUN groupadd nobody && usermod -aG nobody nobody

WORKDIR /app

COPY --from=builder /app/build/bin/trogondb ./trogondb
COPY --from=builder /app/trogondb.yml ./trogondb.yml

USER nobody

EXPOSE 6379

CMD ["./trogondb", "--config", "trogondb.yml"]
