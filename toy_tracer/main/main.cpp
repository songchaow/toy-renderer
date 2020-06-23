#include <QtWidgets/QApplication>
#include <QThread>
#include "main/MainWindow.h"
#include "main/canvas.h"
#include "shape/rectangular.h"
#include "shape/flatcube.h"
#include "main/renderworker.h"
#include "main/ResourceManager.h"
#include "core/glTFLoader.h"
#include "engine/controller.h"
#include <thread>
using namespace std;

int main(int argc, char *argv[])
{
      QApplication a(argc, argv);
      MainWindow window;
      constexpr int window_height = 800;
      constexpr int window_width = 1280;
      Canvas* canvas = new Canvas(window_width, window_height);
      //Camera* cam = CreateRTCamera(Point2i(canvas->width(), canvas->height()));
      Matrix4 world2cam = LookAt({ 0,1,0 }, { 0, -1, -1 });
      Camera* cam = new Camera(world2cam, (Float)window_width / (Float)window_height, 90.f / 180.f*Pi, { 0, -1, -1 });
      
      //PointLight spot(RGBSpectrum(10.f, 10.f, 10.f), Point3f(), std::cos(Pi / 20.f), Vector3f(0.f, 0.f, 1.f));
      //cam->setAssociatedLight(spot);
      //window.winWidget = QWidget::createWindowContainer(canvas);
      window.show();
      canvas->show();
      //canvas->initialize(); // use RenderWorker's
      RenderWorker* worker = RenderWorker::Instance();
      worker->setCanvas(canvas);
      worker->setCamera(cam);
      /*QThread workerThread;
      worker->moveToThread(&workerThread);*/
      //QObject::connect(&workerThread, &QThread::started, worker, &RenderWorker::initialize);
      //worker->initialize();
      //QObject::connect(&workerThread, &QThread::started, worker, &RenderWorker::renderLoop);
      worker->skybox().loadSkybox();
      
#if 0
      // create albedo texture for balls
      Image* off_color = new Image(R8G8B8(25, 25, 25), false, 0.f);
      Image* on_color = new Image(R8G8B8(113, 206.f, 239.f), false, 0.f);
      Image* spec_color = new Image(R8G8B8(0.7f, 0.4f, 0.5f), false, 0.f);
      PBRMaterial m;
      {
            ImageTexture off_texture(on_color);
            ImageTexture specularRough(spec_color);

            m.albedo_map = off_texture;
            m.metallicRoughnessMap = specularRough;
            //m.globalEmission() = RGBSpectrum(0.1, 0.2, 0.1);
      }
      Primitive* cube = new Primitive(new FlatCube(), defaultMaterial, Transform::Identity());
      std::vector<Matrix4> obj2worlds = { TranslateM(-1, -1, 0), TranslateM(-1, 1, 0), TranslateM(1, -1, 0), TranslateM(1, 1, 0) };
      InstancedPrimitive* cubes = new InstancedPrimitive(new FlatCube(), defaultMaterial, obj2worlds);
      cubes->GenMeshes();
      RenderWorker::Instance()->loadObject(cubes);
      //RenderWorker::Instance()->loadPointLight(l);
#else
      std::vector<Primitive3D*> glTFPrimitives;
      //glTFPrimitives = LoadGLTF("model/DamagedHelmet/glTF/DamagedHelmet.gltf");
      //glTFPrimitives = LoadGLTF("model/Sponza/glTF/Sponza.gltf");
      //glTFPrimitives = LoadGLTF("model/pbrspheres/MetalRoughSpheres.gltf");
      //glTFPrimitives = LoadGLTF("C:/Users/songc/Codes/toy_tracer/msvc.build_x64/toy_tracer/model/FlightHelmet/FlightHelmet.gltf");
      //glTFPrimitives = LoadGLTF("C:/Users/songc/Codes/toy_tracer/msvc.build_x64/toy_tracer/model/player/scene.gltf");
      
      glTFPrimitives = LoadGLTF("model/player3/player.gltf");
      ImageTexture char2dAlbedo("texture/character/char2d.png");
      Primitive2D* char2d = new Primitive2D(Point3f(0, 0, 0), Point2f(0.5, 1), char2dAlbedo);
      cam->LookAt(char2d->pos() + Vector3f(2,2,2), Vector3f(-1,-1,-1));

      addKeyboardMoveControl(char2d, UP, DOWN, LEFT, RIGHT);
      //ResourceManager::getInstance()->loadFile("model/Sponza/glTF/Sponza.gltf");
      //ResourceManager::getInstance()->loadFile("model/untitled.gltf");
      std::string otherCharPaths[] = { "model/1/person.gltf","model/3/person.gltf","model/4/person.gltf" };
      //std::string otherCharPaths[] = { "model/4/person.gltf" };
      Float shift = 0.f;
      for (auto p : otherCharPaths) {
            std::vector<Primitive3D*> ps = LoadGLTF(p);
            for (auto* prim : ps) {
                  prim->moveAlong(Vector3f(1, 0, 0), shift);
                  RenderWorker::Instance()->loadCharacter(prim);
                  shift += 0.1f;
            }
      }

      RenderWorker::Instance()->loadObject(glTFPrimitives[1]);
      Primitive3D* duplicateChar = new Primitive3D(*glTFPrimitives[0]);
      duplicateChar->moveBackwardTick(0.1);
      RenderWorker::Instance()->loadCharacter(glTFPrimitives[0]);
      RenderWorker::Instance()->loadCharacter(duplicateChar);
      RenderWorker::Instance()->loadObject(char2d);
      MainWindow::getInstance()->refreshResource();
      //PointLight* l = new PointLight(RGBSpectrum(50.f, 50.f, 50.f), Point3f(0.f, 5.f, 0.f));
      //PointLight* l2 = new PointLight(RGBSpectrum(2000.f, 2000.f, 2000.f), Point3f(10.f, 0.f, -20.f));
      PointLight* l = new PointLight(RGBSpectrum(1, 1, 1), Vector3f(0.f, -5.f, 16.f));
      l->setLightSize(0.01);
      //RenderWorker::Instance()->loadPointLight(l);
      MainWindow::getInstance()->addPointLight(l);
#endif
      //workerThread.start();
      std::thread renderThread(&RenderWorker::start, worker);
      a.exec();
	return 0;
}
