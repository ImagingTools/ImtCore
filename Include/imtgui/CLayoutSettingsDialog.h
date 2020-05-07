#pragma once

// Qt includes
#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtWidgets/QDialog>
#include <GeneratedFiles/imtgui/ui_CCustomLayoutSettings.h>

// ACF includes
#include <imtgui/ILayout.h>


namespace imtgui
{


/**
	Class defines hierarchical layouting as a widget.
*/
class CLayoutSettingsDialog: 
	public QDialog,
	public Ui::CCustomLayoutSettings
{
	Q_OBJECT

public:

	CLayoutSettingsDialog(QWidget* parentPtr = Q_NULLPTR);

	void SetLayoutProperties(const ILayout::LayoutProperties &properties);
	ILayout::LayoutProperties GetLayoutProperties() const;
	void SetBorderColor(const QColor &color);

protected Q_SLOTS:
	void OnBorderColorChanged();

private:
	QColor m_borderColor;
};



} //namespace imtgui


