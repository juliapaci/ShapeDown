// https://iquilezles.org/articles/distfunctions/
float sdf_round_box(vec3 p, vec3 b, float r) {
    vec3 q = abs(p) - b + r;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0) - r;
}

vec4 Min(vec4 a, vec4 b) {
    return (a.x<b.x) ? a : b;
}

// TODO: intersection?
// combination sdf stuff copied from shape up
vec4 opSmoothUnionSteppedColor(vec4 a, vec4 b, float blend) {
    float h =  max( blend-abs(a.x-b.x), 0.0 )/blend;
    float m = h*h*0.5;
    float s = m*blend*(1.0/2.0);
    return (a.x<b.x) ? vec4(a.x-s,a.gba) : vec4(b.x-s,b.gba);
}

vec4 opSmoothUnion(vec4 a, vec4 b, float blend) {
    float h =  max( blend-abs(a.x-b.x), 0.0 )/blend;
    float m = h*h*0.5;
    float s = m*blend*(1.0/2.0);
    return (a.x<b.x) ? vec4(a.x-s,mix(a.gba,b.gba,m)) : vec4(b.x-s,mix(a.gba,b.gba,1.0-m));
}

vec4 opSmoothSubtraction(vec4 d1, vec4 d2, float k) {
    float dist = opSmoothUnion(d1,vec4(-d2.x, d2.gba),k).x;
    return vec4(-dist, d2.gba);
}

// symmetry stuff copied from shape up
vec3 opSymX(vec3 p) {
    p.x = abs(p.x);
    return p;
}
vec3 opSymY(vec3 p) {
    p.y = abs(p.y);
    return p;
}
vec3 opSymZ(vec3 p) {
    p.z = abs(p.z);
    return p;
}
vec3 opSymXY(vec3 p) {
    p.xy = abs(p.xy);
    return p;
}
vec3 opSymXZ(vec3 p) {
    p.xz = abs(p.xz);
    return p;
}
vec3 opSymYZ(vec3 p) {
    p.yz = abs(p.yz);
    return p;
}
vec3 opSymXYZ(vec3 p) {
    p.xyz = abs(p.xyz);
    return p;
}
vec3 opRotateXYZ(vec3 p, vec3 theta) {
    float cz = cos(theta.z);
    float sz = sin(theta.z);
    float cy = cos(theta.y);
    float sy = sin(theta.y);
    float cx = cos(theta.x);
    float sx = sin(theta.x);

    mat3 mat = mat3(
        cz*cy,
        cz*sy*sx - cx*sz,
        sz*sx + cz*cx*sy,

        cy*sz,
        cz*cx + sz*sy*sx,
        cx*sz*sy - cz*sx,

        -sy,
        cy*sx,
        cy*cx
    );

    return mat*p;
}

// thanks raylib raymarching example - Inigo Quilez
mat3 setCamera(in vec3 ro, in vec3 ta, float cr) {
    vec3 cw = normalize(ta-ro);
    vec3 cp = vec3(sin(cr), cos(cr),0.0);
    vec3 cu = normalize(cross(cw,cp));
    vec3 cv = normalize(cross(cu,cw));
    return mat3(cu, cv, cw);
}

// marches a ray in direction until hit
// NOTE: expects a normalized direction
vec4 ray_march(in vec3 origin, in vec3 direction) {
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
