#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QSize>
#include <QtGui/QScreen>

// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtgui/IMonitorInfoProvider.h>


namespace imtgui
{


class CMonitorInfoProvider: public QObject, public IMonitorInfoProvider
{
	Q_OBJECT

public:
	enum MonitorChangeSet
	{
		MCE_PRIMARY_SCREEN_CHANGED = 0,
		MCE_SCREEN_ADDED,
		MCE_SCREEN_REMOVED,

		MCE_PHYSICAL_SIZE,
		MCE_PHYSICAL_RESOLUTION,
		MCE_ORIENTATION
	};

public:
	CMonitorInfoProvider();

	// reimplemented (imtgui::IMonitorResolutionProvider)
	virtual int GetMonitorsCount() const override;
	virtual QSize GetPhysicalSize(int index) const override;
	virtual double GetPhysicalResolutionX(int index) const override;
	virtual double GetPhysicalResolutionY(int index) const override;
	virtual QRect GetGeometry(int index) const override;
	virtual double GetScaling(int index) const override;

protected:
	void UpdateMonitorsInfo();

protected:
	QSize m_physicalSize;

private Q_SLOTS:
	void PrimaryScreenChanged(QScreen* screen);
	void ScreenAdded(QScreen* screen);
	void ScreenRemoved(QScreen* screen);

	void LogicalDotsPerInchChanged(qreal dpi);
	void OrientationChanged(Qt::ScreenOrientation orientation);

private:
	struct MonitorInfo
	{
		QScreen* screenPtr;
		QSize size;
		QRect geometry;
		double resolutionX;
		double resolutionY;
	};

	QList<MonitorInfo> m_monitors;

private:
	QSize RetrievePhysicalSize(QString monitorId);
};


} // namespace imtgui


