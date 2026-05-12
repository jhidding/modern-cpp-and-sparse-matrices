---
icon: lucide/orbit
title: Working with TUDAT
---

These are some notes on working with TUDAT.

## Building

After generating and activating the Conda environment in `environment.yaml`, we can build with

```bash
mkdir build
cd build
ccmake .. -GNinja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
ninja -j 8 design_matrix_sparsity_test
```

- By setting `CMAKE_EXPORT_COMPILE_COMMANDS`, a file `build/compile_commands.json` is being generated that is used by my IDE to run the language server with the proper flags.
- The `-j 8` setting is needed to prevent Ninja from building too many targets in parallel, which takes too much memory.
- By setting a specific target, we don't need to compile all of TudatPy to run a test.
