#include <imtgui/CLayoutSettingsDialog.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtGui/QPixmap>
#include <QtWidgets/QColorDialog>


// ACF includes


namespace imtgui
{


// public methods

CLayoutSettingsDialog::CLayoutSettingsDialog(QWidget* parentPtr)
	:QDialog(parentPtr)
{
	setupUi(this);
	connect(BorderColor, &QToolButton::clicked, this, &CLayoutSettingsDialog::OnBorderColorChanged);

}


void CLayoutSettingsDialog::SetLayoutProperties(const ILayout::LayoutProperties &properties)
{
	FixedLayouts->setChecked(properties.isFixedLayout);
	ViewBox->setChecked(properties.isShowBox);
	SetBorderColor(properties.borderColor);
	QString string;
	string.setNum(properties.minWidth);
	MinWidth->setText(string);
	string.setNum(properties.maxWidth);
	MaxWidth->setText(string);
	string.setNum(properties.minHeight);
	MinHeight->setText(string);
	string.setNum(properties.maxHeight);
	MaxHeight->setText(string);
}


ILayout::LayoutProperties CLayoutSettingsDialog::GetLayoutProperties() const
{
	ILayout::LayoutProperties retVal;
	retVal.isFixedLayout = FixedLayouts->isChecked();
	retVal.isShowBox = ViewBox->isChecked();
	retVal.borderColor = m_borderColor;
	retVal.minWidth = MinWidth->text().toInt();
	retVal.maxWidth = MaxWidth->text().toInt();
	retVal.minHeight = MinHeight->text().toInt();
	retVal.maxHeight = MaxHeight->text().toInt();
	return retVal;
}


void CLayoutSettingsDialog::SetBorderColor(const QColor &color)
{
	QPixmap pixmap(20, 20);
	QPainter painter(&pixmap);
	painter.fillRect(0, 0, 20, 20, color);
	BorderColor->setIcon(pixmap);
	m_borderColor = color;
}


void CLayoutSettingsDialog::OnBorderColorChanged()
{
	//const QColorDialog::ColorDialogOptions options = QFlag(colorDialogOptionsWidget->value());
	const QColor color = QColorDialog::getColor(m_borderColor, this, "Select Color");

	if (color.isValid()){
		SetBorderColor(color);
	}
}


} // namespace imtgui


