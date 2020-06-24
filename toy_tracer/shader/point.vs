# version 330 core
uniform mat4 obj2world;
uniform mat4 world2cam;
uniform mat4 cam2ndc;
uniform vec3 localPos = vec3(0,0,0);

void main() {
      gl_Position = cam2ndc * world2cam * obj2world * vec4(localPos,1);
      
}