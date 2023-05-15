#pragma once


// Qt includes
#include <QtCore/QRecursiveMutex>

// ACF includes
#include <imod/TModelWrap.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>

// GmgCore includes
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
	virtual const imtbase::ICollectionInfo& GetAvailableDeviceList() const override;
	virtual DeviceState GetDeviceState(const QByteArray& deviceId) const override;

protected:
	struct EnumeratedDeviceInfo
	{
		QByteArray id;
		QString name;
		QString description;
	};
	typedef QVector<EnumeratedDeviceInfo> EnumeratedDeviceList;

	// Must be called from the main thread!!!
	void UpdateDeviceList(EnumeratedDeviceList& enumeratedDeviceList);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();

protected:
	QMap<QByteArray, DeviceAccessorPtr> m_openedDevices;
	mutable QRecursiveMutex m_openedDevicesMutex;

private:
	void AutoCloseDisconnectedDevices();

	template <class InteraceType>
	static InteraceType* ExtractDeviceInfoList(CDeviceControllerCompBase& parent)
	{
		return &parent.m_deviceList;
	}

private:
	I_ATTR(bool, m_isAutoCloseEnabledAttrPtr);

	typedef imod::TModelWrap<imtbase::CCollectionInfo> DeviceInfoList;
	DeviceInfoList m_deviceList;
	mutable QMutex m_deviceListMutex;
};


} // namespace imtdev


