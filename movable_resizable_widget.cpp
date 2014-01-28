#include "stdafx.h"
#include "movable_resizable_widget.h"

namespace psv
{

movable_resizable_widget::movable_resizable_widget(QWidget* parent) :
	QWidget(parent),
	captured_(false),
	direction_(rd_no_resize)
{
	setMouseTracking(true);
}

movable_resizable_widget::~movable_resizable_widget()
{
}

void movable_resizable_widget::enterEvent(QEvent* e)
{
	captured_ = false;
	unsetCursor();
}

void movable_resizable_widget::leaveEvent(QEvent* e)
{
	captured_ = false;
	unsetCursor();
}

void movable_resizable_widget::mousePressEvent(QMouseEvent* e)
{
	QWidget::mousePressEvent(e);
	if (e->button() != Qt::LeftButton || childAt(e->pos()))
		// If there is any widget here, ignore this click
		return;

	captured_ = true;
	direction_ = get_resizing_direction(e->pos());
	set_resizing_cursor(direction_);
	mouse_position_ = e->globalPos();
}

void movable_resizable_widget::mouseMoveEvent(QMouseEvent* e)
{
	QWidget::mouseMoveEvent(e);

	if (captured_ && (e->buttons() & Qt::LeftButton))
	{
		make_move_or_resize(direction_, (e->globalPos() - mouse_position_));
		mouse_position_ = e->globalPos();
	}
	else
	{
		captured_ = false;
		set_resizing_cursor(get_resizing_direction(e->pos()));
	}
}

void movable_resizable_widget::mouseReleaseEvent(QMouseEvent* e)
{
	QWidget::mouseReleaseEvent(e);
	if (e->button() != Qt::LeftButton)
		return;
	captured_ = false;
	set_resizing_cursor(get_resizing_direction(e->pos()));
}

movable_resizable_widget::resizing_direction movable_resizable_widget::get_resizing_direction(const QPoint& point)
{	
	auto x_end = width();
	auto x_start = x_end - resize_area_size;
	auto y_end = height();
	auto y_start = y_end - resize_area_size;

	if (point.x() >= x_start && point.x() <= x_end && point.y() >= y_start && point.y() <= y_end)
		return rd_right_bottom;
	else if (point.x() >= x_start && point.x() <= x_end && point.y() < y_start)
		return rd_right;	
	else if (point.y() >= y_start && point.y() <= y_end && point.x() < x_start)	
		return rd_bottom;
	else
		return rd_no_resize;
}

void movable_resizable_widget::set_resizing_cursor(resizing_direction direction)
{
	switch (direction)
	{
	case rd_right_bottom:
		setCursor(Qt::SizeFDiagCursor);
		break;
	case rd_right:
		setCursor(Qt::SizeHorCursor);
		break;
	case rd_bottom:
		setCursor(Qt::SizeVerCursor);
		break;
	default:
		unsetCursor();
		break;
	}
}

void movable_resizable_widget::make_move_or_resize(resizing_direction direction, const QPoint& diff)
{
	switch (direction)
	{
	case rd_right_bottom:
		window()->resize(window()->width() + diff.x(), window()->height() + diff.y());
		break;

	case rd_right:
		window()->resize(window()->width() + diff.x(), window()->height());
		break;

	case rd_bottom:
		window()->resize(window()->width(), window()->height() + diff.y());
		break;

	default:
		window()->move(window()->pos() + diff);
		break;
	}
}

}