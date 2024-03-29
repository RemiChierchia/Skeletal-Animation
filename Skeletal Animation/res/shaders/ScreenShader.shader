#shader vertex
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texFramebuffer;

void main()
{
    //vec3 col = texture(texFramebuffer, TexCoords).rgb;
    FragColor = texture(texFramebuffer, TexCoords);
    //FragColor = vec4(col, 1.0);
}