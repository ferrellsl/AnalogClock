Install Freeglut on your system and open your MSYS2 shell and navigate to the Windows folder. Run the MAKE command in the Windows folder.

To install freeglut under MSYS2 run the following command in the MSYS2 shell: pacman -S mingw-w64-x86_64-freeglut

Run the clock by typing: ./Debug/AnalogClock.exe from the MSYS2 shell.
If running the binary from the Windows command prompt, make sure that libfreeglut.dll is also in the Debug folder and type: Debug\AnalogClock.exe


