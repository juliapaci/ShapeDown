// https://iquilezles.org/articles/distfunctions/
float sdf_round_box(vec3 p, vec3 b, float r) {
    vec3 q = abs(p) - b + r;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0) - r;
}

vec4 Min(vec4 a, vec4 b) {
    return (a.x<b.x) ? a : b;
}

// marches a ray in direction until hit
// NOTE: expects a normalized direction
vec4 ray_march(vec3 origin, vec3 direction) {
    float distance = 0.0;
    vec3 colour = vec3(-1);

    for(float i = 0.0; i < MAX_STEPS; i++) {
        vec3 pos = origin + (direction * distance);
        vec4 sdf = map(pos);

        if(sdf.x < MIN_SDF_DIST || distance > MAX_RAY_DIST) break;
        distance += sdf.x;
        colour = sdf.gba;
    }

    if(distance > MAX_RAY_DIST) colour = vec3(-1);
    return vec4(distance, colour);
}

vec3 render(vec3 origin, vec3 direction) {
    vec4 result = ray_march(origin, direction);
    vec3 colour = vec3(0.4, 0.5, 0.6) + direction.y*0.4;

    if(result.yzw.r > -0.5)
        colour = result.yzw;

    return vec3(clamp(colour, 0.0, 1.0));
}

// thanks raylib raymarching example - Inigo Quilez
mat3 setCamera( in vec3 ro, in vec3 ta, float cr ) {
    vec3 cw = normalize(ta-ro);
    vec3 cp = vec3(sin(cr), cos(cr),0.0);
    vec3 cu = normalize( cross(cw,cp) );
    vec3 cv = normalize( cross(cu,cw) );
    return mat3( cu, cv, cw );
}

void main() {
    vec2 pixel = (-resolution.xy + 2.0*gl_FragCoord.xy)/resolution.y;
    vec3 origin = view_eye;
    mat3 cam = setCamera(origin, view_center, 0.0);
    vec3 direction = cam * normalize(vec3(pixel.xy, 2.0));

    vec3 colour = render(origin, direction);
    finalColor = vec4(colour, 1.0);
}
