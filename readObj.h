#include <GL/gl.h>
#include <vector>
#include <glm/glm.hpp>
//#include <OpenGL/gl3.h>

// Mesh class
// vertices are a vector of 4D points (x,y,z,1)
// normals are a vector of 3D point
// but the elements (triangles) is a vector of 3m integer indices,
// three per triangle. 
class Mesh {

public:
  std::vector<glm::vec4> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
  std::vector<GLuint> elements;

  // Creation and destruction
  Mesh() {}
  ~Mesh() {}
};


// Function to read in object in .obj format
void load_obj(const char*, Mesh*);

