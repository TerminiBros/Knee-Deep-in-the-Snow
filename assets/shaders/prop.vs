#version 100

// Input vertex attributes
attribute vec3 vertexPosition;
attribute vec2 vertexTexCoord;
attribute vec3 vertexNormal;
attribute vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;

// Output vertex attributes (to fragment shader)
varying vec2 fragTexCoord;
varying vec4 fragColor;

// NOTE: Add here your custom variables
uniform sampler2D texLightmap;

void main()
{
    // Send vertex attributes to fragment shader
    fragTexCoord = vertexTexCoord;
    //fragColor = vertexColor;

    vec2 coords = vec2((128.0 + vertexPosition.x) / 256.0, (128.0 + vertexPosition.z) / 256.0); 
    fragColor = vec4( texture2D(texLightmap, vec2( coords.x, 1.0 - coords.y )).rgb, 1.0 );
    
    // Calculate final vertex position
    gl_Position = mvp*vec4(vertexPosition, 1.0);
}