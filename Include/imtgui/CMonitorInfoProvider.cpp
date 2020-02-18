#include <imtgui/CMonitorInfoProvider.h>


// Qt includes
#include <QtGui/QGuiApplication>

// ACF includes
#include <istd/CChangeNotifier.h>

#include <QDebug>


namespace imtgui
{
// pulic methods
CMonitorInfoProvider::CMonitorInfoProvider()
{
	QList<QScreen*> screenList = QGuiApplication::screens();
	for (QScreen* screen : screenList){
		MonitorInfo info;
		info.screenPtr = screen;
		info.size = screen->physicalSize();
		info.resolutionX = screen->physicalDotsPerInchX() / 25.4;
		info.resolutionY = screen->physicalDotsPerInchX() / 25.4;
		
		connect(screen, &QScreen::physicalSizeChanged, this, &CMonitorInfoProvider::PhysicalSizeChanged);
		connect(screen, &QScreen::logicalDotsPerInchChanged, this, &CMonitorInfoProvider::PhysicalDotsPerInchChanged);
		connect(screen, &QScreen::orientationChanged, this, &CMonitorInfoProvider::OrientationChanged);
	}

	qDebug() << connect(qApp, &QGuiApplication::primaryScreenChanged, this, &CMonitorInfoProvider::PrimaryScreenChanged);
	qDebug() << connect(qApp, &QGuiApplication::screenAdded, this, &CMonitorInfoProvider::ScreenAdded);
	qDebug() << connect(qApp, &QGuiApplication::screenRemoved, this, &CMonitorInfoProvider::ScreenRemoved);
}


CMonitorInfoProvider::~CMonitorInfoProvider()
{
	qApp->disconnect(this);
	for (MonitorInfo info : m_monitor){
		info.screenPtr->disconnect(this);
	}
}


// reimplemented (imtgui::IMonitorResolutionProvider)

int CMonitorInfoProvider::GetMonitorsCount() const
{
	return m_monitor.count();
}


QSizeF CMonitorInfoProvider::GetPhysicalSize(int index) const
{
	if (index >= 0 && index < m_monitor.count()){
		return m_monitor[index].size;
	}
	
	return QSizeF();
}


double CMonitorInfoProvider::GetPhysicalResolutionX(int index) const
{
	if (index >= 0 && index < m_monitor.count()){
		return m_monitor[index].resolutionX;
	}

	return -1;
}


double CMonitorInfoProvider::GetPhysicalResolutionY(int index) const
{
	if (index >= 0 && index < m_monitor.count()){
		return m_monitor[index].resolutionY;
	}

	return -1;
}


// private slots

void CMonitorInfoProvider::PrimaryScreenChanged(QScreen* /*screen*/)
{
	istd::IChangeable::ChangeSet changeSet(MCE_PRIMARY_SCREEN_CHANGED);
	istd::CChangeNotifier changeNotifier(this, &changeSet);
}


void CMonitorInfoProvider::ScreenAdded(QScreen* /*screen*/)
{
	istd::IChangeable::ChangeSet changeSet(MCE_SCREEN_ADDED);
	istd::CChangeNotifier changeNotifier(this, &changeSet);
}


void CMonitorInfoProvider::ScreenRemoved(QScreen* /*screen*/)
{
	istd::IChangeable::ChangeSet changeSet(MCE_SCREEN_REMOVED);
	istd::CChangeNotifier changeNotifier(this, &changeSet);
}


void CMonitorInfoProvider::PhysicalDotsPerInchChanged(qreal /*dpi*/)
{
	istd::IChangeable::ChangeSet changeSet(MCE_PHYSICAL_RESOLUTION);
	istd::CChangeNotifier changeNotifier(this, &changeSet);
}


void CMonitorInfoProvider::PhysicalSizeChanged(const QSizeF& /*size*/)
{
	istd::IChangeable::ChangeSet changeSet(MCE_PHYSICAL_SIZE);
	istd::CChangeNotifier changeNotifier(this, &changeSet);
}


void CMonitorInfoProvider::OrientationChanged(Qt::ScreenOrientation /*orientation*/)
{
	istd::IChangeable::ChangeSet changeSet(MCE_ORIENTATION);
	istd::CChangeNotifier changeNotifier(this, &changeSet);
}


} // namespace imtgui


