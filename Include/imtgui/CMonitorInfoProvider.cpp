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
	UpdateMonitorsInfo();
}


CMonitorInfoProvider::~CMonitorInfoProvider()
{
	qApp->disconnect(this);
	for (MonitorInfo info : m_monitors){
		info.screenPtr->disconnect(this);
	}
}


// reimplemented (imtgui::IMonitorResolutionProvider)

int CMonitorInfoProvider::GetMonitorsCount() const
{
	return m_monitors.count();
}


QSizeF CMonitorInfoProvider::GetPhysicalSize(int index) const
{
	if (index >= 0 && index < m_monitors.count()){
		return m_monitors[index].size;
	}
	
	return QSizeF();
}


double CMonitorInfoProvider::GetPhysicalResolutionX(int index) const
{
	if (index >= 0 && index < m_monitors.count()){
		return m_monitors[index].resolutionX;
	}

	return -1;
}


double CMonitorInfoProvider::GetPhysicalResolutionY(int index) const
{
	if (index >= 0 && index < m_monitors.count()){
		return m_monitors[index].resolutionY;
	}

	return -1;
}


double CMonitorInfoProvider::GetMonitorScaling(int index) const
{
	if (index >= 0 && index < m_monitors.count()){
		return m_monitors[index].screenPtr->logicalDotsPerInchX() / 96.0;
	}

	return -1;
}


// private slots

void CMonitorInfoProvider::PrimaryScreenChanged(QScreen* /*screen*/)
{
	istd::IChangeable::ChangeSet changeSet(MCE_PRIMARY_SCREEN_CHANGED);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	UpdateMonitorsInfo();
}


void CMonitorInfoProvider::ScreenAdded(QScreen* /*screen*/)
{
	istd::IChangeable::ChangeSet changeSet(MCE_SCREEN_ADDED);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	UpdateMonitorsInfo();
}


void CMonitorInfoProvider::ScreenRemoved(QScreen* /*screen*/)
{
	istd::IChangeable::ChangeSet changeSet(MCE_SCREEN_REMOVED);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	UpdateMonitorsInfo();
}


void CMonitorInfoProvider::LogicalDotsPerInchChanged(qreal /*dpi*/)
{
	istd::IChangeable::ChangeSet changeSet(MCE_PHYSICAL_RESOLUTION);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	UpdateMonitorsInfo();
}


void CMonitorInfoProvider::PhysicalSizeChanged(const QSizeF& /*size*/)
{
	istd::IChangeable::ChangeSet changeSet(MCE_PHYSICAL_SIZE);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	UpdateMonitorsInfo();
}


void CMonitorInfoProvider::OrientationChanged(Qt::ScreenOrientation /*orientation*/)
{
	istd::IChangeable::ChangeSet changeSet(MCE_ORIENTATION);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	UpdateMonitorsInfo();
}


// private methods

void CMonitorInfoProvider::UpdateMonitorsInfo()
{
	qApp->disconnect(this);
	for (MonitorInfo info : m_monitors){
		info.screenPtr->disconnect(this);
	}

	m_monitors.clear();

	QList<QScreen*> screenList = QGuiApplication::screens();
	for (QScreen* screen : screenList){
		MonitorInfo info;
		info.screenPtr = screen;
		info.size = screen->physicalSize();
		info.resolutionX = screen->physicalDotsPerInchX() / 25.4;
		info.resolutionY = screen->physicalDotsPerInchX() / 25.4;

		connect(screen, &QScreen::physicalSizeChanged, this, &CMonitorInfoProvider::PhysicalSizeChanged);
		connect(screen, &QScreen::logicalDotsPerInchChanged, this, &CMonitorInfoProvider::LogicalDotsPerInchChanged);
		connect(screen, &QScreen::orientationChanged, this, &CMonitorInfoProvider::OrientationChanged);

		m_monitors.append(info);
	}

	connect(qApp, &QGuiApplication::primaryScreenChanged, this, &CMonitorInfoProvider::PrimaryScreenChanged);
	connect(qApp, &QGuiApplication::screenAdded, this, &CMonitorInfoProvider::ScreenAdded);
	connect(qApp, &QGuiApplication::screenRemoved, this, &CMonitorInfoProvider::ScreenRemoved);
}


} // namespace imtgui


