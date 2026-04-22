This is a simple single threaded Database created for learning purpose.

Basic Operations like creating schemas, table, and rows can be performed.

Data types supported are INT, STRING, FLOAT.

## Setting up Container

Dockerfile given can be used to set up a container.

Navigate to directory named db which already contains a pre-built binary, same
image is used to debug so re-build whenever necessary.

**Run following command to build and run binary:**

    # builds in build folder
    cmake -S . -B build
    cmake --build build

    # run
    build/db_scratch

## CMake

*add something if needed*



