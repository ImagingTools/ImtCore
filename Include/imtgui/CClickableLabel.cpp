#pragma once
#include <imtgui/CClickableLabel.h>

CClickableLabel::CClickableLabel(QWidget* parent) : QLabel(parent)
{
}

void CClickableLabel::mousePressEvent(QMouseEvent* event)
{
	emit clicked();
}
