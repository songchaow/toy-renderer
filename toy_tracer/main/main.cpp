// toy_tracer.cpp: 定义应用程序的入口点。
//

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
      PointLight spot(RGBSpectrum(10.f, 10.f, 10.f), Point3f(), std::cos(Pi / 20.f), Vector3f(0.f, 0.f, 1.f));
      cam->setAssociatedLight(spot);
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
      workerThread.start();
      
      // load ground mesh
      /*Image* roughness = Image::CreateColorImage("roughness", R8G8B8(128, 0, 0));
      Image* spec_color = Image::CreateColorImage("black", R8G8B8(0.f, 0.f, 0.f), false);
      ImageTexture ground_albedo("resources/gc_tex.png");
      ImageTexture rough(roughness);
      ImageTexture black(spec_color);
      PBRMaterial ground_m;
      ground_m.albedo_map = ground_albedo;
      ground_m.rough_map = rough;
      ground_m.metallic_map = black;
      Primitive* ground = CreatePrimitiveFromModelFile("resources/gc_tex_refined.obj");

      ground->setPBRMaterial(ground_m);
      RenderWorker::Instance()->loadObject(ground);*/

      a.exec();
	return 0;
}
