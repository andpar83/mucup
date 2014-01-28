#include "stdafx.h"
#include <string>
#include <QString>
#include <libpsd/include/libpsd.h>
#include "active_button.h"
#include "mucup.h"

namespace psv
{

mucup::mucup(QWidget* parent) :
	movable_resizable_widget(parent),
	logo_(new QLabel(this)),
	minimize_button_(nullptr),
	maximize_button_(nullptr),
	close_button_(nullptr),
	show_normal_button_(nullptr),
	layers_area_(nullptr),
	layers_(new layer_manager)
{	
	QPalette pal = palette();
	pal.setColor(backgroundRole(), Qt::white);
	setPalette(pal);

	setMinimumSize(100, 100);

	logo_->setPixmap(QPixmap(":/images/logo.png"));
	logo_->adjustSize();
	logo_->move(30, 24);

	setAcceptDrops(true);
	setMouseTracking(true);
	
	{
		minimize_button_ = new active_button
		(
			QPixmap(":/images/minimize-normal.png"),
			QPixmap(":/images/minimize-active.png"),
			QPixmap(":/images/minimize-hover.png"),
			this
		);
		connect(minimize_button_, SIGNAL(clicked()), window(), SLOT(showMinimized()));
	}

	{
		maximize_button_ = new active_button
		(
			QPixmap(":/images/maximize-normal.png"),
			QPixmap(":/images/maximize-active.png"),
			QPixmap(":/images/maximize-hover.png"),
			this
		);
		if (isMaximized())
			maximize_button_->hide();
		connect(maximize_button_, SIGNAL(clicked()), this, SLOT(show_maximized()));
	}

	{
		show_normal_button_ = new active_button
		(
			QPixmap(":/images/show-normal-normal.png"),
			QPixmap(":/images/show-normal-active.png"),
			QPixmap(":/images/show-normal-hover.png"),
			this
		);
		if (!isMaximized())
			show_normal_button_->hide();
		connect(show_normal_button_, SIGNAL(clicked()), this, SLOT(show_normal()));
	}

	{
		close_button_ = new active_button
		(
			QPixmap(":/images/close-normal.png"),
			QPixmap(":/images/close-active.png"),
			QPixmap(":/images/close-hover.png"),
			this
		);
		connect(close_button_, SIGNAL(clicked()), window(), SLOT(close()));
	}

	layers_area_ = new QScrollArea(this);
	layers_area_->setFrameShape(QFrame::NoFrame);
	layers_area_->setWidget(layers_);
	layers_area_->setWidgetResizable(true);
}

mucup::~mucup()
{
}

void mucup::dragEnterEvent(QDragEnterEvent* e)
{
	// Do not drag from itself
	if (!e->source() && e->mimeData()->hasFormat("text/uri-list"))
		e->acceptProposedAction();
}

void mucup::dropEvent(QDropEvent* e)
{
	try
	{
		QList<QUrl> urls = e->mimeData()->urls();
		if (urls.isEmpty())
			return;
		QString file_name = urls.first().toLocalFile();
		if (file_name.isEmpty())
			return;
		layers_->clear();
		window()->setWindowTitle(QString("Mucup - opening %1").arg(QDir::toNativeSeparators(file_name)));

		psd_context* context = nullptr;
		auto status = psd_image_load(&context, file_name.toLocal8Bit().data());
		if (status != psd_status_done || !context)
		{
			// Failed to load the file
			if (context)
				psd_image_free(context);
			window()->setWindowTitle("Mucup");
			QMessageBox::critical(this, tr("Error"), tr("Failed to open file"), QMessageBox::Ok);
			return;
		}

		{
			status = psd_image_blend(context, 0, 0, context->width, context->height);
			if (status != psd_status_done)
				QMessageBox::critical(this, tr("Error"), tr("Failed to blend file"), QMessageBox::Ok);
			else
			{
				QImage image
				(
					reinterpret_cast<const unsigned char*>(context->blending_image_data),
					context->width,
					context->height,
					QImage::Format_ARGB32
				);
				QPixmap pixmap = QPixmap::fromImage(image);
				layers_->add_layer("Blended", pixmap);
			}
		}

		for (size_t i = 0; i < context->layer_count; ++i)
		{
			const auto& layer = context->layer_records[i];

			QImage image(reinterpret_cast<const unsigned char*>(layer.image_data), layer.width, layer.height, QImage::Format_ARGB32);
			QPixmap pixmap = QPixmap::fromImage(image);
			std::wstring name;
			if (layer.unicode_name && layer.unicode_name_length)
			{
				for (size_t i = 0; (i < layer.unicode_name_length) && layer.unicode_name[i]; ++i)
				{
					//TODO endiannese problem
					psd_ushort ch = layer.unicode_name[i];
					ch = (ch << 8) | (ch >> 8);
					name.push_back(static_cast<wchar_t>(ch));
				}
			}			
			layers_->add_layer(QString::fromStdWString(name), pixmap);
		}
		layers_->set_filenames();
		psd_image_free(context);
		window()->setWindowTitle(QString("Mucup - %1").arg(QDir::toNativeSeparators(file_name)));
	}
	catch (const std::exception& e)
	{
		window()->setWindowTitle("Mucup");
		QMessageBox::critical(this, tr("Error"), tr("Failed to open file: \n%1").arg(e.what()), QMessageBox::Ok);
	}
}

void mucup::show_maximized()
{
	window()->showMaximized();
	show_normal_button_->show();
	maximize_button_->hide();	
}

void mucup::show_normal()
{
	window()->showNormal();
	show_normal_button_->hide();
	maximize_button_->show();
}

void mucup::resizeEvent(QResizeEvent* e)
{
	movable_resizable_widget::resizeEvent(e);

	int x = width() - 
	(
		minimize_button_->width() + buttons_padding +
		maximize_button_->width() + buttons_padding +
		close_button_->width() + buttons_margin
	);

	int y = buttons_margin;

	minimize_button_->move(x, y);
	maximize_button_->move(x + minimize_button_->width() + buttons_padding, y);
	show_normal_button_->move(x + minimize_button_->width() + buttons_padding, y);
	close_button_->move(x + minimize_button_->width() + buttons_padding + maximize_button_->width() + buttons_padding, y);

	auto layers_area_y = y + minimize_button_->height() + buttons_margin;
	layers_area_->setGeometry
	(
		corner_radius,
		layers_area_y,
		width() - (corner_radius + corner_radius),
		height() - (layers_area_y + corner_radius)
	);
	update();
}

void mucup::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::TextAntialiasing, true);

	QPainterPath path;
	path.addRoundedRect(rect(), corner_radius, corner_radius);
	painter.fillPath(path, Qt::white);
}

}