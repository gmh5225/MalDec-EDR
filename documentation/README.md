# MalDec EDR (Endpoint Detection and Response) Documentation

Comprehensive Documentation on the Functionality and Components of MalDec-EDR.

## Table of Contents

1. [Build Debian-based](#build-debian-based)
2. [Build using Docker](#build-using-docker)
3. [Generate Documentation using Doxygen](#generate-documentation-using-doxygen)
4. [Folder organization](#folder-organization)

---

## Build Debian-based

To build projects, you need to download the Debian-based dependencies `libsystemd-dev`, `libsqlite3-dev`, `libyara-dev`. Additionally, for building the GUI using QT, you will need [QT6](https://www.qt.io/product/qt6).

Run this script [scripts/build.sh](../scripts/build.sh) to build the project.

## Build using Docker 

To build using Docker, run the following command:

``` 
docker build .
```

## Generate Documentation using Doxygen

To generate documentation using Doxygen for the entire codebase, simply run the script [`scripts/doxygen/doxygen.sh`](../scripts/doxygen/doxygen.sh). This script will generate the documentation.

# Drivers

Maldec-edr currently uses 2 drivers to help detect malware, take into account that the drivers and their interactions are in the development phase, nothing consolidated, however it makes up [crowarmor](https://github.com/maldeclabs/CrowArmor) and [telekinesis](https://github.com/maldeclabs/Telekinesis).

You can check the features it comprises using the maldec-edr CLI.

```
  Telekinesis Driver:
    --status-telekinesis         Check the driver status and whether its features are active.

  CrowArmor Driver:
    --status-crowarmor           Check the driver status and whether its features are active.
```

# Folder organization

```md
.
├── [assets](../assets/)
├── [benchmark](../benchmark/)
├── CMakeLists.txt
├── [config](../config/)
├── Dockerfile
├── [documentation](../documentation/)
├── [drivers](../drivers/)
├── [include](../include)
├── [libraries](../libraries/)
├── [packages](../packages/)
├── README.md
├── [rules](../rules)
├── [scripts](../scripts/)
├── [sources](../sources/)
└── [tests](../tests/)

14 directories, 3 files

```

* include - headers that modules use
* sources - contains the source code of header and modules
* libraries - libraries that we often use a submodule
* rules - currently yara rules
* packages - packages that we use as dependencies
* config - contains the project input settings
* drivers - contains submodules that point to drivers
* benchmark - Contains component benchmark
* tests - contains unit tests
* assets - images
* script - contains automations using shell scripts