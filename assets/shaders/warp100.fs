#version 100

precision mediump float;

// Input vertex attributes (from vertex shader)
varying vec2 fragTexCoord;
varying vec4 fragColor;

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

void main() {
    float distortion = amplitude * sin(frequency * fragTexCoord.y + time);

    vec2 dist = vec2(float(oscillateH) * distortion, float(oscillateV) * distortion);
    dist *= (float(interleaveH) * mod(gl_FragCoord.y - 0.5, 2.0) == 1.0) ? -1.0 : 1.0;
    dist *= (float(interleaveV) * mod(gl_FragCoord.x - 0.5, 2.0) == 1.0) ? -1.0 : 1.0;

    vec4 texelColor = texture2D(texture0, fragTexCoord + dist + scroll, 10.0);

    gl_FragColor = vec4(texelColor.rgb * fragColor.rgb, texelColor.a);
}

