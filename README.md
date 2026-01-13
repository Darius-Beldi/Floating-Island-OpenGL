# Floating-Island-OpenGL

## Project Concept
A project consisting of a 3D scene representing 3 floating islands (2 small ones and one larger).
On the main island there is a house, a large tree, and 2 streetlamps.
On the small islands there are trees and a large cross.
Our goal was to create a horror scene, which is why we have limited lighting.

### Rendered scene
https://github.com/user-attachments/assets/e3dbf8d0-bbdc-41f9-8977-23ab635f5a2d

## Included Elements
The scene is created in Blender, then exported to .obj format. The lighting is implemented entirely in OpenGL.

## Why is it original?
We tried to create an atmosphere that is as spooky as possible, and we believe we succeeded.
Additionally, we extended the *objloader* class with the *loadmtl* function:

```cpp
bool loadMTL(const std::string& path, std::map<std::string, Material>& materials) {
    FILE* file = fopen(path.c_str(), "r");
    if (!file) return false;
    char lineHeader[128];
    Material mat;
    std::string currentMat;
    while (fscanf(file, "%s", lineHeader) != EOF) {
        if (strcmp(lineHeader, "newmtl") == 0) {
            if (!currentMat.empty()) {
                materials[currentMat] = mat;
            }
            char name[128];
            fscanf(file, "%s\n", name);
            currentMat = name;
            mat = Material();
            mat.name = currentMat;
        }
        else if (strcmp(lineHeader, "Kd") == 0) {
            fscanf(file, "%f %f %f\n", &mat.diffuseColor.r, &mat.diffuseColor.g, &mat.diffuseColor.b);
        }
        else {
            char buffer[1024];
            fgets(buffer, 1024, file);
        }
    }
    if (!currentMat.empty()) {
        materials[currentMat] = mat;
    }
    fclose(file);
    return true;
}
```

This allows the program to implement the *.mtl files from Blender as well.

## Individual Contributions
We both worked on this project equally.
Darius handled the 3D scene, while Ovidiu handled the lighting.


## Pciture of the render
<img width="1032" height="772" alt="image" src="https://github.com/user-attachments/assets/598c5133-79a3-4a4b-840d-9f1347741e9e" />
