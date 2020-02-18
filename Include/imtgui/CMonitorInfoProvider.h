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
	~CMonitorInfoProvider();

	// reimplemented (imtgui::IMonitorResolutionProvider)
	virtual int GetMonitorsCount() const override;
	virtual QSizeF GetPhysicalSize(int index) const override;
	virtual double GetPhysicalResolutionX(int index) const override;
	virtual double GetPhysicalResolutionY(int index) const override;
	virtual double GetMonitorScaling(int index) const override;

private Q_SLOTS:
	void PrimaryScreenChanged(QScreen* screen);
	void ScreenAdded(QScreen* screen);
	void ScreenRemoved(QScreen* screen);

	void LogicalDotsPerInchChanged(qreal dpi);
	void PhysicalSizeChanged(const QSizeF& size);
	void OrientationChanged(Qt::ScreenOrientation orientation);

private:
	struct MonitorInfo
	{
		QScreen* screenPtr;
		QSizeF size;
		double resolutionX;
		double resolutionY;
	};

	QList<MonitorInfo> m_monitors;

private:
	void UpdateMonitorsInfo();
};


} // namespace imtgui


