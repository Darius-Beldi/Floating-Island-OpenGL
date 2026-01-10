#pragma once
#include <vector>
#include <string>
#include "glm/glm.hpp"
#include <map>

struct Material {
    std::string name;
    glm::vec3 diffuseColor = glm::vec3(0.8f, 0.8f, 0.8f); // Kd
};

bool loadOBJ(
    const char* path,
    std::vector<glm::vec3>& out_vertices,
    std::vector<glm::vec2>& out_uvs,
    std::vector<glm::vec3>& out_normals,
    std::vector<glm::vec3>& out_colors // nou!
);


bool loadAssImp(
	const char * path, 
	std::vector<unsigned short> & indices,
	std::vector<glm::vec3> & vertices,
	std::vector<glm::vec2> & uvs,
	std::vector<glm::vec3> & normals
);