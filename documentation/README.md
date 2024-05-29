# MalDec EDR (Endpoint Detection and Response) Documentation

Comprehensive Documentation on the Functionality and Components of MalDec-EDR.

## Table of Contents

1. [Build Debian-based](#build-debian-based)
1. [Build Arch-based](#build-debian-based)
2. [Build using Docker](#build-using-docker)
3. [Generate Documentation using Doxygen](#generate-documentation-using-doxygen)
4. [Drivers](#drivers)
5. [Folder organization](#folder-organization)
6. [Unit Tests](#unit-tests)

---

## Build Debian-based

To build projects, you need to download the Debian-based dependencies `libsystemd-dev`, `libsqlite3-dev`, `libyara-dev`. Additionally, for building the GUI using QT, you will need [QT6](https://www.qt.io/product/qt6).

Run this script [scripts/build.sh](../scripts/build.sh) to build the project.

## Build Arch-based

> [!WARNING]
> If you do not use systemd as init the daemon will not work correctly, we are looking for solutions to suit all init systems

To build projects, you need to download the Arch-based dependencies `sqlite3`, `yara`. Additionally, for building the GUI using QT, you will need [QT6](https://www.qt.io/product/qt6).

Run this script [scripts/build.sh](../scripts/build.sh) to build the project.

## Build using Docker 

To build using Docker, run the following command:

``` 
docker build .
```

## Generate Documentation using Doxygen

To generate documentation using Doxygen for the entire codebase, simply run the script [`scripts/doxygen/doxygen.sh`](../scripts/doxygen/doxygen.sh). This script will generate the documentation.

## Drivers

Maldec-edr currently uses 2 drivers to help detect malware, take into account that the drivers and their interactions are in the development phase, nothing consolidated, however it makes up [crowarmor](https://github.com/maldeclabs/CrowArmor).

You can check the features it comprises using the maldec-edr CLI.

```
  CrowArmor Driver:
    --status-crowarmor           Check the driver status and whether its features are active.
```

---

## Folder organization

```markdown
.
├── assets
├── benchmark
├── CMakeLists.txt
├── config
├── Dockerfile
├── documentation
├── drivers
├── include
├── libraries
├── packages
├── README.md
├── rules
├── scripts
├── sources
└── tests

14 directories, 3 files
```

* [include](../include) - headers that modules use
* [sources](../sources) - contains the source code of header and modules
* [libraries](../libraries) - libraries that we often use as submodules
* [rules](../rules) - currently Yara rules
* [packages](../packages) - packages that we use as dependencies
* [config](../config) - contains the project input settings
* [drivers](../drivers) - contains submodules that point to drivers
* [benchmark](../benchmark) - Contains component benchmarks
* [tests](../tests) - contains unit tests
* [assets](../assets) - images
* [scripts](../scripts) - contains automations using shell scripts


## Unit Tests

To build the unit tests you need to enable the build variable which comes as false by default in cmakelists, then after the build run a `ctest` in the folder, an example of the expected result.

```
Test project MalDec-EDR
    Start 1: scanner_test
1/4 Test #1: scanner_test .....................   Passed    0.24 sec
    Start 2: json_parser_config_test
2/4 Test #2: json_parser_config_test ..........   Passed    0.02 sec
    Start 3: inotify_test
3/4 Test #3: inotify_test .....................   Passed    0.01 sec
    Start 4: inspector_test
4/4 Test #4: inspector_test ...................   Passed    0.63 sec

100% tests passed, 0 tests failed out of 4

Total Test time (real) =   0.96 sec
```