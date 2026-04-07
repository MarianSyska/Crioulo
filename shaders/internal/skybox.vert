#version 450 core

layout (location = 0) in vec3 aPos;

layout (std140) uniform TransformMatrices {
    mat4 view;
    mat4 projection;
};

out vec3 TexCoords;

void main()
{
    TexCoords = aPos;
    mat4 staticView = mat4(mat3(view));
    gl_Position = projection * staticView * vec4(aPos, 1.0);
}  