// toy_tracer.cpp: 定义应用程序的入口点。
//

#include <QtWidgets/QApplication>
#include "main/MainWindow.h"
#include "main/canvas.h"
using namespace std;

int main(int argc, char *argv[])
{
      QApplication a(argc, argv);
      MainWindow window;
      Canvas* canvas = new Canvas();
      //window.winWidget = QWidget::createWindowContainer(canvas);
      window.show();
      canvas->show();
      canvas->initialize();
      a.exec();
	return 0;
}
