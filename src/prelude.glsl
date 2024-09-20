// great video on ray marching: https://www.youtube.com/watch?v=TSAIR03FPfY
#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

uniform vec3 view_eye;
uniform vec3 view_center;
uniform vec2 resolution;

// TODO: maybe only write if we have an object selected?
uniform vec3 object_props[5];

// Output fragment color
out vec4 finalColor;

const float MAX_RAY_DIST = 256.0;
const float MAX_STEPS    = 128.0;
const float MIN_SDF_DIST = 0.001;
