# OpenGLSceneViewer

Unisinos Porto Alegre, 2019/1

Prof. Rossana Queiroz

Developed by Thiago Hardt and João Rothmann

## The Project
OpenGL scene viewer, where you can move the camera around, and load objects (.obj files) in it.
Made using OpenGL, and following the methods described in https://learnopengl.com/. 

### How To Use It
Open the Visual Studio solution and compile the Exercise4 project. There is a *sceneDirectory* variable that defines the path to the file that will be used to load the scene. The program uses formats in .cgs format, which is a unique format for this project. There is a sample file in the *scenes* folder. The structure of the file is explained within the *readerCGS.h* header

## Controls

### Movement
**W** - Move Upwards

**A** - Move Leftwards

**S** - Move Downwards

**D** - Move Rightwards

**SPACE** - Move Upwards

**L SHIFT** - Move Downwards

**MOUSE** - Look Around

**SCROLL** - Zoom In/Out

### Transform
**TAB** - Switches Which Model To Transform

**U** - Apply Transformation (All Modes)

**I** - Apply Transformation (Translating and Rotating)

**O** - Apply Transformation (All Modes)

**J** - Apply Transformation (Translating and Rotating)

**K** - Apply Transformation (Translating and Rotating)

**L** - Apply Transformation (Translating and Rotating)

**C** - Cycle Through Cameras

**X** - Cycle Through Lights

**T** - Animate In Parametric Curve

**1** - Change Transformation Mode to **ROTATING**

**2** - Change Transformation Mode to **TRANSLATING**

**3** - Change Transformation Mode to **SCALING**
