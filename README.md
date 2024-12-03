# Peeker
[![](https://img.shields.io/badge/Author-Xu.Cao-lightgreen)](https://github.com/SteveCurcy) [![](https://img.shields.io/badge/Dependencies-libbpf-blue)](https://github.com/libbpf/libbpf-bootstrap) ![](https://img.shields.io/badge/Version-0.0.1-yellow)

## 使用说明

### 安装依赖

本程序使用 libbpf 进行开发，需要安装 clang（v11 及以上版本），libelf 和 zlib 来完成构建。

在 Debian/Ubuntu 上，请执行：

```shell
apt install clang libelf1 libelf-dev zlib1g-dev cmake
```

在 CentOS/Fedora 上，请执行：

```shell
dnf install git clang elfutils-libelf elfutils-libelf-devel zlib-devel cmake
# 下面的是可选项.
# The command below is optional.
dnf install binutils-devel llvm llvm-devel libcap-devel
```

在 openSUSE 上，请执行：

```shell
zypper install -y git gcc ncurses-devel libelf-devel bc libopenssl-devel \
libcap-devel clang llvm graphviz bison flex glibc-devel-static
```
