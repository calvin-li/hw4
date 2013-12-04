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
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "readObj.h"

using namespace std;

void load_obj(const char* filename, Mesh* mesh) {
	ifstream in(filename, ios::in);
	if (!in){ 
		cerr << "Cannot open " << filename << endl; 
		exit(1); 
	}

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
				//temp_v.push_back(v);
				mesh->vertices.push_back(v);
		}//if

		else if(line.substr(0, 3) == "vt "){
			istringstream s(line.substr(2));

			glm::vec2 v;
				s >> v.x;
				s >> v.y;
				//temp_t.push_back(v);
				mesh->uvs.push_back(v);
		}//else if
		
		else if(line.substr(0, 3) == "vn "){
			istringstream s(line.substr(2));

			glm::vec3 v;
				s >> v.x;
				s >> v.y;
				s >> v.z;
				mesh->normals.push_back(v);
		}//else if

		else if(line.substr(0, 2) == "f "){
			GLuint v[3], t[3], n[3];

			if(sscanf(line.c_str(), "%*s %d/%d/%d %d/%d/%d %d/%d/%d",
															&v[0],&t[0],&n[0], &v[1],&t[1],&n[1], &v[2],&t[2],&n[2]) == 9){
				for(int i = 0; i < 3; i++){
					v[i] --;	mesh->elements.push_back(v[i]);
					t[i] --;	t_elements.push_back(t[i]);
					n[i] --;	n_elements.push_back(n[i]);
				}//for
			}//if

			/*else{ // Missing things that we need
				cout << "Cannot draw object with provided data. Abort program.\n";
				exit(0);
			}//else*/
		}//else if

		else if(line[0] == '#') {} // Found comment. Move on

		else{} // Everything else we don't care about
	}//while

	cout << mesh->elements.size();

	for(unsigned int i = 0; i < mesh->elements.size(); i++){
		GLuint v_index = mesh->elements[i],
					 t_index = t_elements[i],
					 n_index = n_elements[i];

		//cout << mesh->elements[i] << endl;

		//glm::vec4 v = temp_v[v_index];	mesh->vertices.push_back(v);
		//glm::vec2 t = temp_t[t_index];	mesh->uvs.push_back(t);
		//glm::vec3 n = temp_n[n_index];	mesh->normals.push_back(n);
	}//for
}

#endif

