
gazebo/build cmake: gazebo/CMakeLists.txt
	mkdir -p gazebo/build
	cd gazebo/build && cmake ..

build_simulator: gazebo/build
	make -C gazebo/build

simulator: build_simulator
	GAZEBO_MODEL_PATH=$$GAZEBO_MODEL_PATH:${CURDIR}/gazebo/models \
	GAZEBO_PLUGIN_PATH=$$GAZEBO_PLUGIN_PATH:${CURDIR}/gazebo/build \
	gazebo --verbose ${CURDIR}/gazebo/flix.world

clean:
	rm -rf gazebo/build flix/build flix/cache

.PHONY: cmake build_simulator simulator clean
