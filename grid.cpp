#include "grid.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glut.h>

int Grid::isContainedAt(const Vec3Df & pos){
	//returns index that contains the position
	float cellsize = size / r;
	Vec3Df relativepos = pos - origin;
	int index = 0;
	for (int x = 0; x < r; x++) {
		if (origin[0] + x*cellsize >= pos[0]){
			index += x;
			break;
		}
	}
	for (int y = 0; y < r; y++) {
		if (origin[1] + y*cellsize >= pos[1]){
			index += y*r;
			break;
		}
	}
	for (int z = 0; z < r; z++) {
		if (origin[2] + z*cellsize >= pos[2]){
			index += z*r*r;
			break;
		}
	}
	return index;
}

void Grid::putVertices(const std::vector<Vertex> & vertices){
    //put vertices in the corresponding voxels.
	for (int i = 0; i < vertices.size(); i++) {
		Vec3Df pos = vertices[i].p;
		addToCell(pos);		
	}
}

void Grid::addToCell(const Vec3Df & vertexPos){
	float cellsize = size / r;
	Vec3Df relativepos = vertexPos - origin;
	int index = 0;
	for (int x = 0; x < r; x++) {
		if (origin[0] + x*cellsize >= vertexPos[0]){
			index += x;
			break;
		}
	}
	for (int y = 0; y < r; y++) {
		if (origin[1] + y*cellsize >= vertexPos[1]){
			index += y*r;
			break;
		}
	}
	for (int z = 0; z < r; z++) {
		if (origin[2] + z*cellsize >= vertexPos[2]){
			index += z*r*r;
			break;
		}
	}
	verticesInCell[index].push_back(vertexPos);
}

void Grid::computeRepresentatives() {
	int count = 0;
	for (std::map<unsigned, std::vector<Vec3Df>>::iterator iter = verticesInCell.begin(); iter != verticesInCell.end(); ++iter) {
		count++;
		std::vector<Vec3Df> v = iter->second;
		int index = iter->first;
		Vec3Df total;
		for (int i = 0; i < v.size(); i++) {
			total += v[i];
		}
		Vec3Df finalpos = total / v.size();
		representatives[index] = Vertex(finalpos);
	}
	std::cout << count;
}

Vec3Df Grid::getRepresentative(const Vec3Df & pos) {
	int index = isContainedAt(pos);
	return representatives[index].p;
}

void Grid::drawCell(const Vec3Df & Min,const Vec3Df& Max) {

    const Vec3Df corners[8] =
    {
        Vec3Df(Min[0],Min[1],Min[2]),
        Vec3Df(Max[0],Min[1],Min[2]),
        Vec3Df(Min[0],Max[1],Min[2]),
        Vec3Df(Max[0],Max[1],Min[2]),
        Vec3Df(Min[0],Min[1],Max[2]),
        Vec3Df(Max[0],Min[1],Max[2]),
        Vec3Df(Min[0],Max[1],Max[2]),
        Vec3Df(Max[0],Max[1],Max[2])
    };


    static const unsigned short faceCorners[6][4] =
    {
        { 0,4,6,2 },
        { 5,1,3,7 },
        { 0,1,5,4 },
        { 3,2,6,7 },
        { 0,2,3,1 },
        { 6,4,5,7 }
    };

    glBegin(GL_QUADS);
    for (unsigned short f=0;f<6;++f)
    {
        const unsigned short* face = faceCorners[f];
        for(unsigned int v = 0; v<4; v++)
            glVertex3f(corners[face[v]][0], corners[face[v]][1], corners[face[v]][2]);

    }
    glEnd();

}

void Grid::drawGrid(){
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glLineWidth(1.0f);
    glColor3f(1.0f,1.0f,0.0f);
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    //Complete this function by calling draw cell several times.
	for (float x = origin[0]; x < origin[0] + size; x += size/r) {
		for (float y = origin[1]; y < origin[1] + size; y += size/r) {
			for (float z = origin[2]; z < origin[2] + size; z += size/r) {
				drawCell(Vec3Df(x , y , z), Vec3Df(x+1*size/r, y+1*size/r,z+1*size/r));
			}
		}
	}
    glPopAttrib();
}


