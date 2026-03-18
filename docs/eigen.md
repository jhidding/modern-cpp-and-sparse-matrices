---
icon: material/owl
---

# About Eigen

Eigen is a header-only library for doing linear algebra in C++.

```meson
#| id: meson-executables
executable('eigen-hello', 'src/eigen_hello.cpp',
    dependencies: [libeigen])
```

```c++
//| file: src/eigen_hello.cpp
#include <cstdlib>
#include <iostream>
#include <Eigen/Dense>

using Eigen::MatrixXd;

int main() {
    MatrixXd m(2,2);
    m(0,0) = 3;
    m(1,0) = 2.5;
    m(0,1) = -1;
    m(1,1) = m(1,0) + m(0,1);
    std::cout << m << "\n";
    return EXIT_SUCCESS;
}
```
