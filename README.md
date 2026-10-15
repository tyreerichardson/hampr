# Hampr

**Hampr** is a lightweight, C++17-based command-line task runner designed to simplify executing and managing project-specific commands.

The goal is to build a minimal, local alternative to more complex build automation tools while exploring systems programming concepts such as inter-process communication (IPC), process management, task queues, and daemon architecture.

## Features

- Define project-specific commands using a `config.hampr` file.
- Execute tasks through a simple command-line interface.
- Communicate between the CLI and a separate daemon using Unix domain sockets.
- Resolve task configurations based on the project's directory.
- Validate configuration entries and handle errors.

## How It Works

Each project contains a `config.hampr` file defining its available tasks:

```ini
build=cmake --build build
test=ctest --test-dir build
run=./build/my-app
```

Run a task from the project's directory:

```bash
hampr build
```

Hampr uses two components:

- **`hampr`** — The CLI that submits task requests.
- **`hamprd`** — The daemon responsible for receiving requests and, eventually, managing and executing queued tasks.

The CLI sends a task request containing the task name and project directory. The daemon uses that information to locate the appropriate configuration.

## Building

**Requirements:** C++17, CMake, and a Unix-like operating system (macOS or Linux).

```bash
cmake -S . -B build
cmake --build build
```

This builds the `hampr` and `hamprd` executables.

## Development Status

**Hampr is currently under active development.**

The initial CLI task execution functionality has been implemented, and development is focused on transitioning execution to a separate daemon.

Current priorities include:

- Reliable IPC message framing and serialization.
- Complete configuration validation.
- Daemon-managed task execution.
- FIFO task queue management.
- Automatic daemon startup and lifecycle management.

## Project Goals

Hampr is both a practical developer tool and a hands-on exploration of modern C++ systems programming.

The long-term goal is a lightweight, extensible task runner that can manage builds, tests, and custom commands across multiple projects without requiring a complex CI/CD environment.

## License

License to be determined.
