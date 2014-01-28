#include "stdafx.h"
#include <Windows.h>
#include "shadowed_window.h"

namespace psv
{

shadowed_window::shadowed_window(QWidget* parent) :
	QWidget(parent),
	widget_(nullptr),
	top_(":/images/shadow-top.png"),
	left_(":/images/shadow-left.png"),
	right_(":/images/shadow-right.png"),
	bottom_(":/images/shadow-bottom.png"),
	top_left_(":/images/shadow-top-left.png"),
	top_right_(":/images/shadow-top-right.png"),
	bottom_left_(":/images/shadow-bottom-left.png"),
	bottom_right_(":/images/shadow-bottom-right.png")
{
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_NativeWindow);
	update_minimum_size();

	setWindowTitle("Mucup");
	setWindowIcon(QIcon(":/images/icon.ico"));

	QSettings settings("Klaia Inc.", "Mucup");
	restoreGeometry(settings.value("geometry").toByteArray());
}

shadowed_window::~shadowed_window()
{
	QSettings settings("Klaia Inc.", "Mucup");
	settings.setValue("geometry", saveGeometry());
}

void shadowed_window::set_central_widget(QWidget* widget)
{
	Q_ASSERT(widget);
	if (widget_)
		delete widget_;
	widget_ = widget;
	widget_->setParent(this);
	widget_->setGeometry(child_area());
	update_minimum_size();
	update();
}

void shadowed_window::update_minimum_size()
{
	QSize sz(100, 100);
	if (!isMaximized())
		sz += QSize(top_left_.width() + top_right_.width(), top_left_.height() + bottom_left_.height());
	if (widget_)
		sz += widget_->minimumSize();
	setMinimumSize(sz);
	updateGeometry();
}

QRect shadowed_window::child_area() const
{
	QRect r = rect();
	if (!isMaximized())
		r.adjust(left_.width(), top_.height(), -right_.width(), -bottom_.height());
	return r;
}

void shadowed_window::resizeEvent(QResizeEvent* e)
{
	QWidget::resizeEvent(e);
	update_minimum_size();
	if (widget_)
		widget_->setGeometry(child_area());
	update();
}

void shadowed_window::paintEvent(QPaintEvent* e)
{
	if (isMaximized())
		// Do nothing when maximized because we don't need shadow
		return;

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::TextAntialiasing, true);

	for (int i = top_left_.width(); i < width() - top_right_.width(); i += top_.width())
		painter.drawPixmap(i, 0, top_);
	for (int i = top_left_.width(); i < width() - top_right_.width(); i += bottom_.width())
		painter.drawPixmap(i, height() - bottom_.height(), bottom_);
	for (int i = top_left_.height(); i < height() - bottom_left_.height(); i += left_.height())
		painter.drawPixmap(0, i, left_);
	for (int i = top_left_.height(); i < height() - bottom_left_.height(); i += right_.height())
		painter.drawPixmap(width() - right_.width(), i, right_);

	painter.drawPixmap(0, 0, top_left_);
	painter.drawPixmap(width() - top_right_.width(), 0, top_right_);
	painter.drawPixmap(0, height() - bottom_left_.height(), bottom_left_);	
	painter.drawPixmap(width() - bottom_right_.width(), height() - bottom_right_.height(), bottom_right_);
}

void shadowed_window::mousePressEvent(QMouseEvent* e)
{
	if (childAt(e->pos()))
		return;
	bring_next_window(e->globalPos());
}

void shadowed_window::bring_next_window(const QPoint& p)
{
	auto next = GetWindow(window()->winId(), GW_HWNDNEXT);
	while (next && next != window()->winId())
	{
		DWORD process = 0;
		GetWindowThreadProcessId(next, &process);
		if (GetCurrentProcessId() != process) // Skip our process
		{
			RECT rect;
			memset(&rect, 0, sizeof(rect));
			if (!GetWindowRect(next, &rect))
			{
				auto err = GetLastError();
				qWarning() << "Failed to get window rect. Error " << err;
				Q_ASSERT(false && "Failed to get window rect");
				return;
			}

			QRect qrect(QPoint(rect.left, rect.top), QPoint(rect.right, rect.bottom));
			if (qrect.contains(p))
			{
				if (!BringWindowToTop(next))
				{
					auto err = GetLastError();
					qWarning() << "Failed to bring window to top. Error " << err;
					Q_ASSERT(false && "Failed to bring window to top");
				}
				return;
			}
		}
		next = GetWindow(next, GW_HWNDNEXT);
	}
}

}