#include "stdafx.h"
#include "active_button.h"

namespace psv
{

active_button::active_button(const QPixmap& normal, const QPixmap& active, const QPixmap& hover, QWidget* parent) :
	QAbstractButton(parent),	
	normal_(normal),
	active_(active),
	hover_(hover),
	current_(&normal_)
{
	setMouseTracking(true);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	set_current(normal_);
}

active_button::~active_button()
{
}

void active_button::enterEvent(QEvent* e)
{
	set_current(hover_);
	QAbstractButton::enterEvent(e);
}

void active_button::leaveEvent(QEvent* e)
{
	set_current(normal_);
	QAbstractButton::leaveEvent(e);
}

void active_button::mousePressEvent(QMouseEvent* e)
{
	set_current(active_);
	QAbstractButton::mousePressEvent(e);
}

void active_button::mouseReleaseEvent(QMouseEvent* e)
{
	set_current(normal_);
	QAbstractButton::mouseReleaseEvent(e);
}

void active_button::set_current(QPixmap& pix)
{
	current_ = &pix;	
	resize(pix.size());
	updateGeometry();
	// For some unknown reasons if here is update and not repaint than paintEvent sometimes is not called
	// This happens in situation when minimized button is pressed, window minimezes, after reopens to normal
	// and buttons doesn't update on hover even if update is called
	repaint();	
}

void active_button::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.drawPixmap(0, 0, *current_);
}

QSize active_button::sizeHint() const
{
	return current_->size();
}

}
