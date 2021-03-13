#include <imtgui/CHierarchicalLayoutWidget.h>


// Qt includes
#include <QtCore/QMimeData>
#include <QtCore/QUuid>
#include <QtGui/QPixmap>
#include <QtWidgets/QAction>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>

// ACF includes
#include <iser/IArchive.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <iser/CArchiveTag.h>
#include <istd/CChangeNotifier.h>
#include <iser/ISerializable.h>
#include <iser/CXmlStringWriteArchive.h>
#include <iser/CXmlStringReadArchive.h>


namespace imtgui
{


// public methods

CHierarchicalLayoutWidget::CHierarchicalLayoutWidget(QWidget* parentPtr)
	:QWidget(parentPtr),
	m_viewMode(VM_UNDEFINED),
	m_borderColor(QColor("gray")),
	m_isShowBox(true)
{
	setAutoFillBackground(true);
	SetViewMode(VM_NORMAL);
}


void CHierarchicalLayoutWidget::SetViewMode(ViewMode viewMode)
{
	m_viewMode = viewMode;

	IdsList keys = m_customWidgetMap.keys();
	for (QByteArray& key : keys){
		CCustomLayoutWidget* customWidgetPtr = m_customWidgetMap[key];
		customWidgetPtr->SetEditMode(viewMode == VM_EDIT);
	}
	update();
}


CHierarchicalLayoutWidget::ViewMode CHierarchicalLayoutWidget::GetViewMode()
{
	return m_viewMode;
}

void CHierarchicalLayoutWidget::ClearAll()
{
	CleanLayoutRecursive(layout());
	m_customWidgetMap.clear();
	delete layout();
}


// protected methods

void CHierarchicalLayoutWidget::SetAdditionalNames(QStringList& additionalNames)
{
	m_additionalNames = additionalNames;
}


void CHierarchicalLayoutWidget::CleanLayoutRecursive(QLayout* layoutPtr)
{
	if (layoutPtr != NULL){
		while (QLayoutItem* itemPtr = layoutPtr->takeAt(0)){
			QWidget* widgetPtr = itemPtr->widget();
			if (widgetPtr != NULL){
				widgetPtr->deleteLater();
			}

			QLayout* childLayoutPtr = itemPtr->layout();
			if (childLayoutPtr != NULL){
				CleanLayoutRecursive(childLayoutPtr);
			}

			delete itemPtr;
		}
	}
}


CCustomLayoutWidget* CHierarchicalLayoutWidget::createCustomWidget()
{
	QByteArray id = QUuid::createUuid().toByteArray();

	CCustomLayoutWidget* customLayoutWidgetPtr = new CCustomLayoutWidget(id, *this);
	m_customWidgetMap.insert(id, customLayoutWidgetPtr);

	return customLayoutWidgetPtr;
}


void CHierarchicalLayoutWidget::SetBorderColor(const QColor &color)
{
	m_borderColor = color;
}


QColor CHierarchicalLayoutWidget::GetBorderColor() const
{
	return m_borderColor;
}


void CHierarchicalLayoutWidget::SetBorderEnabled(bool isBorderEnabled)
{
	m_isShowBox = isBorderEnabled;
}


bool CHierarchicalLayoutWidget::IsBorderEnabled() const
{
	return m_isShowBox;
}


// public methods of class CCustomLayoutWidget

CCustomLayoutWidget::CCustomLayoutWidget(
			const QByteArray& id,
			CHierarchicalLayoutWidget& hierarchicalLayoutWidget,
			CCustomLayoutWidget* parentCustomWidgetPtr,
			QWidget* parentPtr)
	:QWidget(NULL),
	m_hierarchicalLayoutWidget(hierarchicalLayoutWidget),
	m_parentCustomWidgetPtr(NULL),
	m_id(id),
	m_externalWidgetPtr(NULL),
	m_titleAlign(ILayout::AT_LEFT)
{
	setupUi(this);

	SetEditMode(m_hierarchicalLayoutWidget.m_viewMode == CHierarchicalLayoutWidget::VM_EDIT);

	connect(SplitVerticaly, &QToolButton::clicked, this, &CCustomLayoutWidget::OnSplitVertical);
	connect(SplitHorizontaly, &QToolButton::clicked, this, &CCustomLayoutWidget::OnSplitHorizontal);
	connect(DeleteButton, &QToolButton::clicked, this, &CCustomLayoutWidget::OnDeleteWidget);
	connect(AlignLeft, &QToolButton::clicked, this, &CCustomLayoutWidget::OnAlignLeft);
	connect(AlignCenter, &QToolButton::clicked, this, &CCustomLayoutWidget::OnAlignCenter);
	connect(AlignRight, &QToolButton::clicked, this, &CCustomLayoutWidget::OnAlignRight);
	connect(ChangeIconButton, &QToolButton::clicked, this, &CCustomLayoutWidget::OnChangeIcon);
	connect(RemoveIcon, &QToolButton::clicked, this, &CCustomLayoutWidget::OnRemoveIcon);
	connect(Settings, &QToolButton::clicked, this, &CCustomLayoutWidget::OnChangeSettings);
	connect(TitleNameEdit, &QLineEdit::editingFinished, this, &CCustomLayoutWidget::OnTitleChanged);
	
	QAction *action;
	for (int i = -1; i < m_hierarchicalLayoutWidget.m_additionalNames.count(); i++){
		if (i < 0){
			action = new QAction(tr("Empty"), this);
		}
		else{
			action = new QAction(m_hierarchicalLayoutWidget.m_additionalNames[i], this);
		}

		connect(action, &QAction::triggered, this, &CCustomLayoutWidget::OnAddWidget);

		AddWidgets->addAction(action);
	}

	setAutoFillBackground(true);

	SetDefaultPalette();

	setMinimumSize(60, 60);

	setLayout(new QVBoxLayout(this));
}


void CCustomLayoutWidget::SetDefaultPalette()
{
	QPalette pal = palette();
	pal.setColor(QPalette::Background, QColor("transparent"));

	setPalette(pal);
}


void CCustomLayoutWidget::SetHighLightPalette()
{
	QPalette pal = palette();
	pal.setColor(QPalette::Background, QColor("lightblue"));

	setPalette(pal);
}


bool CCustomLayoutWidget::SetWidget(QWidget* widgetPtr)
{
	if (widgetPtr != NULL){
		if (WidgetPanel->layout() == NULL){
			WidgetPanel->setLayout(new QVBoxLayout(this));
		}

		if (WidgetPanel->layout()->count() == 0){
			WidgetPanel->layout()->addWidget(widgetPtr);
			m_externalWidgetPtr = widgetPtr;

			return true;
		}
	}

	if (m_externalWidgetPtr == NULL){
		return true;
	}

	// take care about external widget
	if ((WidgetPanel->layout() != NULL) && (WidgetPanel->layout()->count() == 1) && (WidgetPanel->layout()->itemAt(0)->widget() != NULL)){
		QWidget* takeWidgetPtr = WidgetPanel->layout()->takeAt(0)->widget();
		takeWidgetPtr->setParent(NULL);
	}

	delete WidgetPanel->layout();

	m_externalWidgetPtr = NULL;

	return true;
}


QWidget* CCustomLayoutWidget::GetWidget()
{
	return m_externalWidgetPtr;
}


void CCustomLayoutWidget::SetName(QString name)
{
	CCustomLayoutWidgetForm::TitleName->setText(name);
	CCustomLayoutWidgetForm::TitleNameEdit->setText(name);
}


QString CCustomLayoutWidget::GetName()
{
	QString retVal;
	if (m_hierarchicalLayoutWidget.m_viewMode == CHierarchicalLayoutWidget::VM_EDIT){
		retVal = CCustomLayoutWidgetForm::TitleNameEdit->text();
	}
	else{
		retVal = CCustomLayoutWidgetForm::TitleName->text();
	}
	return retVal;
}


void CCustomLayoutWidget::SetEditMode(bool isEditMode)
{
	if (isEditMode == true){
		EditPanel->setVisible(true);
		ChangeIconButton->setVisible(true);
		TitleNameEdit->setVisible(true);
		TitleNameEdit->setText(TitleName->text());
		TitleIcon->setVisible(false);
		TitleName->setVisible(false);
		RemoveIcon->setVisible(true);
	}
	else{
		EditPanel->setVisible(false);
		ChangeIconButton->setVisible(false);
		TitleNameEdit->setVisible(false);
		TitleIcon->setVisible(true);
		TitleName->setVisible(true);
		TitleName->setText(TitleNameEdit->text());
		RemoveIcon->setVisible(false);
	}
}


void CCustomLayoutWidget::SetId(const QByteArray& id)
{
	m_id = id;
}


QByteArray CCustomLayoutWidget::GetId()
{
	return m_id;
}


QPixmap CCustomLayoutWidget::GetIcon()
{
	QPixmap pixmap = *TitleIcon->pixmap();
	return pixmap;
}


void CCustomLayoutWidget::SetIcon(const QPixmap &icon)
{
	TitleIcon->setPixmap(icon);

	ChangeIconButton->setIcon(icon);
}

QByteArray CCustomLayoutWidget::GetViewId()
{
	return m_viewId;
}


void CCustomLayoutWidget::SetViewId(const QByteArray &viewId)
{
	m_viewId = viewId;
}


void CCustomLayoutWidget::SetTitleAlign(const ILayout::AlignType &align)
{
	m_titleAlign = align;
	if (align == ILayout::AT_LEFT){
		TitleSpacerLeft->changeSize(2, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);
		TitleSpacerRight->changeSize(2, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	}

	if (align == ILayout::AT_RIGHT){
		TitleSpacerLeft->changeSize(2, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		TitleSpacerRight->changeSize(2, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);
	}

	if (align == ILayout::AT_H_CENTER){
		TitleSpacerLeft->changeSize(2, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		TitleSpacerRight->changeSize(2, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	}

	bool isEditMode = m_hierarchicalLayoutWidget.m_viewMode == CHierarchicalLayoutWidget::VM_EDIT;

	SetEditMode(!isEditMode);
	SetEditMode(isEditMode);
}


ILayout::AlignType CCustomLayoutWidget::GetTitleAlign()
{
	return m_titleAlign;
}


void CCustomLayoutWidget::SetLayoutProperties(const ILayout::LayoutProperties &properties)
{
	setMinimumWidth(properties.minWidth);
	setMaximumWidth(properties.maxWidth);
	setMinimumHeight(properties.minHeight);
	setMaximumHeight(properties.maxHeight);

	m_properties = properties;
}


int CCustomLayoutWidget::LeftMargin()
{
	return m_properties.leftMargin;
}


int CCustomLayoutWidget::RightMargin()
{
	return m_properties.rightMargin;
}


int CCustomLayoutWidget::TopMargin()
{
	return m_properties.topMargin;
}


int CCustomLayoutWidget::BottomMargin()
{
	return m_properties.bottomMargin;
}


// protected methods

// reimplemented (QWidget)

void CCustomLayoutWidget::paintEvent(QPaintEvent* /*eventPtr*/)
{
	QPainter painter(this);

	painter.save();
	//painter.setOpacity(0.7);

//	if (m_hierarchicalLayoutWidget.IsBorderEnabled() == true 
	if (m_properties.isBorderEnabled == true
		|| m_hierarchicalLayoutWidget.m_viewMode == CHierarchicalLayoutWidget::VM_EDIT){

//		QPen pen(m_hierarchicalLayoutWidget.GetBorderColor());
		QPen pen(m_properties.borderColor);

		if (m_hierarchicalLayoutWidget.m_viewMode == CHierarchicalLayoutWidget::VM_EDIT){
			pen.setStyle(Qt::DashLine);
		}

		painter.setPen(pen);
		painter.drawRect(rect().adjusted(0, 0, -1, -1));
	}
}


void CCustomLayoutWidget::OnAddWidget()
{
	QAction* actionPtr = dynamic_cast<QAction*>(sender());
	if (actionPtr != nullptr){
		QList<QAction*> actions = AddWidgets->actions();

		int index = actions.indexOf(actionPtr) - 1;

		Q_EMIT m_hierarchicalLayoutWidget.EmitAddWidget(m_id, index);
	}
}


void CCustomLayoutWidget::OnChangeSettings()
{
	Q_EMIT m_hierarchicalLayoutWidget.EmitChangeProperties(m_id);
}


void CCustomLayoutWidget::OnDeleteWidget()
{
	Q_EMIT m_hierarchicalLayoutWidget.EmitDeleteWidget(m_id);
}


void CCustomLayoutWidget::OnSplitVertical()
{
	Q_EMIT m_hierarchicalLayoutWidget.EmitSplitLayout(m_id, ILayout::LT_VERTICAL_SPLITTER, width(), height());
}


void CCustomLayoutWidget::OnSplitHorizontal()
{
	Q_EMIT m_hierarchicalLayoutWidget.EmitSplitLayout(m_id, ILayout::LT_HORIZONTAL_SPLITTER, width(), height());
}


void CCustomLayoutWidget::OnAlignLeft()
{
	Q_EMIT m_hierarchicalLayoutWidget.EmitChangeAlignTitle(m_id, ILayout::AT_LEFT);
}


void CCustomLayoutWidget::OnAlignCenter()
{
	Q_EMIT m_hierarchicalLayoutWidget.EmitChangeAlignTitle(m_id, ILayout::AT_H_CENTER);
}


void CCustomLayoutWidget::OnAlignRight()
{
	Q_EMIT m_hierarchicalLayoutWidget.EmitChangeAlignTitle(m_id, ILayout::AT_RIGHT);
}


void CCustomLayoutWidget::OnChangeIcon()
{
	Q_EMIT m_hierarchicalLayoutWidget.EmitChangeIcon(m_id);
}


void CCustomLayoutWidget::OnTitleChanged()
{
	QString newText = TitleNameEdit->text();

	Q_EMIT m_hierarchicalLayoutWidget.EmitChangeTitle(m_id, newText);
}


void CCustomLayoutWidget::OnRemoveIcon()
{
	Q_EMIT m_hierarchicalLayoutWidget.EmitRemoveIcon(m_id);
}



} // namespace imtgui


