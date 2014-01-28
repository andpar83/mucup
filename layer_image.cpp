#include "stdafx.h"
#include <algorithm>
#include <iterator>
#include "layer_image.h"

namespace psv
{

layer_image::layer_image(QWidget* parent) :
	QWidget(parent),
	selected_(false)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

layer_image::~layer_image()
{
}

QSize layer_image::sizeHint() const
{
	return QSize(150, 150);
}

void layer_image::set_layer_name(const QString& layer_name)
{
	if (layer_name != layer_name_)
	{		
		layer_name_ = layer_name;
		setToolTip(layer_name_);
		update();
	}
}

void layer_image::set_file_name(const QString& file_name)
{
	file_name_ = file_name;
}

QString layer_image::file_name() const 
{
	Q_ASSERT(!file_name_.isEmpty());
	return file_name_;
}

void layer_image::set_pixmap(const QPixmap& pixmap)
{
	pixmap_ = pixmap;
	if (pixmap.width() > icon_size || pixmap.height() > icon_size)
		icon_ = pixmap.scaled(icon_size, icon_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	else
		icon_ = pixmap;
	update();
}

void layer_image::set_selected(bool selected)
{
	if (selected_ != selected)
	{
		selected_ = selected;
		update();
	}
}

void layer_image::mouseDoubleClickEvent(QMouseEvent* e)
{
	if (!QDesktopServices::openUrl(QUrl::fromLocalFile(file_name())))
		QMessageBox::critical(this, tr("Failed to open file"), tr("Failed to open file"), QMessageBox::Ok);
}

void layer_image::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	if (hasFocus())
		painter.fillRect(QRect(0, 0, size().width(), size().height()), QBrush(palette().dark().color()));

	QPen pen(palette().text().color());
	pen.setWidth(selected_ ? 2 : 1);
	painter.setPen(pen);

	if (selected_)
		painter.drawRect(1, 1, size().width() - 2, size().height() - 2);
	else
		painter.drawRect(0, 0, size().width() - 1, size().height() - 1);
	painter.drawPixmap((size().width() - icon_.width()) / 2, (size().height() - icon_.height()) / 2, icon_);
}

QString layer_image::filesystem_layer_name() const
{
	auto name = layer_name();
	QChar incorrect_path_synbols[] = { '\\', '/', ':', '*', '?', '"', '<', '>', '|' };
	std::for_each
	(
		std::begin(incorrect_path_synbols),
		std::end(incorrect_path_synbols),
		[&name](const QChar& ch) { name.remove(ch); }
	);
	return name;
}

}
