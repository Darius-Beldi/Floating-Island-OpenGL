# Floating-Island-OpenGL

## Conceptul Proiectului
Un proiect care consta intr-o scena 3D ce reprezinta o 3 insule plutitoare (2 mici si una mai mare).
Pe insula principala se afla o casa, un copac mare si 2 lampi de iluminat.
Pe insulele mici se afla copaci si o cruce mare. 
Scopul nostru era sa creem o scena horror, de aceea avem si putina lumina. 

## Elementele incluse

Scena este realizata in Blender, apoi exportata in .obj. Iluminarea este facuta complet in OpenGL.

## De ce este original? 

Am incercat sa creem o atmosfera cat mai spooky, si credem ca ne-a iesit.

De asemenea am extins clasa *objloader*  cu functia *loadmtl* 
```
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
Aceasta permite programului sa implementeze si fisierele *.mtl din Blender.


## Contributii individuale 

Am lucrat amandoi la acest proiect in egala masura. 
Darius s-a ocupat de scena 3D, iar Ovidiu s-a ocupat de iluminare.

