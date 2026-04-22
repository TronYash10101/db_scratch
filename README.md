This is a simple single-threaded database built for learning purposes.

It supports basic operations such as:
- Creating schemas
- Creating tables
- Inserting rows

Supported data types:
- INT
- STRING
- FLOAT

---

## Build and Run (Local)

Build the project using CMake:

    cmake -S . -B build
    cmake --build build

Run the database:

    ./build/db_scratch

---

## Using Docker

A Dockerfile is provided to build and run the project inside a container.

### Build the image

    docker build -t db_scratch .

### Run the container (interactive)

    docker run -it db_scratch

Inside the container:

    ./build/db_scratch

---

## Notes

- The Docker image includes build tools and is intended for both development and debugging.
- If you modify the source code, rebuild the image:

    docker build -t db_scratch .

---

## CMake

Uses a basic CMake setup for building the project.
