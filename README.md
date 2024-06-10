# AutoFrac2DCLI

CLI software that takes an input file as parameter of a fractal structure and exports it in a python script file that can be interpreted by another software (link coming soon...).

## How to build

Clone this repo and build it with CMake.  
Replace `{NB_CORES}` by the number of cores you want to use to compile the project.

```bash
git clone https://github.com/borisbordeaux/AutoFrac2DCLI.git
cd AutoFrac2DCLI
mkdir build
cd build
cmake ..
make -j {NB_CORES}
```

## How to use

```bash
AutoFracCli [-a] [-c] filename
  filename   path to the input file
  -a         automatic position of intern control points
  -c         use cubic bezier curves, default is quadratic
```

The input file defines parameters of a fractal topology.  
The parameter `-a` allows automatic position of intern control points of Bézier curves depending on the one from the curve's extremities.  
The parameter `-c` makes the Bézier curves cubic, otherwise they are quadratic.

You can use the `example/simple.txt` file with the `-a` option. The file contains the coordinates for all cell's corners, not for intern control points.

## The input file

It must contain the definition of:

- the fractal topology of all faces of the structure.
- adjacency constraints for faces in the structure.
- the positions of all control points (or no intern control points if option `-a` is used).

Here is an example of input file, it contains 2 faces, 1 adjacency constraint, and position of all control points but the intern ones.

```text
f
C_2_0 - B_2_0 - C_2_0 - B_2_0 / C_2_0 - B_2_0 - B_2_0 / 0 / 1
C_2_0 - B_2_0 - C_2_0 - B_2_0 / C_2_0 - B_2_0 - B_2_0 / 0 / 1
c
0.3 / 1.1
p
0 10
-10 10
-10 0
0 0
10 10
0 10
0 0
10 0
```

The file starts with an `f`. All following lines until the `c` are parameters for different fractal faces.  
All lines after the `c` until the `p` are the definition of the constraints on the faces.  
All lines after the `p` are the definition of the position of the control points.  
A line starting by `#` is ignored, hence can be used as a comment.

A face definition is separated in 4 parts by ` / `. The parts are the following:

- a list of edges with a form of `X_n_d`, `X` being either `B` or `C` for Bézier or Cantor, `n` being the number of subdivision, and `d` being the delay of the edge subdivision.
- 3 edges that are the adjacency edge, the lacuna edge, and the mid-Cantor edge. They are used by the subdivision algorithm.
- the delay of subdivision of the face.
- the algorithm used, either 0, 1, or 2.

A constraint definition has the form `F1.E1 / F2.E2`, where:

- `F1` is the index of the first face.
- `E1` is the index of the edge of the first face.
- `F2` is the index of the second face.
- `E2` is the index of the edge of the second face. 

A position has the form `x y`, where `x` and `y` can be float numbers.  
They are in a specific order. Face by face, it starts by the position of the vertices of the edge 0. Then follows the positions for the vertices in counter clockwise order.
