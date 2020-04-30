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

	m_viewPtr = new CEventGraphicsView(GetQtWidget());
	m_viewPtr->setScene(m_scenePtr);
	m_viewPtr->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	GetQtWidget()->layout()->addWidget(m_viewPtr);

	QGraphicsLineItem *item;
	QPen pen;
	pen.setWidth(0);

	//item = m_scenePtr->addLine(0, 0, 0, 200);
	//item->setPen(pen);
	//item = m_scenePtr->addLine(0, 0, 200, 200);
	//item->setPen(pen);
	//item = m_scenePtr->addLine(200, 0, 0, 200);
	//item->setPen(pen);
	//item = m_scenePtr->addLine(200, 0, 200, 200);
	//item->setPen(pen);
	//item = m_scenePtr->addLine(250, 0, -250, 200);
	//item->setPen(pen);

	QDateTime begin = QDateTime::currentDateTime();
	QDateTime end;
	end.setSecsSinceEpoch(begin.toSecsSinceEpoch() + 100000);

	m_timeAxisPtr = new CTimeAxis();
	m_timeAxisPtr->setGeometry(QRect(-10000, 250, 500, 100));
	m_timeAxisPtr->setTimeInterval(begin, end);

	m_scenePtr->addItem(m_timeAxisPtr);

	//m_viewPtr->setSceneRect(QRectF(0,-100,200,200));
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


