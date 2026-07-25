# AnalogClock
Analog Clock using OpenGL and FreeGlut for Windows and Linux


I've always been fascinated by analog clocks and found one that I liked very much at: https://github.com/yagudaev/analog-clock
The code there hasn't been maintained in 1.5 decades and it had two major problems and another minor problem.  Firstly, it had a memory leak and would cause ~300KB of system memory to be allocated every second causing a total system crash if you left the clock running for several hours.  Second, it spiked CPU utilization at 100%.  And lastly, when resizing the clock's window, the clock wouldn't resize/rescale with the window.  I've fixed these problems and provided versions for Windows using MSYS2 and Linux.


<img width="746" height="687" alt="image" src="https://github.com/user-attachments/assets/72c85d9b-3029-4f7b-b75f-38757f20beca" />

