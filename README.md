# Galloc
Galloc is memory allocation and garbage collection project with user interface.

I want to experiance about arena memory allocation, stack usage and garbage collection
techniques like Java's heaven, tracing, reference counting, escape analyse and manual
collecting from strach.

This is pure c project and it only depends on [Raylib](https://www.raylib.com/) (located on `./raylib-5.0`) for creating graphics.
You can choose the management strategy and write user stort in to the `story.txt` file
(create scope, define variable, alloc memory, close scope, collect, etc. as like as programing lang)
then its virtualize the story.


Galloc has hot-reload feature for development. All logics and drawing jobs in
`plugin.c` file. `main.c` file only load `libplugin.so` file using dynamic library
functions and create window.

**Table of Contents**:

- [setup](#setup-code)
  - [install](#install-source-code)
  - [run](#run-code)
- [structure of project](#structure-of-project)
- [development](#development)
- [contributing](#contributing)
- [license](#license)

> [!WARNING]
> This is experimental project, **DO NOT USE** in prod!

## Setup Code

### Install
Clone this project using GIT

```bash
git clone git@github.com:saracalihan/galloc
```

Go into directory

```bash
cd galloc
```

Now, you **have to compile raylib**. Please read [raylib readme](./raylib-5.0/README.md).

After the compile of reylib,

Compile all source code:

```bash
make
```

This command compile and link main executable and libplugin.

### Run Code

Run main executable:
```bash
./main
```

## Structure of Project

## Development
If you have only made improvements related to memory management or drawing,
run the following command while the `main` executable is running:

```bash
make build_plugin
```

Then press the `r` key in the user interface. When you press the `r` key,
the current application data (memory, variables, etc.) is kept in the
`Plugin` struct and `libplugin.so` is reopened and the values are restored.

## Contributing
This project accepts the open source and free software mentality in its main terms.
Development and distribution are free within the framework of the rules specified
in the license section, BUT the course and mentality of the project depends entirely
on my discretion.

**Please respect this mentality and contributing rules**

## License
This projct is under the [GNU GENERAL PUBLIC LICENSE](./LICENSE).
