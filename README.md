Visual swarm simulation with unique audio effects programmed with C++ and Qt.

See youtube screen cast
https://youtu.be/SRziCX3io6U


It is my fun and learn project. I wanted to simulate the movement or behavior of birds or fished in swarm.
I have created it to refresh my C++ skills and learn the Qt programming.
Anyway the program is usable and it is fun to play with it. My kids love to play with.

Functionality:
- Simple user graphical interface
- Visualize the movement of swarm objects (elements)
- The elements show their status as different colors
- many parameters are customizable at run time
 - speed
 - distance between swarm objects
 - refresh rate
- add and remove elements by simple click
- add another types of elements as barrier and poi (point of interest)
- every elements creates stereo sound that will be mixed to one swarm sound. It gives unique amazing sound visualization
- swarm simulation can be saved to xml files (and loaded again)

# The Algorithm

The algorithm was designed from scratch. Each element moves according the physical rules.
Every element has own logic and operate only because of data it can collect from surrounding area.
Elements have so called see field. The can see other elements and adapt their speed and movement direction.
The elements movement are computed iterative.

# Sound Effects

Each elements generate sinus stereo sound that depends on speed. Additionally the elements generates saw wave sound depending on stress.
Stress is on the one side the status and the another is acceleration (difference in movement).

# Developing

The programm is written in C++ using Qt library. Use QtDesigner to compile it
