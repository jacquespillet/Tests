 //attribs
#version 440
layout(location = 0) in vec3 position;
out vec3 posValue;
//main
void main()
{
    posValue = position;
}