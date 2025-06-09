#pragma once


// Qt includes
#if QT_VERSION < 0x060000
	#include <QtCore/QMutex>
#else
	#include <QtCore/QRecursiveMutex>
#endif

// ACF includes
#include <imod/TModelWrap.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtdev/IDeviceController.h>


namespace imtdev
{


class CDeviceControllerCompBase:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public imtdev::IDeviceController
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CDeviceControllerCompBase)
		I_REGISTER_SUBELEMENT(DeviceInfoList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceInfoList, imtbase::ICollectionInfo, ExtractDeviceInfoList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceInfoList, istd::IChangeable, ExtractDeviceInfoList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceInfoList, imod::IModel, ExtractDeviceInfoList);
		I_REGISTER_INTERFACE(IDeviceController);
		I_REGISTER_INTERFACE(IDeviceEnumerator);
		I_ASSIGN(m_isAutoCloseEnabledAttrPtr, "AutoClose", "Automatically close an opened device if it is not found during the next enumeration", true, "true");
	I_END_COMPONENT;

	// reimplemented (IDeviceController)
	virtual const imtbase::ICollectionInfo& GetDeviceInstanceList() const override;
	virtual const IDeviceStateProvider& GetDeviceStateProvider() const override;

protected:
	struct EnumeratedDeviceInfo
	{
		QByteArray id;
		QString name;
		QString description;
	};
	typedef QVector<EnumeratedDeviceInfo> EnumeratedDeviceList;

	class DeviceStateProvider : virtual public IDeviceStateProvider
	{
	public:
		DeviceStateProvider();
		void SetParent(CDeviceControllerCompBase& parent);

		// reimplemented (IDeviceStateProvider)
		virtual DeviceState GetDeviceState(const QByteArray& deviceId) const override;

	private:
		CDeviceControllerCompBase* m_parentPtr;
	};

protected:
	// Must be called from the main thread!!!
	void UpdateDeviceList(EnumeratedDeviceList& enumeratedDeviceList);
	void AutoCloseDisconnectedDevices();

	template <class InteraceType>
	static InteraceType* ExtractDeviceInfoList(CDeviceControllerCompBase& parent)
	{
		return &parent.m_deviceList;
	}

protected:
	I_ATTR(bool, m_isAutoCloseEnabledAttrPtr);

	imod::TModelWrap<imtbase::CCollectionInfo> m_deviceList;
	imod::TModelWrap<DeviceStateProvider> m_deviceStateProvider;
	QMap<QByteArray, DeviceAccessorPtr> m_openedDevices;

#if QT_VERSION < 0x060000
	mutable QMutex m_deviceListMutex;
	mutable QMutex m_openedDevicesMutex;
#else
	mutable QRecursiveMutex m_deviceListMutex;
	mutable QRecursiveMutex m_openedDevicesMutex;
#endif
};


} // namespace imtdev


