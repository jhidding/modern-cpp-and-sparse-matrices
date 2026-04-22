---
icon: lucide/lightbulb
---

# The Idea

Refamiliarize myself with C++ development cycle, the Eigen library and doing some benchmarks on dense versus sparse matrix representations.

## Building

This project uses Meson to build its executables.

```meson
#| file: meson.build
project('sparse-world', 'cpp',
    version : '0.1.0',
    default_options : ['cpp_std=c++23'])

incdir = include_directories('src')

# libeigen = dependency('eigen3', version: '>=3.4')
libeigen_inc = include_directories('./vendor/eigen-5.0.1')
libeigen = declare_dependency(include_directories : libeigen_inc)

argparse_inc = include_directories('./vendor/argparse-3.2/include')
argparse = declare_dependency(include_directories : argparse_inc)

benchmark_inc = include_directories('./vendor/benchmark-1.9.5/include')
cc = meson.get_compiler('cpp')
benchmark_lib = meson.global_source_root() / 'vendor/benchmark-1.9.5/build/src/libbenchmark.a'
benchmark = declare_dependency(
    include_directories : benchmark_inc,
    link_args : benchmark_lib)

<<meson-executables>>
```

You may build this by running,

```bash
meson build
ninja -C build
./build/hello
```

By default, this will create a debug executable (i.e. `-O0 -g`). To get a release build, add `--buildtype release` to the Meson command.

### Makefile
This project also has a `Makefile` for ease-of-use. Without arguments `make` will perform a debug build in `build/debug`. For release build, run `make release`. Please appreciate the mess we're in: GNU Make calls Meson, that prepares a Ninja file, and then Make also calls Ninja. To be fair, once the build directories are setup, all you need to do is call `ninja -C build/debug` (or `release`) to rebuild, but that's still more characters than just `make`.

## Resources

- [CPP Reference](https://en.cppreference.com/index.html)
- [Zensical Documentation](https://zensical.org/docs/get-started/)
- [Eigen Library](https://libeigen.gitlab.io/)
- [Meson Build System](https://mesonbuild.com/)
