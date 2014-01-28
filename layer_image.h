#pragma once
#include <QString>
#include <QWidget>
#include <QPixmap>
#include <QPoint>

namespace psv
{

class layer_image : public QWidget
{
	Q_OBJECT

public:
	layer_image(QWidget* parent = nullptr);
	~layer_image();

	void set_layer_name(const QString& layer_name);
	QString layer_name() const { return layer_name_; }
	QString filesystem_layer_name() const;

	void set_file_name(const QString& file_name);
	QString file_name() const;

	void set_pixmap(const QPixmap& pixmap);
	const QPixmap& pixmap() const { return pixmap_; }
	const QPixmap& icon() const { return icon_; }

	void set_selected(bool selected);
	bool selected() const { return selected_; }

	virtual QSize sizeHint() const;

	static const int icon_size = 128;

private:
	virtual void mouseDoubleClickEvent(QMouseEvent* e);
	virtual void paintEvent(QPaintEvent* e);

private:
	bool	selected_;
	QString layer_name_;
	QString file_name_;
	QPixmap pixmap_;
	QPixmap icon_;
	QPoint	start_drag_;
};

}