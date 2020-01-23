#include <QtWidgets/QApplication>
#include <QThread>
#include "main/MainWindow.h"
#include "main/canvas.h"
#include "main/renderworker.h"
#include "main/ResourceManager.h"
using namespace std;

int main(int argc, char *argv[])
{
      QApplication a(argc, argv);
      MainWindow window;
      Canvas* canvas = new Canvas();
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

      // create albedo texture for balls
      Image* off_color = Image::CreateColorImage("grey", R8G8B8(25, 25, 25), false);
      Image* on_color = Image::CreateColorImage("yellow", R8G8B8(113, 206.f, 239.f), false);
      Image* spec_color = Image::CreateColorImage("black", R8G8B8(0.f, 0.f, 0.f), false);
      PBRMaterial m;
      {
            ImageTexture off_texture(on_color);
            ImageTexture specular(spec_color);
            ImageTexture roughness(off_color);
            //ImageTexture on_texture(on_color);

            m.albedo_map = off_texture;
            m.metallic_map = specular;
            m.rough_map = roughness;
      }
      Primitive* ball = new Primitive(new Sphere(0.2f), m, Translate(0.f, 0.f, 1.f));
      ball->GenMeshes();
      //Primitive* ball2 = new Primitive(new Sphere(1.f), m, Translate(0.f, 0.f, 3.f));
      Primitive* ball2 = new Primitive(new Sphere(1.f), m, Translate(0.f, 0.f, 3.f));
      ball2->GenMeshes();
      PointLight* l = new PointLight(RGBSpectrum(50.f, 50.f, 50.f), Point3f(0.f, 0.f, -5.f));
      RenderWorker::Instance()->loadObject(ball);
      RenderWorker::Instance()->loadObject(ball2);
      RenderWorker::Instance()->loadPointLight(l);
      workerThread.start();
     

      a.exec();
	return 0;
}
