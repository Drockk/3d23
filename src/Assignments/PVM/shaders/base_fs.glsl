#version 460

layout(location=0) out vec4 vFragColor;

 layout(std140, binding = 0) uniform Modifier {
 float strength;
 vec3  color;
};

void main() {
    vec3 out_color = color * strength;
    vFragColor = vec4(out_color, 1.0f);
}
