#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform float amplitude;
uniform float frequency;
uniform float time;
uniform vec2 scroll;
uniform int oscillateH;
uniform int oscillateV;
uniform int interleaveH;
uniform int interleaveV;

// Output fragment color
out vec4 finalColor;

void main() {
    float distortion = amplitude * sin(frequency * fragTexCoord.y + time);

    vec2 dist = vec2(oscillateH * distortion, oscillateV * distortion);
    dist *= (interleaveH * mod(gl_FragCoord.y - 0.5, 2.0) == 1.0) ? -1.0 : 1.0;
    dist *= (interleaveV * mod(gl_FragCoord.x - 0.5, 2.0) == 1.0) ? -1.0 : 1.0;

    vec4 texelColor = texture(texture0, fragTexCoord + dist + scroll);

    finalColor = vec4(texelColor.rgb * fragColor.rgb, texelColor.a);
}

