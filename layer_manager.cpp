#include "stdafx.h"
#include "layer_image.h"
#include "layer_manager.h"

namespace psv
{

layer_manager::layer_manager(QWidget* parent) :
	QWidget(parent),
	layout_(new FlowLayout)
{
	QSizePolicy policy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	policy.setHeightForWidth(true);
	setSizePolicy(policy);

	setLayout(layout_);
	setFocusPolicy(Qt::StrongFocus);

	auto shortcut = new QShortcut(QKeySequence(tr("Ctrl+A")), this);
	connect(shortcut, SIGNAL(activated()), this, SLOT(select_all()));
}

layer_manager::~layer_manager()
{
}

void layer_manager::clear()
{
	auto layers = findChildren<layer_image*>();	
	foreach (auto item, layers)
		item->deleteLater();
	updateGeometry();
	update();
}

void layer_manager::add_layer(const QString& name, const QPixmap& pixmap)
{
	auto layer = new layer_image;	
	layer->set_layer_name(name);
	layer->set_pixmap(pixmap);
	layout_->addWidget(layer);
	updateGeometry();
	update();
}

int layer_manager::heightForWidth(int width) const
{
	return layout_->heightForWidth(width);
}

QSize layer_manager::sizeHint() const
{
	return layout_->sizeHint();
}

void layer_manager::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
		start_drag_ = e->pos();
	QWidget::mousePressEvent(e);
}

void layer_manager::mouseReleaseEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		if ((e->modifiers() & Qt::ControlModifier) == 0)
		{
			auto layers = findChildren<layer_image*>();
			foreach (auto item, layers)
				item->set_selected(false);
		}
		auto layer = qobject_cast<layer_image*>(childAt(e->pos()));
		if (layer)
			select_layer(layer, !layer->selected());
		update();
	}
	QWidget::mouseReleaseEvent(e);
}

void layer_manager::mouseMoveEvent(QMouseEvent* e)
{
	if (e->buttons() & Qt::LeftButton)
	{
		int distance = (e->pos() - start_drag_).manhattanLength();
		if (distance >= QApplication::startDragDistance())
			perform_drag();
	}
	QWidget::mouseMoveEvent(e);
}

void layer_manager::perform_drag()
{
	auto layers = selected_layers();
	if (layers.empty())
		return;

	QMimeData* mime_data = new QMimeData;
	QDrag* drag = new QDrag(this);
	drag->setMimeData(mime_data);

	QList<QUrl> urls;
	foreach(auto layer, layers)
	{
		if (layer->pixmap().save(layer->file_name(), "PNG", 0))
			urls.append(QUrl::fromLocalFile(layer->file_name()));
		else
			qCritical() << "Failed to save layer to file '" << layer->file_name() << "'";
	}
	mime_data->setUrls(urls);
	drag->setPixmap(get_layers_icon(layers));
	drag->exec(Qt::MoveAction | Qt::CopyAction, Qt::CopyAction);
	// If we remove file some applications can't open it because they try to open after drop was done.
	// For example, paint when there are unsaved changes
}

QVector<layer_image*> layer_manager::selected_layers() const
{
	QVector<layer_image*> result;
	auto layers = findChildren<layer_image*>();
	foreach (auto layer, layers)
	{
		if (layer->selected())
			result.append(layer);
	}
	return result;
}

void layer_manager::set_filenames() const
{
	auto layers = findChildren<layer_image*>();
	QMap<QString, size_t> cnt;
	foreach (auto layer, layers)
		++cnt[layer->filesystem_layer_name()];

	QMap<QString, size_t> used;

	foreach (auto layer, layers)
	{
		auto name = layer->filesystem_layer_name();
		auto path = QDir::tempPath() + QDir::separator() + name;
		if (cnt[name] > 1)
			path += QString("_%1").arg(++used[name]);
		path += ".png";
		layer->set_file_name(path);
	}
}

QPixmap layer_manager::get_layers_icon(QVector<layer_image*>& layers) const
{
	Q_ASSERT(!layers.empty());

	switch (layers.size())
	{
	case 1:
		return layers[0]->icon();

	case 2:
	case 3:
		{
			QPixmap result(layer_image::icon_size, layer_image::icon_size);
			result.fill(Qt::transparent);
			QPainter painter(&result);
			int h = layer_image::icon_size / 2;
			int w = layer_image::icon_size;
			painter.drawPixmap(0, 0, w, h, layers[0]->icon());
			painter.drawPixmap(0, h, w, h, layers[1]->icon());
			return result;
		}

	default:
		{
			QPixmap result(layer_image::icon_size, layer_image::icon_size);
			result.fill(Qt::transparent);
			QPainter painter(&result);
			int sz = layer_image::icon_size / 2;
			painter.drawPixmap(0, 0, sz, sz, layers[0]->icon());
			painter.drawPixmap(sz, 0, sz, sz, layers[1]->icon());
			painter.drawPixmap(0, sz, sz, sz, layers[2]->icon());
			painter.drawPixmap(sz, sz, sz, sz, layers[3]->icon());
			return result;
		}
	}	
}

void layer_manager::select_all()
{
	auto layers = findChildren<layer_image*>();
	foreach (auto layer, layers)
	{
		select_layer(layer, true);
	}
}

void layer_manager::select_layer(layer_image* layer, bool select)
{
	layer->set_selected(select);
	auto layers = selected_layers();
	if (layers.size() != 1)
		return;

	QMimeData* mimeData = new QMimeData();
	QByteArray data;
	QBuffer buffer(&data);
	buffer.open(QIODevice::WriteOnly);
	layers[0]->pixmap().save(&buffer, "PNG");
	buffer.close();
	mimeData->setData("PNG", data);
	mimeData->setImageData(layers[0]->pixmap());

	qApp->clipboard()->setMimeData(mimeData);
}

}
