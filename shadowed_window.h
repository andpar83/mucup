#pragma once
#include <QPixmap>
#include <QWidget>
#include <QRect>

namespace psv
{

class shadowed_window : public QWidget
{
	Q_OBJECT
public:
	shadowed_window(QWidget* parent = nullptr);
	virtual ~shadowed_window();
	void set_central_widget(QWidget* widget);
	
private:
	virtual void resizeEvent(QResizeEvent* e);
	virtual void paintEvent(QPaintEvent* e);
	virtual void mousePressEvent(QMouseEvent* e);

private:
	QRect child_area() const;
	void update_minimum_size();
	void bring_next_window(const QPoint& p);

private:
	QWidget*	widget_;

	QPixmap		top_;
	QPixmap		left_;
	QPixmap		right_;
	QPixmap		bottom_;
	QPixmap		top_left_;
	QPixmap		top_right_;
	QPixmap		bottom_left_;
	QPixmap		bottom_right_;
};

}