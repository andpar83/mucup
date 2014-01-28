#pragma once
#include <QAbstractButton>
#include <QPixmap>

namespace psv
{

class active_button : public QAbstractButton
{
	Q_OBJECT
public:
	active_button(const QPixmap& normal, const QPixmap& active, const QPixmap& hover, QWidget* parent = nullptr);
	virtual ~active_button();

private:
	virtual void enterEvent(QEvent* e);
	virtual void leaveEvent(QEvent* e);
	virtual void mousePressEvent(QMouseEvent* e);
	virtual void mouseReleaseEvent(QMouseEvent* e);
	virtual void paintEvent(QPaintEvent* e);

	virtual QSize sizeHint() const;

private:
	void set_current(QPixmap& pix);

private:	
	QPixmap		normal_;
	QPixmap		active_;
	QPixmap		hover_;
	QPixmap*	current_;
};

}