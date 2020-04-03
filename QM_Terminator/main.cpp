#include "stdafx.h"
#include "QM_Terminator.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QM_Terminator w;
	w.show();
	return a.exec();
}
