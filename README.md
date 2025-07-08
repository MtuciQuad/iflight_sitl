# Installation and simulator launch
Setup container:
1) Install distrobox and podman
2) ```distrobox create --root --image ubuntu:24.04 --name "container name"```
3) ```distrobox enter --root "container name" ```

Install libs and Gazebo:
1) ```sudo apt-get update```
2) ```sudo apt-get upgrade```
3) Install Gazebo Harmonic (https://gazebosim.org/docs/harmonic/install_ubuntu/)
4) ```sudo apt install libgz-cmake4-dev```
5) ```sudo apt-get install build-essential```
6) Install gz-transport13 (https://aur.archlinux.org/packages/gz-transport13)

Clone the project:
1) ```git clone https://github.com/HungryM1NT/Simulator_new.git```
2) ```cd Simulator_new```

Run simulator:
```
make simulator
```
To use simulator you always need to be in the ubuntu container

# RC data processing and publishing program
Go in rust_utility folder and launch the rc handler in another terminal:
1) sudo chown "username" "hidraw file for RC (example: /dev/hidraw3)"
2) ```cargo run -r --bin rc```

# Firmware part
In rc_imu_motor.rs you can find a combination of rc, inu and motor forces processing
## Subscribe to RC
In get_rc.rs you can find a way to get data from rc publisher.
## Get data from imu
In imu_sub.rs you can find a way to get data from imu.
## Publish motor data
In motor_pub.rs you can find a way to publish motor data
