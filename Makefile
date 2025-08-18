
plugin/build cmake: plugin/CMakeLists.txt
	mkdir -p plugin/build
	cd plugin/build && cmake ..

build_simulator: plugin/build
	make -C plugin/build

simulator: build_simulator
	GZ_SIM_RESOURCE_PATH=${CURDIR}/models:$$GZ_SIM_RESOURCE_PATH \
	GZ_SIM_SYSTEM_PLUGIN_PATH=${CURDIR}/plugin/build:$$GZ_SIM_SYSTEM_PLUGIN_PATH \
	gz sim -v 3 -s -r --headless-rendering world.sdf

	
.PHONY: cmake build_simulator simulator

	# gz sim -v 3 world.sdf