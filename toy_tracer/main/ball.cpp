#include <QtWidgets/QApplication>
#include <QThread>
#include "main/canvas.h"
#include "main/renderworker.h"
using namespace std;

int main(int argc, char *argv[])
{
      QApplication a(argc, argv);
      Canvas* canvas = new Canvas();
      Camera* cam = CreateRTCamera(Point2i(canvas->width(), canvas->height()));
      //window.winWidget = QWidget::createWindowContainer(canvas);
      canvas->setControlCamera();
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
      // create albedo texture for balls
      Image* off_color = Image::CreateColorImage("grey", R8G8B8(25, 25, 25), false);
      Image* on_color = Image::CreateColorImage("yellow", R8G8B8(113, 206.f, 239.f), false);
      Image* spec_color = Image::CreateColorImage("black", R8G8B8(0.f, 0.f, 0.f), false);
      PBRMaterial m;
      {
            ImageTexture off_texture(off_color);
            ImageTexture specular(spec_color);
            ImageTexture roughness(off_color);
            //ImageTexture on_texture(on_color);
            
            m.albedo_map = off_texture;
            m.metallic_map = specular;
            m.rough_map = roughness;
      }
      
      constexpr int BALL_LEN = 8;
      constexpr Float BALL_DISTANCE = 3.f;
      // camera position
      while (!RenderWorker::Instance()->getCamera());
      RenderWorker::Instance()->getCamera()->LookAt();
      // add lights
      PointLight* pl_front = new PointLight(RGBSpectrum(2000.f, 2000.f, 2000.f), Point3f(0.f, 0.f, -25.f));
      PointLight* pl_back = new PointLight(RGBSpectrum(2000.f, 300.f, 2000.f), Point3f(0.f, 0.f, 25.f));
      PointLight* pl_left = new PointLight(RGBSpectrum(2000.f, 2000.f, 300.f), Point3f(-25.f, 0.f, 0.f));
      PointLight* pl_right = new PointLight(RGBSpectrum(300.f, 2000.f, 2000.f), Point3f(25.f, 0.f, 0.f));
      RenderWorker::Instance()->loadPointLight(pl_front);
      RenderWorker::Instance()->loadPointLight(pl_back);
      RenderWorker::Instance()->loadPointLight(pl_left);
      RenderWorker::Instance()->loadPointLight(pl_right);
      // draw ball
      Primitive* ball = new Primitive(new Sphere(), m);
      ball->GenMeshes();
      Point3f startPoint(-(BALL_LEN - 1)*BALL_DISTANCE / 2, -(BALL_LEN - 1)*BALL_DISTANCE / 2, -(BALL_LEN - 1)*BALL_DISTANCE / 2);
      for (int i = 0; i < BALL_LEN; i++) {
            for (int j = 0; j < BALL_LEN; j++) {
                  for (int k = 0; k < BALL_LEN; k++) {
                        Primitive* ledUnit = new Primitive(*ball);
                        ledUnit->obj2world() = SRT::fromTranslation(startPoint.x + i*BALL_DISTANCE, startPoint.y + j*BALL_DISTANCE, startPoint.z + k*BALL_DISTANCE);
                        RenderWorker::Instance()->loadObject(ledUnit);
                        if (i + j + k == 15)
                              ledUnit->getPBRMaterial()[0].globalEmission() = RGBSpectrum(0.2f, 0.25f, 0.01f);
                  }
            }
      }
      /*ball->getPBRMaterial()->globalEmission() = 0.5f;
      RenderWorker::Instance()->loadObject(ball);*/
      a.exec();
      return 0;
}
