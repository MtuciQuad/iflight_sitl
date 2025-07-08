Setup container:
1) Install distrobox and podman
2) distrobox create --root --image ubuntu:24.04 --name "container name"
3) distrobox enter --root "container name" 

Install libs and Gazebo:
1) sudo apt-get update
2) sudo apt-get upgrade
3) sudo apt-get install git curl lsb-release gnupg
4) Install Gazebo Harmonic (https://gazebosim.org/docs/harmonic/install_ubuntu/)
5) sudo apt install libgz-cmake4-dev
6) sudo apt-get install build-essential

Clone the project:
1) git clone https://github.com/HungryM1NT/Simulator_new.git
2) cd Simulator_new

Run simulator:
```
make simulator
```