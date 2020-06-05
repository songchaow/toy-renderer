#include "core/interaction.h"
#include "core/material.h"
#include "core/texture.h"
#include "core/primitive.h"
#include "core/tracer.h"
#include "core/camera.h"
#include "light/skybox.h"
#include "texture/checkerboard.h"
#include <iostream>
#include <fstream>

int renderTest() {
      // Scene object initialization
      
      RGBSpectrumTexture2D* board_texture = new CheckerBoard(R8G8B8(255, 255, 255), R8G8B8(255, 0, 0), 40);
      ConstColorTexture<RGBSpectrum>* skycolor = new ConstColorTexture<RGBSpectrum>(RGBSpectrum(1.f, 1.f, 1.f));
      ImageTexture* sphereMap = new ImageTexture("sph2.jpg", ImageTexture::WrapMode::BLACK);
      Float eq_radius = sphereMap->width() / 2 / Pi;
      EquiRectSkybox* skybox = new EquiRectSkybox(sphereMap, eq_radius);
      auto transform = Translate(Vector3f(0, 0.f, 900.f));
      Sphere* sphere_shape = new Sphere(890.f, transform);
      Dielectric* d = new Dielectric();
      FlatSurface* glass_surface = new FlatSurface(d);
      SimpleMaterial* glass_material = new SimpleMaterial(glass_surface);
      Primitive3D* sphere_primitive = new Primitive3D(sphere_shape, glass_material);
      // Tracer
      Scene* scene = new Scene();
      scene->setSkybox(skybox);
      PathTracer tracer(scene);
      //scene->AddObj(sphere_primitive);
      // Camera
      Camera cam(scene, Translate(Vector3f(0, 0, -10.f)), Point2i(500, 500), 80.f * Pi / 180);
      RenderOption options;
      options.sample_per_pixel = 1024;
      cam.Render(options);
      const Film& film = cam.getFilm();
      film.writePNG("hopefully.png");
      std::ofstream test("text.txt");
      for (int i = 0; i < film.getWidth(); i++) {
            for (int j = 0; j < film.getHeight(); j++) {
                  const RGBSpectrum& res = film.ContribSum(Point2i(i, j));
                  test << res[0] << ' ' << res[1] << ' ' << res[2] << '|';
            }
            test << std::endl;
      }
      test.close();
      return 0;
}

int testSkybox() {
      return 0;
}

int main() {
      
      renderTest();

      return 0;
}