/**
 * From the OpenGL Programming wikibook: http://en.wikibooks.org/wiki/OpenGL_Programming
 * This file is in the public domain.
 * Contributors: Sylvain Beucler
 */
#ifndef READOBJ_H
#define READOBJ_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "readObj.h"

using namespace std;

//index builder from https://github.com/huamulan/OpenGL-tutorial/blob/master/common/vboindexer.cpp

struct PackedVertex{
    glm::vec4 position;
    glm::vec2 uv;
    glm::vec3 normal;
    bool operator<(const PackedVertex that) const{
        return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
    }
};//PackedVertex

bool getSimilarVertexIndex_fast(
    PackedVertex & packed,
    map<PackedVertex,unsigned short> & VertexToOutIndex,
    unsigned short & result
){
    map<PackedVertex,unsigned short>::iterator it = VertexToOutIndex.find(packed);
    if ( it == VertexToOutIndex.end() ){
        return false;
    }else{
        result = it->second;
        return true;
    }//else
}//getSimilarVertexIndex_fast

void indexVBO(
    vector<glm::vec4> & in_vertices,
    vector<glm::vec2> & in_uvs,
    vector<glm::vec3> & in_normals,

    vector<GLuint> & out_indices,
    vector<glm::vec4> & out_vertices,
    vector<glm::vec2> & out_uvs,
    vector<glm::vec3> & out_normals
){
    map<PackedVertex,unsigned short> VertexToOutIndex;

    // For each input vertex
    for ( unsigned int i=0; i<in_vertices.size(); i++ ){

        PackedVertex packed = {in_vertices[i], in_uvs[i], in_normals[i]};

        // Try to find a similar vertex in out_XXXX
        unsigned short index;
        bool found = getSimilarVertexIndex_fast( packed, VertexToOutIndex, index);

        if ( found ){ // A similar vertex is already in the VBO, use it instead !
            out_indices.push_back( index );
        }else{ // If not, it needs to be added in the output data.
            out_vertices.push_back( in_vertices[i]);
            out_uvs .push_back( in_uvs[i]);
            out_normals .push_back( in_normals[i]);
            GLuint newindex = (unsigned short)out_vertices.size() - 1;
            out_indices .push_back( newindex );
            VertexToOutIndex[ packed ] = newindex;
        }//else
    }//for
}//indexVBO

void load_obj(const char* filename, Mesh* mesh) {
	ifstream in(filename, ios::in);
	if (!in){ 
		cerr << "Cannot open " << filename << endl; 
		exit(1); 
	}//if

    vector<glm::vec4> raw_v;
    vector<glm::vec2> raw_t;
    vector<glm::vec3> raw_n;

    vector<glm::vec4> temp_v;
    vector<glm::vec2> temp_t;
    vector<glm::vec3> temp_n;

	vector<GLuint> t_elements;
	vector<GLuint> n_elements;

	string line;
	while(getline(in, line)){
		if(line.substr(0, 2) == "v "){
			istringstream s(line.substr(2));

			glm::vec4 v;
				s >> v.x;
				s >> v.y;
				s >> v.z;
                v.w = 1.0;
                //raw_v.push_back(v);
                raw_v.push_back(v);
		}//if

		else if(line.substr(0, 3) == "vt "){
			istringstream s(line.substr(2));

			glm::vec2 v;
				s >> v.x;
				s >> v.y;
                raw_t.push_back(v);
				//mesh->uvs.push_back(v);
		}//else if
		
		else if(line.substr(0, 3) == "vn "){
			istringstream s(line.substr(2));

			glm::vec3 v;
				s >> v.x;
				s >> v.y;
				s >> v.z;
                raw_n.push_back(v);
				//mesh->normals.push_back(v);
		}//else if

		else if(line.substr(0, 2) == "f "){
			GLuint v[3], t[3], n[3];

			// vertices + textures + normals
			if(sscanf(line.c_str(), "%*s %d/%d/%d %d/%d/%d %d/%d/%d",
                &v[0],&t[0],&n[0], &v[1],&t[1],&n[1], &v[2],&t[2],&n[2]) == 9){
				for(int i = 0; i < 3; i++){
                    temp_v.push_back( raw_v[v[i]-1] );
                    temp_t.push_back( raw_t[t[i]-1] );
                    temp_n.push_back( raw_n[n[i]-1] );
				}//for
			}//if

			// vertices + textures
//			else if(sscanf(line.c_str(), "%*s %d/%d %d/%d %d/%d",
//																		&v[0],&t[0], &v[1],&t[1], &v[2],&t[2]) == 6){	
//				for(int i = 0; i < 3; i++){
//					v[i] --;	mesh->elements.push_back(v[i]);
//					t[i] --;	t_elements.push_back(n[i]);
//				}//for
//			}//else if

			// vertices + normals
			else if(sscanf(line.c_str(), "%*s %d//%d %d//%d %d//%d",
																		&v[0],&n[0], &v[1],&n[1], &v[2],&n[2]) == 6){	
				for(int i = 0; i < 3; i++){
					v[i] --;	mesh->elements.push_back(v[i]);
					n[i] --;	n_elements.push_back(n[i]);
				}//for
			}//else if

			// vertices
			else if(sscanf(line.c_str(), "%*s %d %d %d",
																		&v[0], &v[1], &v[2]) == 3){	
				for(int i = 0; i < 3; i++){
					v[i] --;	mesh->elements.push_back(v[i]);
				}//for
			}//else if

			/*else{ // Missing things that we need
				cout << "Cannot draw object with provided data. Abort program.\n";
				exit(0);
			}//else*/
		}//else if

		else if(line[0] == '#') {} // Found comment. Move on

		else{} // Everything else we don't care about
	}//while

    indexVBO(temp_v, temp_t, temp_n, mesh->elements, mesh->vertices, mesh->uvs, mesh->normals);

}//load_obj

#endif

