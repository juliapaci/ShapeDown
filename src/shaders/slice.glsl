uniform float z_slice;

void main() {
    float signed_distance = map(vec3(fragTexCoord, z_slice)).x;

    finalColor = vec4(vec3(signed_distance), 1.0);
}
