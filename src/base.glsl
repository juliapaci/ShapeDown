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

// https://iquilezles.org/articles/rmshadows/
float calcSoftshadow(in vec3 ro, in vec3 rd, float mint, float maxt, float w) {
    float res = 1.0;
    float t = mint;
    for(int i = 0; i < 16 && t < maxt; i++) {
        float h = map(ro + t*rd).x;
        res = min( res, h/(w*t) );
        t += clamp(h, 0.005, 0.50);
        if( res<-1.0 || t>maxt ) break;
    }
    res = max(res,-1.0);
    return 0.25*(1.0+res)*(1.0+res)*(2.0-res);
}

vec3 calcNormal(in vec3 pos) {
    vec2 e = vec2(1.0,-1.0)*0.5773*0.0005;
    return normalize(
        e.xyy*map( pos + e.xyy ).x +
        e.yyx*map( pos + e.yyx ).x +
        e.yxy*map( pos + e.yxy ).x +
        e.xxx*map( pos + e.xxx ).x
    );
}

float calcAO(in vec3 pos, in vec3 nor) {
    float occ = 0.0;
    float sca = 1.0;
    for(int i=0; i < 5; i++) {
        float hr = 0.01 + 0.12*float(i)/4.0;
        vec3 aopos =  nor * hr + pos;
        float dd = map( aopos ).x;
        occ += -(dd-hr)*sca;
        sca *= 0.95;
    }
    return clamp(1.0 - 3.0*occ, 0.0, 1.0);
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

vec3 render(in vec3 origin, in vec3 direction) {
    vec4 result = ray_march(origin, direction);
    vec3 colour = vec3(0.4, 0.5, 0.6) + direction.y*0.4;

    if(result.yzw.r > -0.5) {
        // lighting from raylib example
        vec3 pos = origin + result.x*direction;
        vec3 nor = calcNormal(pos);
        vec3 ref = reflect(direction, nor);

        // material
        colour = result.yzw;
        // colour = 0.45 + 0.35*sin(vec3(0.05,0.08,0.10)*(result.yzw.r-1.0));

        // lighting
        float occ = calcAO(pos, nor);
        vec3  lig = normalize(vec3(cos(-0.4), sin(0.7), -0.6));
        vec3  hal = normalize(lig-direction);
        float amb = clamp(0.5+0.5*nor.y, 0.0, 1.0);
        float dif = clamp(dot(nor, lig), 0.0, 1.0);
        float bac = clamp(dot(nor, normalize(vec3(-lig.x,0.0,-lig.z))), 0.0, 1.0)*clamp(1.0-pos.y,0.0,1.0);
        float dom = smoothstep(-0.1, 0.1, ref.y);
        float fre = pow(clamp(1.0+dot(nor,direction),0.0,1.0), 2.0);

        dif *= calcSoftshadow(pos, lig, 0.02, 2.5, 1./8.);
        dom *= calcSoftshadow(pos, ref, 0.02, 2.5, 1./8.);

        float spe = pow(clamp(dot(nor, hal), 0.0, 1.0),16.0) *
                    dif *
                    (0.04 + 0.96*pow(clamp(1.0+dot(hal,direction),0.0,1.0), 5.0));

        vec3 lin = vec3(0.0);
        lin += 1.30*dif*vec3(1.00,0.80,0.55);
        lin += 0.40*amb*vec3(0.40,0.60,1.00)*occ;
        lin += 0.50*dom*vec3(0.40,0.60,1.00)*occ;
        lin += 0.50*bac*vec3(0.25,0.25,0.25)*occ;
        lin += 0.25*fre*vec3(1.00,1.00,1.00)*occ;
        colour = colour*lin;
        colour += 10.00*spe*vec3(1.00,0.90,0.70);
    }

    return vec3(clamp(colour, 0.0, 1.0));
}

// thanks raylib raymarching example - Inigo Quilez
mat3 setCamera(in vec3 ro, in vec3 ta, float cr) {
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
