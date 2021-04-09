---
nav_order: 4
title: Software Setup
---

# Software Setup

We have tried to make the installation process as easy as possible. We provide two options for installation:

1. [Install our Pre-Built Binary](#1-install-our-pre-built-binary)
1. [Install from Source](#2-install-from-source)

## 1. Install our Pre-Built Binary

[PosturePerfection Releases](https://github.com/ESE-Peasy/PosturePerfection/releases){: .btn .btn-outline}

If you are only interested in running PosturePerfection with minimal overhead, you can download our latest release supported on both the Raspberry Pi and Linux systems. Our installation script downloads the binary which is appropriate for your system:

### Download and Run

```sh
wget https://raw.githubusercontent.com/ESE-Peasy/PosturePerfection/main/PosturePerfection_install.sh
sh PosturePerfection_install.sh
```

Follow the instructions provided!

## 2. Install from Source

To install PosturePerfection from source run the following commands:

### Clone our GitHub Repository

```sh
git clone https://github.com/ESE-Peasy/PosturePerfection.git
```

### Install the required dependencies

Note that this step includes the installation of Tensorflow and OpenCV from source, and is therefore expected to take a long time (approximately 30 mins to 1 hour).

```sh
cd PosturePerfection && ./scripts/install-dependencies.sh
```

### Build PosturePerfection

```sh
./scripts/build.sh
```

Follow the instructions provided!
