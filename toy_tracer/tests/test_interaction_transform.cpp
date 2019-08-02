#include "core/interaction.h"
#include "core/material.h"
#include "core/texture.h"
#include "core/primitive.h"
#include "core/tracer.h"
#include "core/camera.h"
#include "light/skybox.h"
#include <iostream>

int renderTest() {
      // Scene object initialization
      ConstColorTexture<RGBSpectrum>* skycolor = new ConstColorTexture<RGBSpectrum>(RGBSpectrum(255));
      Skybox skybox(skycolor);
      auto transform = Translate(Vector3f(10.f, 0.f, 0.f));
      Sphere* sphere_shape = new Sphere(3.5f, transform);
      Dielectric* d = new Dielectric();
      FlatSurface* glass_surface = new FlatSurface(d);
      SimpleMaterial* glass_material = new SimpleMaterial(glass_surface);
      Primitive* sphere_primitive = new Primitive(sphere_shape, glass_material);
      // Tracer
      Scene* scene = new Scene();
      PathTracer tracer(scene);
      scene->AddObj(sphere_primitive);
      // Camera
      Camera cam(scene, Translate(Vector3f(0, 0, -10)), Point2i(200, 200));
      RenderOption options;

      cam.Render();
}

int main() {
      Interaction i;
      Vector3f wo(0, 1, 0);
      Normal3f n(0.7071, 0.7071, 0);
      i.wo = wo;
      i.n = n;
      Vector3f localWo = i.GetLocalWo();
      // wo calculated by sample_f
      Dielectric* d = new Dielectric();
      FlatSurface* flat_m = new FlatSurface(d);
      Vector3f localWi;
      Spectrum reflect = flat_m->delta_f(localWo, localWi, Vector3f(0, 0, 1), nullptr, true);
      std::cout << "localWo:" << localWo.x << " " << localWo.y << " " << localWo.z << std::endl;
      Vector3f wi = i.GlobalDirection(localWi);
      return 0;
}