# FLAME

# Required Packages installation on Ubuntu
```
$ sudo apt-get install build-essential libmosquittopp-dev libmosquitto-dev
```

# Build on Ubuntu (x86_64 Architecture)
```
$ apt-get install libzmq3-dev
```

# Build on Raspberry Pi 4 (ARM64 Architecture)
```
$ apt-get install g++-aarch64-linux-gnu gcc-aarch64-linux-gnu
```
* [Warning] Cross built flame execuable file shows error on raspbian 64bit(arm64) as below. It strongly depends on libc version, but it cannot be solved this problem.
./flame: /lib/aarch64-linux-gnu/libc.so.6: version `GLIBC_2.32' not found (required by ./flame)
./flame: /lib/aarch64-linux-gnu/libc.so.6: version `GLIBC_2.34' not found (required by ./flame)
./flame: /lib/aarch64-linux-gnu/libstdc++.so.6: version `GLIBCXX_3.4.29' not found (required by ./flame)


# Dependencies
 - libsqlite-dev 3.37.2