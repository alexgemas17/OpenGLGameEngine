#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;



//Salida para el fragment shader
out vec3 GFragPos;
out vec3 GVertexNormal;
out vec2 GTexCoords;
out mat3 GTBN;

// Entrada del vertex shader
in vec3 FragPos[];
in vec3 VertexNormal[];
in vec2 TexCoords[];
in mat3 TBN[];

struct FrustumPlanes{
    vec4 Plane;
};
uniform FrustumPlanes planes[6];	//view frustum planes


//check if the given point (p) is inside the plane. The code simply finds the point to plane distance
//if it is less than 0, the point is clearly below the plane
bool PointInFrustum(in vec3 p) {
	for(int i=0; i < 6; i++) 
	{
		vec4 plane = planes[i].Plane;
        double vAbsolute = abs( (plane.x*p.x) + (plane.y*p.y) + (plane.z*p.z) + plane.w );
        double raiz = sqrt( pow(plane.x,2) + pow(plane.y,2) + pow(plane.z,2) );
		//if ((dot(plane.xyz, p) + plane.w) < 0)
			//return false;
        if( (vAbsolute/raiz) < 0 )
            return false;
	}
	return true;
}

void main() {    

    //get the vertices
	for(int i=0; i<gl_in.length(); i++) 
    {
        vec4 vInPos = gl_in[i].gl_Position;
        gl_Position = vInPos;
        GFragPos = FragPos[i];
        GVertexNormal = VertexNormal[i];
        GTexCoords = TexCoords[i];
        GTBN = TBN[i];

        vec3 Point = vec3(vInPos.x, vInPos.y, vInPos.y);
        if(PointInFrustum(Point)) { 
			EmitVertex();		
		}
    }

    EndPrimitive();
} 