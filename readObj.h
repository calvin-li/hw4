#include <GL/gl.h>
#include <vector>
#include <glm/glm.hpp>
#include "readBMP.cpp"
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

    GLuint v_buffer_size;
    GLuint u_buffer_size;
    GLuint n_buffer_size;
    GLuint e_buffer_size;

    glm::mat4 moveToOrigin;
    glm::mat4 scaleToCube;
    glm::mat4 model;

    GLuint mObjectBufferID[4];

    GLuint mTextureID;
    Image texImage;

    GLfloat bounds;

  // Creation and destruction
  Mesh():
      moveToOrigin(glm::mat4(1.0f)), scaleToCube(glm::mat4(1.0f)),
      model(glm::mat4(1.0f)), bounds(0.0f)
{
      mObjectBufferID[0] = 0;
      mObjectBufferID[1] = 0;
      mObjectBufferID[2] = 0;
      mObjectBufferID[3] = 0;
      texImage = Image();
}//constructor
  ~Mesh() {}
};


// Function to read in object in .obj format
void load_obj(const char*, Mesh*);

