// toy_tracer.cpp: 定义应用程序的入口点。
//

#include <QtWidgets/QApplication>
#include "main/MainWindow.h"
#include "main/canvas.h"
#include "main/renderworker.h"
using namespace std;

int main(int argc, char *argv[])
{
      QApplication a(argc, argv);
      MainWindow window;
      Canvas* canvas = new Canvas();
      //window.winWidget = QWidget::createWindowContainer(canvas);
      window.show();
      canvas->show();
      //canvas->initialize(); // use RenderWorker's
      RenderWorker* worker = RenderWorker::Instance();
      worker->initialize(canvas);
      QThread workerThread;
      worker->moveToThread(&workerThread);
      QObject::connect(&workerThread, &QThread::started, worker, &RenderWorker::renderLoop);
      a.exec();
	return 0;
}
