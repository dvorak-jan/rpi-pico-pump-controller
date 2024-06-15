# RPI Pico Pump Controller

## Description

This project contains firmware for a custom-built pump controller used to switch-on/off a garden pump based on the input from hydrostatic sensor (analog input).

## Dependencies

- FreeRTOS-Kernel is included as submodule.
- pico-sdk is included as submodule.
- Some other files needed to compile and build the project are included directly (in ./pico-cpp).

## License

This project is licensed under the terms of the MIT license.

## Cloning and building of the project

```
git clone https://github.com/dvorak-jan/rpi-pico-pump-controller.git
cd rpi-pico-pump-controller
git checkout develop
git submodule init
git submodule update
cmake -B build
cmake --build build
```

## Collaborators

- Please clone the project, checkout develop branch (``git checkout develop``)

- Create your own feature branch (``git checkout -B feature/my-feature-description``)

- Push to the remote repository as needed and raise PR when ready
- Do not merge into develop without permission

# Debugging/flashing Pico using debug probe, Openocd and Dbg

## Flashing firmware to pico using the debug probe:

Copy the .elf file to /home/jan/pico/openocd/to_flash/

``cp pump_project.elf /home/jan/pico/openocd/to_flash/``

Run the following command from /home/jan/pico/openocd:

``sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program to_flash/pump_project.elf verify reset exit"``

## Using Openocd for access to pico debug features

Start openocd by running the following command from /home/jan/pico/openocd:

``sudo src/openocd -f interface/cmsis-dap.cfg -c "adapter speed 5000" -f target/rp2040.cfg -s tcl``

Start another console window. From the ./build directory (where the .elf file of the project is located), run:

``gdb-multiarch pump_project.elf``

Connect to the pico using the following commands:

``target remote localhost:3333``

``monitor reset init``

``continue``
