// Adaptat dupa http://www.opengl-tutorial.org/ 
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>
#include "glm/glm.hpp"

#include "objloader.hpp"

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

bool loadOBJ(
	const char* path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals,
	std::vector<glm::vec3>& out_colors // nou!
)
{
    printf("Loading OBJ file %s...\n", path);

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices, materialIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
    std::vector<std::string> faceMaterials;

    std::map<std::string, Material> materials;
    std::string currentMaterial = "";
    std::string mtlFile = "";

    // Determină directorul fișierului OBJ
    std::string objPath(path);
    std::string directory = objPath.substr(0, objPath.find_last_of("/\\") + 1);

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Impossible to open the file !\n");
        return false;
    }

    char lineHeader[128];
    while (fscanf(file, "%s", lineHeader) != EOF) {
        if (strcmp(lineHeader, "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0) {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            uv.y = -uv.y;
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0) {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0) {
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9) {
                printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                fclose(file);
                return false;
            }
            for (int i = 0; i < 3; ++i) {
                vertexIndices.push_back(vertexIndex[i]);
                uvIndices.push_back(uvIndex[i]);
                normalIndices.push_back(normalIndex[i]);
                faceMaterials.push_back(currentMaterial);
            }
        }
        else if (strcmp(lineHeader, "mtllib") == 0) {
            char mtlName[128];
            fscanf(file, "%s\n", mtlName);
            mtlFile = directory + mtlName;
            loadMTL(mtlFile, materials);
        }
        else if (strcmp(lineHeader, "usemtl") == 0) {
            char matName[128];
            fscanf(file, "%s\n", matName);
            currentMaterial = matName;
        }
        else {
            char buffer[1024];
            fgets(buffer, 1024, file);
        }
    }
    fclose(file);

    // Asamblează datele finale, inclusiv culoarea
    for (unsigned int i = 0; i < vertexIndices.size(); i++) {
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];

        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        glm::vec2 uv = temp_uvs[uvIndex - 1];
        glm::vec3 normal = temp_normals[normalIndex - 1];

        out_vertices.push_back(vertex);
        out_uvs.push_back(uv);
        out_normals.push_back(normal);

        // Adaugă culoarea materialului curent
        glm::vec3 color(0.8f, 0.8f, 0.8f);
        if (!faceMaterials[i].empty() && materials.count(faceMaterials[i])) {
            color = materials[faceMaterials[i]].diffuseColor;
        }
        out_colors.push_back(color);
    }
    return true;
}



