// toy_tracer.cpp: 定义应用程序的入口点。
//

#include <QtWidgets/QApplication>
#include "MainWindow.h"
using namespace std;

int main(int argc, char *argv[])
{
      QApplication a(argc, argv);
      MainWindow window;
      window.show();
      a.exec();
	return 0;
}
