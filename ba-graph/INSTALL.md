# Installing `ba-hraph`
### Install dependencies

For compiling and development you have to install the following packages:

* gcc (in fact any C++ compiler) supporting concepts (c++-2a)
* libstdc++ >= 8.0.0 (on macos)
* CMake 3.10
* [google-test](https://github.com/google/googletest.git) (as a git submodule)

### Compiling with CMake

In the project root create the folder `build` and then...

        cd ./build
        cmake ..
        make

If your compiler does not support concepts (or you want to choose the other than system default)

        cmake -DCMAKE_CXX_COMPILER=<absolute_path_to_c++> ..

or

        CXX=<absolute_path_to_c++> cmake ..



To perform a verbose build issue

        make VERBOSE=ON

instead the last commandline. You can also use parallel build (e.g. `-j 4`)

#### Compilation variants with CMake

All of the following options can be set by using `cmake -D<option>=<value> ..`

* `WERROR={ON, OFF}`, default `ON`
* `CMAKE_BUILD_TYPE={Debug, Release, MinSizeRel, RelWithDebInfo}`  default `Debug`
* `BUILD_TESTS={ON, OFF}`, default `ON`



To run tests you should issue (still in `<project_root>/build`) e.g.

    test/impl/impl_tests

This information is valid just for present state, the way to run test will be changed soon.

The best way to deal with CMake-driven build is to use IDE like e.g. [CLion](https://www.jetbrains.com/clion/)
or [Qt Creator](https://www.qt.io/download-qt-installer).
