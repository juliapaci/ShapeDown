uniform vec3 view_eye;
uniform vec3 view_center;
uniform vec2 resolution;

void main() {
    vec2 pixel = (-resolution.xy + 2.0*gl_FragCoord.xy)/resolution.y;
    vec3 origin = view_eye;
    mat3 cam = setCamera(origin, view_center, 0.0);
    vec3 direction = cam * normalize(vec3(pixel.xy, 2.0));

    vec3 colour = ray_march(origin, direction).gba;
    finalColor = vec4(colour, 1.0);
}
