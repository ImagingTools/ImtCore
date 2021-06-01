#pragma once


// Qt includes
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStyledItemDelegate>


namespace imtwidgets
{


class CNumericItemDelegate: public QStyledItemDelegate
{
	Q_OBJECT
public:

	enum Type
	{
		SpinInt = QVariant::Int,
		SpinDouble = QVariant::Double,
	};

	CNumericItemDelegate(Type type = CNumericItemDelegate::SpinInt, QObject* parent = 0);
	virtual ~CNumericItemDelegate();

	void SetMaximum(int max);
	void SetMinimum(int min);
	void SetDecimals(int _decimal);
	void SetButtonSymbols(QAbstractSpinBox::ButtonSymbols bs);

	// reimplemented (QStyledItemDelegate)
	virtual QWidget* createEditor(
				QWidget* parent,
				const QStyleOptionViewItem& option,
				const QModelIndex& index) const override;
	virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	virtual void setModelData(QWidget* editor, QAbstractItemModel*model, const QModelIndex& index) const override;
	virtual void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

protected:
	QAbstractSpinBox::ButtonSymbols m_buttonSimbols;
	int m_max;
	int m_min;
	int m_decimal;
	Type m_type;
 };


} // namespace imtwidgets


