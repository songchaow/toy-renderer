# version 330 core
uniform mat4 obj2world;
uniform mat4 world2cam;
uniform mat4 cam2ndc;

void main() {
      gl_Position = cam2ndc * world2cam * obj2world * vec4(0,0,0,1);
      
}