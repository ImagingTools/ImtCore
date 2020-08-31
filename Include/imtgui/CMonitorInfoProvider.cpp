#include <imtgui/CMonitorInfoProvider.h>


// STL includes
#include <cstring>

// Qt includes
#include <QtCore/QSettings>
#include <QtGui/QGuiApplication>

// ACF includes
#include <istd/CChangeNotifier.h>

// Windows includes
#include <windows.h>
#include <winreg.h>


namespace imtgui
{


// pulic methods

CMonitorInfoProvider::CMonitorInfoProvider()
{
	connect(qApp, &QGuiApplication::primaryScreenChanged, this, &CMonitorInfoProvider::PrimaryScreenChanged);
	connect(qApp, &QGuiApplication::screenAdded, this, &CMonitorInfoProvider::ScreenAdded);
	connect(qApp, &QGuiApplication::screenRemoved, this, &CMonitorInfoProvider::ScreenRemoved);

	UpdateMonitorsInfo();
}


// reimplemented (imtgui::IMonitorResolutionProvider)

int CMonitorInfoProvider::GetMonitorsCount() const
{
	return m_monitors.count();
}


QSize CMonitorInfoProvider::GetPhysicalSize(int index) const
{
	if (index >= 0 && index < m_monitors.count()){
		return m_monitors[index].size;
	}

	return QSize();
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


QRect CMonitorInfoProvider::GetGeometry(int index) const
{
	if (index >= 0 && index < m_monitors.count()){
		return m_monitors[index].geometry;
	}

	return QRect();
}


double CMonitorInfoProvider::GetScaling(int index) const
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


void CMonitorInfoProvider::OrientationChanged(Qt::ScreenOrientation /*orientation*/)
{
	istd::IChangeable::ChangeSet changeSet(MCE_ORIENTATION);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	UpdateMonitorsInfo();
}


// private methods

QSize CMonitorInfoProvider::RetrievePhysicalSize(QString monitorId)
{
	QSize size;

	DISPLAY_DEVICE ddAdapter;
	ddAdapter.cb = sizeof(ddAdapter);
	int adapterIndex = 0;

	bool isMonitorFound = false;
	while (EnumDisplayDevices(0, adapterIndex, &ddAdapter, 0) && !isMonitorFound)
	{
		DISPLAY_DEVICE ddMonitor;
		memset(&ddMonitor, 0, sizeof(ddMonitor));
		ddMonitor.cb = sizeof(ddMonitor);
		int monitorIndex = 0;

		if (monitorId == QString::fromStdWString(ddAdapter.DeviceName)){
			while (EnumDisplayDevices(ddAdapter.DeviceName, monitorIndex, &ddMonitor, 0) && !isMonitorFound){
				if (ddMonitor.StateFlags & DISPLAY_DEVICE_ACTIVE && !(ddMonitor.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)){
					QString deviceId = QString::fromStdWString(ddMonitor.DeviceID);
					int index1 = deviceId.indexOf('\\');
					int index2 = deviceId.indexOf('\\', index1 + 1);
					deviceId = deviceId.mid(index1 + 1, index2 - index1 - 1);

					QString displayKey = "SYSTEM\\CurrentControlSet\\Enum\\DISPLAY";
					QSettings registry("HKEY_LOCAL_MACHINE\\" + displayKey, QSettings::NativeFormat);
					QStringList keys = registry.allKeys();

					QString edidKey;
					for (QString key : keys){
						if (key.contains(deviceId) && key.contains("EDID")){
							edidKey = key;
							break;
						}
					}

					DWORD type = REG_BINARY;
					unsigned char data[128];
					DWORD dataSize = 128;

					edidKey.remove(edidKey.size() - 5, 5);
					edidKey.replace("/", "\\");
					displayKey.append("\\");
					displayKey.append(edidKey);
					HKEY hKey = 0;
					RegOpenKeyExA(HKEY_LOCAL_MACHINE, displayKey.toStdString().c_str(), 0, KEY_QUERY_VALUE, &hKey);
					RegQueryValueExA(hKey, "EDID", NULL, &type, data, &dataSize);

					size.setWidth(data[66] + (((int)data[68] & 0xF0) << 4));
					size.setHeight(data[67] + (((int)data[68] & 0x0F) << 8));

					isMonitorFound = true;
				}
				monitorIndex++;

				memset(&ddMonitor, 0, sizeof(ddMonitor));
				ddMonitor.cb = sizeof(ddMonitor);
			}
		}

		memset(&ddAdapter, 0, sizeof(ddAdapter));
		ddAdapter.cb = sizeof(ddAdapter);
		adapterIndex++;
	}

	return size;
}


void CMonitorInfoProvider::UpdateMonitorsInfo()
{
	for (MonitorInfo info : m_monitors){
		info.screenPtr->disconnect(this);
	}

	m_monitors.clear();

	QList<QScreen*> screenList = QGuiApplication::screens();
	for (QScreen* screen : screenList){
		MonitorInfo info;
		info.screenPtr = screen;
		info.size = RetrievePhysicalSize(screen->name());
		info.geometry = screen->geometry();
		if (info.size.width() != 0 && info.size.height() != 0){
			info.resolutionX = (double)info.geometry.width() / info.size.width();
			info.resolutionY = (double)info.geometry.height() / info.size.height();
		}
		else{
			info.resolutionX = 0;
			info.resolutionY = 0;
		}

		connect(screen, &QScreen::logicalDotsPerInchChanged, this, &CMonitorInfoProvider::LogicalDotsPerInchChanged);
		connect(screen, &QScreen::orientationChanged, this, &CMonitorInfoProvider::OrientationChanged);

		m_monitors.append(info);
	}
}


} // namespace imtgui


