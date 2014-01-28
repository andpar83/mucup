#pragma once
#include <QPoint>
#include <QRect>
#include <QWidget>

namespace psv
{

class movable_resizable_widget : public QWidget
{
public:
	movable_resizable_widget(QWidget* parent = nullptr);
	virtual ~movable_resizable_widget();

private:
	virtual void enterEvent(QEvent* e);
	virtual void leaveEvent(QEvent* e);
	virtual void mousePressEvent(QMouseEvent* e);
	virtual void mouseMoveEvent(QMouseEvent* e);
	virtual void mouseReleaseEvent(QMouseEvent* e);

private:
	static const int resize_area_size = 3;

	enum resizing_direction
	{
		rd_no_resize,

		rd_right,
		rd_bottom,
		rd_right_bottom
	};

	resizing_direction get_resizing_direction(const QPoint& point);
	void set_resizing_cursor(resizing_direction direction);
	void make_move_or_resize(resizing_direction direction, const QPoint& diff);

private:
	QPoint				mouse_position_;
	bool				captured_;
	resizing_direction	direction_;
};

}