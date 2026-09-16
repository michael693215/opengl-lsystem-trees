# opengl-lsystem-trees
An L-System modeling simulation written in C++ and OpenGL as part of the Spring 2026 Computer Graphics course taught by professor Marc Olano @ UMBC. (Examples are at the bottom of the page)

## Overview
### L-Systems
An L-system is a context-free grammar (CFG) that can be described as "an alphabet of symbols that can be used to make strings, a collection of production rules that expand each symbol into some larger string of symbols, an initial "axiom" string from which to begin construction, and a mechanism for translating the generated strings into geometric structures." as stated on the Wikipedia page on L-systems: https://en.wikipedia.org/wiki/L-system. Like a CFG would, it follows the formula G = (V, T, S, P) where:
  - ```V``` is a set of non-terminals that change between iterations,
  - ```T``` is a set of terminals that do not change between iterations,
  - ```S``` is a start symbol from the set union of ```V``` and ```T```,
  - P is a set of production rules that follow the form ```A -> x``` where ```A``` is in ```T``` and ```x``` is in the set union of ```V``` and ```T```.

For example, an L-system where capital letters are terminals and lower-case letters are non-terminals that has a start symbol, ```S = aAA```, and production rule(s), ```P => { a = Aa }``` would have the following generations:
  0. ```aAA```
  1. ```AaAA```
  2. ```AAaAA```
  3. ```AAAaAA```
  4. etc.

As you can see, every generation, every non-terminal (in this case, it's just ```a```) in the string is resolved once, and the non-terminals do not change. This means that if you had a start symbol that was, instead, ```AAA```, then the string will not change after any number of generations. Interesting patterns can be formed with the use of L-systems. 
### Simulation
In this project, an OpenGL wrapper class that is mostly prepared by professor Marc Olano @ UMBC, some shader code, and texture ppm files are modified and extended in order to display a 3D simulation of an L-system tree with the following grammar:
  - 'A'–'Z': add a branch segment and translate to the end (so subsequent rotations will be around the tip of the branch).
  - '^': a 103° rotation around the current branch axis (4π/7 radians)
  - "+" and "-": +30° and –30° rotations around the branch x axis
  - "[" and "]": push and pop the transformation stack
  - 'a'–'z': additional non-branch terminals or non-terminals

### Writing your own L-systems
If you are creating your own lsystem file, then the first line should be the start symbol or string, and each subsequent line should define a production rule. The ```=``` symbol separates non-terminals (lhs) from its replacement string. Sample L-system files can be found in the ```GLapp/data/trees``` directory, ready to use. 

### Usage
This project uses ```cmake``` to build the files, GLFW to create the window, and GLEW to manage OpenGL functions and extensions. For Linux, the packages will be downloadable from your package manager. For Mac, you can use homebrew.

For Windows:
  1) download the GLlibs.zip file (https://userpages.cs.umbc.edu/olano/435/GLlibs.zip),
  2) extract the contents,
  3) run the envset.bat script,
  4) restart CMake,
  5) use CMake to generate the GLapp.slnx file.
The envset.bat script should be run in the same directory as the GLlibs files.

After acquiring the GL packages:
1. run ```cmake -B build -S .``` from the ```GLapp``` directory. You can also run ```cmake -B build -S . -DCMAKE_BUILD_TYPE=Release``` to speed up the program.
2. Afterwards, run ```cmake --build build``` from the trace directory to build the program.
3. Finally, you can execute with ```program_name -[options] file.txt```.

#### Options
The program takes 2 arguments, the first being options, and the second being the L-system text file. These options include:
- ```-h```: Shows a short usage guide.
- ```-g(count)```: Specifies the number of generations to execute for the initial view of the program.

#### Output
The output is a GLFW window. Within it, you can press the following keys:
- W/A/S/D: Panning and tilting the screen.
- P/O: Zooming in and out.
- J/K: Increasing or decreasing the number of generations and re-rendering the tree.
- Mouse Click: Lets you pan and tilt the screen with your mouse.
- L: Toggle wireframe view.
- I: Cycle through light intensities.

## Examples 

### tree1.txt with 6 generations
<img width="1448" height="767" alt="image" src="https://github.com/user-attachments/assets/eade053d-b4fb-4293-a813-a85f9a6520e3" />

### tree1.txt with 8 generations (going beyond 8 causes quite a bit of lag)
<img width="961" height="680" alt="image" src="https://github.com/user-attachments/assets/537230c0-4875-4c0d-bbda-9f8f5005e588" />

### tree2.txt with 4 generations
<img width="933" height="576" alt="image" src="https://github.com/user-attachments/assets/94464915-d451-4f8a-abe9-ad7ffb98cd68" />

