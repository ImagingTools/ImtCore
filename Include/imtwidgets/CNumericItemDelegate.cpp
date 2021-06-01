#include <imtwidgets/CNumericItemDelegate.h>


// Qt includes
#include <QtGui/QPainter>


namespace imtwidgets
{


CNumericItemDelegate::CNumericItemDelegate(Type type, QObject *parent)
	: QStyledItemDelegate(parent)
{
	m_type = type;

	SetMaximum(9999999);
	SetMinimum(0);
	SetDecimals(2);
	SetButtonSymbols(QAbstractSpinBox::UpDownArrows);
}


CNumericItemDelegate::~CNumericItemDelegate()
{
}


void CNumericItemDelegate::SetButtonSymbols(QAbstractSpinBox::ButtonSymbols bs)
{
	m_buttonSimbols = bs;
}


void CNumericItemDelegate::SetMaximum(int max)
{
	m_max = max;
}


void CNumericItemDelegate::SetMinimum(int min)
{
	m_min = min;
}


void CNumericItemDelegate::SetDecimals(int decimal)
{
	m_decimal = decimal;
}


// reimplemented (QStyledItemDelegate)

QWidget *CNumericItemDelegate::createEditor(
			QWidget*parent,
			const QStyleOptionViewItem& option,
			const QModelIndex&) const
{
	if (m_type == CNumericItemDelegate::SpinInt){
		QSpinBox *editor = new QSpinBox(parent);
		editor->setMaximum(m_max);
		editor->setMinimum(m_min);
		editor->setButtonSymbols(m_buttonSimbols);
		editor->setGeometry(option.rect);

		return editor;
	}
	else if (m_type == CNumericItemDelegate::SpinDouble){
		QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
		editor->setMaximum(m_max);
		editor->setMinimum(m_min);
		editor->setDecimals(m_decimal);
		editor->setButtonSymbols(m_buttonSimbols);
		editor->setGeometry(option.rect);

		return editor;
	}

	return nullptr;
}


void CNumericItemDelegate::setEditorData(QWidget*editor, const QModelIndex& index) const
{
	if (m_type == CNumericItemDelegate::SpinInt){
		int value = index.model()->data(index, Qt::DisplayRole).toInt();
		QSpinBox *spinbox = qobject_cast<QSpinBox*>(editor);
		spinbox->setValue(value);
	}
	else if (m_type == CNumericItemDelegate::SpinDouble){
		double value = index.model()->data(index, Qt::DisplayRole).toDouble();
		QDoubleSpinBox *spinbox = qobject_cast<QDoubleSpinBox*>(editor);
		spinbox->setValue(value);
	}
}


void CNumericItemDelegate::setModelData(QWidget* editor, QAbstractItemModel*model, const QModelIndex &index) const
{
	if (!model || !index.isValid()) return;
	if (m_type == CNumericItemDelegate::SpinInt){
		QSpinBox *spinbox = qobject_cast<QSpinBox*>(editor);
		int value = spinbox->value();
		model->setData(index, value, Qt::EditRole);
	}
	else if (m_type == CNumericItemDelegate::SpinDouble){
		QDoubleSpinBox *spinbox = qobject_cast<QDoubleSpinBox*>(editor);
		double value = spinbox->value();
		model->setData(index, value, Qt::EditRole);
	}
	else
		return;
}


void CNumericItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& ) const
{
	editor->setGeometry(option.rect);
}


} // namespace imtwidgets


