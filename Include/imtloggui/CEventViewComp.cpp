#include <imtloggui/CEventViewComp.h>


// Qt includes
#include <QDebug>
#include <QGraphicsLineItem>

// ACF includes
#include <iser/IObject.h>
#include <ilog/CMessage.h>
#include <ilog/CExtMessage.h>
#include <iwidgets/iwidgets.h>

// ImtCore includes
#include <imtloggui/CEventGraphicsView.h>
#include <imtloggui/CEventGroup.h>


namespace imtloggui
{


// public methods

CEventViewComp::CEventViewComp()
{
}


// reimplemented (ilog::IMessageConsumer)

bool CEventViewComp::IsMessageSupported(
			int /*messageCategory*/,
			int /*messageId*/,
			const istd::IInformationProvider* /*messagePtr*/) const
{
	return true;
}


void CEventViewComp::AddMessage(const IMessageConsumer::MessagePtr& messagePtr)
{
}


// reimplemented (iqtgui::TRestorableGuiWrap)

void CEventViewComp::OnRestoreSettings(const QSettings& settings)
{
}


void CEventViewComp::OnSaveSettings(QSettings& settings) const
{
}


// reimplemented (iqtgui::CGuiComponentBase)

void CEventViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	m_scenePtr = new QGraphicsScene(GetQtWidget());

	QGraphicsLineItem *item;

	QDateTime begin = QDateTime::currentDateTime();
	QDateTime end;
	end.setSecsSinceEpoch(begin.toSecsSinceEpoch() + 11000);

	m_timeAxisPtr = new CTimeAxis();
	m_timeAxisPtr->setColor(Qt::green);
	m_timeAxisPtr->setRect(0, 30, 10000, -30);
	m_timeAxisPtr->setTimeSpan(begin, end);
	m_timeAxisPtr->setMinorTickCount(12);

	qsrand(QDateTime::currentDateTime().toMSecsSinceEpoch());

	for (int i = 0; i < 10; i++){
		CEventGroup *item = new CEventGroup();
		QColor color(qrand() % 256, qrand() % 256, qrand() % 256);
		item->setColor(color);
		item->setGeometry(QRect(0, 300 * i, 10000, 300));
		m_scenePtr->addItem(item);
	}

	m_scenePtr->addItem(m_timeAxisPtr);

	m_viewPtr = new CEventGraphicsView(GetQtWidget());
	GetQtWidget()->layout()->addWidget(m_viewPtr);

	m_viewPtr->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	m_viewPtr->setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
	m_viewPtr->setScene(m_scenePtr);
	m_viewPtr->setSceneRect(m_scenePtr->sceneRect());
	m_viewPtr->setTimeAxis(m_timeAxisPtr);
}


void CEventViewComp::OnGuiDestroyed()
{
	delete m_viewPtr;
	delete m_scenePtr;

	BaseClass::OnGuiDestroyed();
}


void CEventViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();
}


// reimplemented (icomp::CComponentBase)

void CEventViewComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


void CEventViewComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


} // namespace imtloggui


