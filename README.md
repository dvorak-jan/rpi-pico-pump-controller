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
