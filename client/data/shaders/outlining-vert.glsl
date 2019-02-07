// just transform and nothing else

void main()
{	
	float scale= 1.00001;
	vec4 pos= gl_ModelViewProjectionMatrix * gl_Vertex;

	pos.w= pos.w*scale;

	gl_Position = pos;
	gl_FrontColor= gl_Color;
}
