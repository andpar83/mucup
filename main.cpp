#include "stdafx.h"
#include "shadowed_window.h"
#include "mucup.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setWindowIcon(QIcon(":/images/icon.ico"));
	qApp->setStyleSheet
	(
		"* { "
			"background-color: white; "
		"} "
		"QScrollArea { "
			"margin: 0px 28px 0px 78px; "
		"} "
		"QScrollBar:vertical { "
			"border: 0px; "
			"background: white; "
			"width: 35px; "
		"} "
		"QScrollBar::handle:vertical { "
			"background: #D3D6E4; "
			"border-radius: 6px; "
			"width: 12px; "
			"min-height: 20px; "
			"margin-left: 23px;"
		"} "
		"QScrollBar::add-line:vertical { "
			"background: none; "
			"height: 0px; "
		"} "

		"QScrollBar::sub-line:vertical { "
			"background: none; "
			"height: 0px; "
		"} "
	);
	psv::shadowed_window w;
	w.set_central_widget(new psv::mucup(&w));
	w.show();
	return a.exec();
}
