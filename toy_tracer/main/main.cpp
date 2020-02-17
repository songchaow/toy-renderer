#include <QtWidgets/QApplication>
#include <QThread>
#include "main/MainWindow.h"
#include "main/canvas.h"
#include "shape/rectangular.h"
#include "shape/flatcube.h"
#include "main/renderworker.h"
#include "main/ResourceManager.h"
#include "core/glTFLoader.h"
using namespace std;

int main(int argc, char *argv[])
{
      QApplication a(argc, argv);
      MainWindow window;
      constexpr int window_height = 800;
      constexpr int window_width = 1280;
      Canvas* canvas = new Canvas(window_width, window_height);
      Camera* cam = CreateRTCamera(Point2i(canvas->width(), canvas->height()));
      //PointLight spot(RGBSpectrum(10.f, 10.f, 10.f), Point3f(), std::cos(Pi / 20.f), Vector3f(0.f, 0.f, 1.f));
      //cam->setAssociatedLight(spot);
      //window.winWidget = QWidget::createWindowContainer(canvas);
      window.show();
      canvas->show();
      //canvas->initialize(); // use RenderWorker's
      RenderWorker* worker = RenderWorker::Instance();
      worker->setCanvas(canvas);
      worker->setCamera(cam);
      QThread workerThread;
      worker->moveToThread(&workerThread);
      QObject::connect(&workerThread, &QThread::started, worker, &RenderWorker::initialize);
      QObject::connect(&workerThread, &QThread::started, worker, &RenderWorker::renderLoop);
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
      std::vector<Primitive*> glTFPrimitives;
      //glTFPrimitives = LoadGLTF("model/DamagedHelmet/glTF/DamagedHelmet.gltf");
      //glTFPrimitives = LoadGLTF("model/Sponza/glTF/Sponza.gltf");
      ResourceManager::getInstance()->loadFile("model/Sponza/glTF/Sponza.gltf");
      //PointLight* l = new PointLight(RGBSpectrum(50.f, 50.f, 50.f), Point3f(0.f, 5.f, 0.f));
      PointLight* l2 = new PointLight(RGBSpectrum(2000.f, 2000.f, 2000.f), Point3f(10.f, 0.f, -20.f));
      
      //RenderWorker::Instance()->loadPointLight(l);
      MainWindow::getInstance()->addPointLight(l2);
#endif
      workerThread.start();
      a.exec();
	return 0;
}
