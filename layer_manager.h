#pragma once
#include <QWidget>
#include <QString>
#include <QPixmap>
#include <QVector>
#include <QPair>
#include "flow_layout.h"
#include "layer_image.h"

namespace psv
{

class layer_manager : public QWidget
{
	Q_OBJECT

public:
	layer_manager(QWidget* parent = nullptr);
	virtual ~layer_manager();

	virtual int heightForWidth(int width) const;
	virtual QSize sizeHint() const;

	void clear();
	void add_layer(const QString& name, const QPixmap& pixmap);
	void set_filenames() const;

private slots:
	void select_all();

private:
	virtual void mousePressEvent(QMouseEvent* e);
	virtual void mouseMoveEvent(QMouseEvent* e);
	virtual void mouseReleaseEvent(QMouseEvent* e);

private:
	void perform_drag();
	QVector<layer_image*> selected_layers() const;
	QPixmap get_layers_icon(QVector<layer_image*>& layers) const;
	void select_layer(layer_image* layer, bool select);

private:
	FlowLayout*	layout_;
	QPoint		start_drag_;
};

}