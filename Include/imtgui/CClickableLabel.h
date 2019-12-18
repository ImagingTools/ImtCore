#pragma once
#include <QtWidgets/QLabel>

class CClickableLabel: public QLabel
{
	Q_OBJECT
public:
	explicit CClickableLabel(QWidget* parent = 0);

signals:
	void clicked();

protected:
	void mousePressEvent(QMouseEvent* event);
};