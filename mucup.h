#pragma once
#include <QLabel>
#include "movable_resizable_widget.h"
#include "active_button.h"
#include "layer_manager.h"

namespace psv
{

class mucup : public movable_resizable_widget
{
	Q_OBJECT

public:
	mucup(QWidget* parent = nullptr);
	virtual ~mucup();

private slots:
	void show_maximized();
	void show_normal();

private:
	virtual void resizeEvent(QResizeEvent* e);
	virtual void paintEvent(QPaintEvent* e);
	virtual void dragEnterEvent(QDragEnterEvent* e);
	virtual void dropEvent(QDropEvent* e);

private:
	static const int corner_radius = 5;
	static const int buttons_margin = 23;
	static const int buttons_padding = 1;

private:
	QLabel*				logo_;
	active_button*		minimize_button_;
	active_button*		maximize_button_;
	active_button*		show_normal_button_;
	active_button*		close_button_;
	QScrollArea*		layers_area_;
	layer_manager*		layers_;
};

}
