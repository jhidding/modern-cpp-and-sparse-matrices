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

libeigen = dependency('eigen3', version: '>=3.4')

<<meson-executables>>
```

You may build this by running,

```bash
meson build
ninja -C build
./build/hello
```

By default, this will create a debug executable (i.e. `-O0 -g`). To get a release build, add `--buildtype release` to the Meson command.

## Resources

- [CPP Reference](https://en.cppreference.com/index.html)
- [Zensical Documentation](https://zensical.org/docs/get-started/)
- [Eigen Library](https://libeigen.gitlab.io/)
- [Meson Build System](https://mesonbuild.com/)



