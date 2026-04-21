# ~/~ begin <<docs/index.md#meson.build>>[init]
project('sparse-world', 'cpp',
    version : '0.1.0',
    default_options : ['cpp_std=c++23'])

libeigen = dependency('eigen3', version: '>=3.4')

# ~/~ begin <<docs/eigen.md#meson-executables>>[init]
executable('eigen-hello', 'src/eigen_hello.cpp',
    dependencies: [libeigen])
# ~/~ end
# ~/~ begin <<docs/eigen.md#meson-executables>>[1]
executable('random-values', 'src/random_values.cpp')
# ~/~ end
# ~/~ begin <<docs/eigen.md#meson-executables>>[2]
executable('solve-dense', 'src/solve-dense.cpp',
    dependencies: [libeigen])
# ~/~ end
# ~/~ begin <<docs/modern-cpp.md#meson-executables>>[0]
executable('hello', 'src/hello_world.cpp')
# ~/~ end
# ~/~ begin <<docs/modern-cpp.md#meson-executables>>[1]
executable('jetzt', 'src/concepts.cpp')
# ~/~ end
# ~/~ end
