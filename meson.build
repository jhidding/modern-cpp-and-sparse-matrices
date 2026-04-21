# ~/~ begin <<docs/index.md#meson.build>>[init]
project('sparse-world', 'cpp',
    version : '0.1.0',
    default_options : ['cpp_std=c++23'])

# libeigen = dependency('eigen3', version: '>=3.4')
libeigen_inc = include_directories('./vendor/eigen-5.0.1')
libeigen = declare_dependency(include_directories : libeigen_inc)

argparse_inc = include_directories('./vendor/argparse-3.2/include')
argparse = declare_dependency(include_directories : argparse_inc)

# ~/~ begin <<docs/eigen.md#meson-executables>>[init]
executable('eigen-hello', 'src/eigen_hello.cpp',
    dependencies: [libeigen])
# ~/~ end
# ~/~ begin <<docs/eigen.md#meson-executables>>[1]
executable('random-values', 'src/random_values.cpp')
# ~/~ end
# ~/~ begin <<docs/eigen.md#meson-executables>>[2]
executable('solve-dense', 'src/solve-dense.cpp',
    dependencies: [libeigen, argparse])
# ~/~ end
# ~/~ begin <<docs/modern-cpp.md#meson-executables>>[0]
executable('hello', 'src/hello_world.cpp')
# ~/~ end
# ~/~ begin <<docs/modern-cpp.md#meson-executables>>[1]
executable('jetzt', 'src/concepts.cpp')
# ~/~ end
# ~/~ end
