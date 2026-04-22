FROM ubuntu:22.04

RUN apt update && apt install -y build-essential cmake gdb \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /db
COPY . .

RUN cmake -S . -B build && cmake --build build

CMD ["bash"]
