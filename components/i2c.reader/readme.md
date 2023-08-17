# i2c.reader component


# Dependencies
* Paho MQTT C
```
$ git clone https://github.com/eclipse/paho.mqtt.c.git
$ cd paho.mqtt.c
$ git checkout v1.3.12

$ cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON -DPAHO_BUILD_SHARED=ON -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON -DCMAKE_INSTALL_PREFIX=./build/_install
$ sudo cmake --build build/ --target install
$ sudo ldconfig
```
* Paho MQTT C++
```
$ git clone https://github.com/eclipse/paho.mqtt.cpp
$ cd paho.mqtt.cpp

$ cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON -DPAHO_BUILD_SAMPLES=TRUE -DPAHO_BUILD_DOCUMENTATION=FALSE -DCMAKE_PREFIX_PATH=/home/iae/dev/flame/include/3rdparty/paho.mqtt.c/build/_install -DPAHO_MQTT_C_LIBRARIES=/home/iae/dev/flame/include/3rdparty/paho.mqtt.c/build/_install/lib/libpaho-mqtt3as.a
$ sudo cmake --build build/ --target install
$ sudo ldconfig
```

* Prerequisites
```
$ sudo apt-get install libssl-dev
```

# Notice
1. Tested on NVidia Jetson NX
2. Applied Paho MQTT C Library and C++ Wrapper
    - https://github.com/eclipse/paho.mqtt.c.git
    - https://github.com/eclipse/paho.mqtt.cpp.git

# References
* https://raspberry-projects.com/pi/programming-in-c/i2c/using-the-i2c-interface