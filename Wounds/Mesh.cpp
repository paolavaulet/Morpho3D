// --------------------------------------------------------------------------
// Copyright(C) 2009-2015
// Tamy Boubekeur
//                                                                            
// All rights reserved.                                                       
//                                                                            
// This program is free software; you can redistribute it and/or modify       
// it under the terms of the GNU General Public License as published by       
// the Free Software Foundation; either version 2 of the License, or          
// (at your option) any later version.                                        
//                                                                            
// This program is distributed in the hope that it will be useful,            
// but WITHOUT ANY WARRANTY; without even the implied warranty of             
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              
// GNU General Public License (http://www.gnu.org/licenses/gpl.txt)           
// for more details.                                                          
// --------------------------------------------------------------------------

#include "Mesh.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

void operator>> (std::istream & input, openvdb::Vec3f & v) {
    input >> v[0] >> v[1] >> v[2];
}

void operator>> (std::istream & input, openvdb::Vec3I & v) {
    input >> v[0] >> v[1] >> v[2];
}

openvdb::Vec3f cross (const openvdb::Vec3f & a, const openvdb::Vec3f & b) {
    openvdb::Vec3f r;
    r[0] = a[1] * b[2] - a[2] * b[1];
    r[1] = a[2] * b[0] - a[0] * b[2];
    r[2] = a[0] * b[1] - a[1] * b[0];
    return r;
}

float dist (const openvdb::Vec3f & a, const openvdb::Vec3f & b) {
    return (a-b).length ();
}


void Mesh::loadOFF (const std::string & filename, int l) {
	ifstream in (filename.c_str ());
    if (!in) 
        exit (1);
	string offString;
    unsigned int sizeV, sizeT, tmp;
    in >> offString >> sizeV >> sizeT >> tmp;
    V.resize (sizeV);
    T.resize (sizeT);
    openvdb::Vec3f p(0,0,0);
    for (unsigned int i = 0; i < sizeV; i++){
        in >> p;
        V[i] = l*p;
        //in >> V[i].p;
    }
    int s;
    openvdb::Vec3I n(0,0,0);
    for (unsigned int i = 0; i < sizeT; i++) {
        in >> s;
        in >> n;
        T[i]=n;
        /*for (unsigned int j = 0; j < 3; j++)
            int32_t v;
            in >> T[i].v[j];*/
    }
    in.close ();
  //  centerAndScaleToUnit ();
 //   recomputeNormals ();
}

/*void Mesh::loadOBJ(const std::string &filename){
    
    FILE * file = fopen(filename.c_str(), "r");
    if( file == NULL ){
        printf("Impossible to open the file !\n");
    }
    while( 1 ){
        
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.
        
        if ( strcmp( lineHeader, "v" ) == 0 ){
            float tmpx, tmpy, tmpz;
            fscanf(file, "%f %f %f\n", &tmpx, &tmpy, &tmpz );
            openvdb::Vec3f tmpVertex(tmpx,tmpy,tmpz);
            V.push_back(tmpVertex);
        } else if ( strcmp( lineHeader, "f" ) == 0 ){
            int32_t tmpIndex1, tmpIndex2, tmpIndex3, tmpIndex4;
            std::string vertex1, vertex2, vertex3;
            unsigned int uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d//%d %d//%d %d//%d %d//%d\n", &tmpIndex1, &uvIndex[0], &tmpIndex2, &uvIndex[1], &tmpIndex3, &normalIndex[1], &tmpIndex4, &uvIndex[2]);
            if (matches != 9){
                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
            }
            openvdb::Vec4I tmpVertexIndex(tmpIndex1,tmpIndex2,tmpIndex3,tmpIndex4);
            Q.push_back(tmpVertexIndex);
            secretString = "je suis passé par là";
        }
    }
    i=1;
}*/

/*void Mesh::recomputeNormals () {
    for (unsigned int i = 0; i < V.size (); i++)
        V[i].n = openvdb::Vec3f (0.0, 0.0, 0.0);
    for (unsigned int i = 0; i < T.size (); i++) {
        openvdb::Vec3f e01 = V[T[i].v[1]].p -  V[T[i].v[0]].p;
        openvdb::Vec3f e02 = V[T[i].v[2]].p -  V[T[i].v[0]].p;
        openvdb::Vec3f n = cross (e01, e02);
        n.normalize ();
        for (unsigned int j = 0; j < 3; j++)
            V[T[i].v[j]].n += n;
    }
    for (unsigned int i = 0; i < V.size (); i++)
        V[i].n.normalize ();
}*/

void Mesh::centerAndScaleToUnit () {
    openvdb::Vec3f c;
    for  (unsigned int i = 0; i < V.size (); i++)
        c += V[i];
    c /= V.size ();
    float maxD = dist (V[0], c);
    for (unsigned int i = 0; i < V.size (); i++){
        float m = dist (V[i], c);
        if (m > maxD)
            maxD = m;
    }
    for  (unsigned int i = 0; i < V.size (); i++)
        V[i] = (V[i]- c) / maxD;
}
