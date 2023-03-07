#pragma once


// Qt includes
#include <QtCore/QMutex>

// ACF includes
#include <imod/TModelWrap.h>
#include <iprm/IOptionsList.h>
#include <ilog/TLoggerCompWrap.h>
#include <iproc/IProcessor.h>

// Acula includes
#include <imtdev/IDeviceController.h>
#include <imtdev/IDeviceInfo.h>


namespace imtdev
{


template <class Base>
class TDeviceControllerCompWrap: public ilog::CLoggerComponentBase, virtual public Base
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(TDeviceControllerCompWrap)
		I_REGISTER_INTERFACE(IDeviceController);
		I_REGISTER_SUBELEMENT(DeviceInfoList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceInfoList, istd::IChangeable, ExtractDeviceInfoList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceInfoList, imod::IModel, ExtractDeviceInfoList);
		I_REGISTER_SUBELEMENT_INTERFACE(DeviceInfoList, iprm::IOptionsList, ExtractDeviceInfoList);
		I_ASSIGN(m_deviceFactoryCompPtr, "DeviceFactory", "Factory used for creation of the devices", true, "DeviceFactory");
	I_END_COMPONENT;

	// pseudo-reimplemented (IDeviceController)
	virtual const iprm::IOptionsList& GetDeviceInfoList() const override;
	virtual const IDeviceInfo* GetDeviceInfo(const QByteArray& deviceId) const override;
	virtual int GetDeviceCapabilities(const QByteArray& deviceId) const override;
	virtual IDeviceController::DeviceState GetDeviceState(const QByteArray& deviceId) const override;
	virtual bool InitializeDevice(const QByteArray& deviceId, const iprm::IParamsSet* paramsPtr = nullptr) override;
	virtual bool StartDevice(const QByteArray& deviceId) override;
	virtual bool StopDevice(const QByteArray& deviceId) override;
	virtual QByteArrayList GetSupportedCommands(const QByteArray& deviceId) const override;
	virtual int ExecuteCommand(const QByteArray& deviceId, const QByteArray& commandTypeId, const iprm::IParamsSet* commandParamsPtr = nullptr) override;

protected:
	struct DeviceInfo
	{
		QString name;
		QString description;
		QByteArray id;

		bool operator==(const DeviceInfo& deviceInfo) const
		{
			return 
					(name == deviceInfo.name) && 
					(description == deviceInfo.description) &&
					(id == deviceInfo.id);
		}


		bool operator!=(const DeviceInfo& deviceInfo) const
		{
			return !operator==(deviceInfo);
		}
	};

	typedef QVector<DeviceInfo> DeviceInfos;

	typedef istd::TSmartPtr<IDeviceController> DevicePtr;
	typedef QMap<QByteArray, DevicePtr> DeviceMap;

	virtual bool InitializeDeviceManager() = 0;
	virtual bool DeinitializeDeviceManager() = 0;
	virtual bool EnumerateDevices(DeviceInfos& deviceInfoList) const = 0;
	virtual bool SetupDevice(const IDeviceController& device) const = 0;
	virtual DevicePtr FindDeviceById(const QByteArray& deviceId) const;
	virtual void UpdateDevices();
	virtual DevicePtr CreateDeviceInstance() const;
	virtual void OnUpdateDevices(const DeviceInfos& deviceInfos, const DeviceMap& devices);
	QByteArray GetDeviceId(const imtdev::IDeviceController* devicePtr) const;
	QString GetDeviceName(const imtdev::IDeviceController* devicePtr) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();
	virtual void OnComponentDestroyed();

private:
	template <class InteraceType>
	static InteraceType* ExtractDeviceInfoList(TDeviceControllerCompWrap& parent)
	{
		return &parent.m_deviceInfoList;
	}

	class DeviceInfoList: virtual public iprm::IOptionsList
	{
	public:
		DeviceInfoList();

		void SetParent(TDeviceControllerCompWrap* parent);
		void SetDeviceInfos(const DeviceInfos& deviceInfos);

		// reimplemented (iprm::IOptionsList)
		virtual int GetOptionsFlags() const override;
		virtual int GetOptionsCount() const override;
		virtual QString GetOptionName(int index) const override;
		virtual QString GetOptionDescription(int index) const override;
		virtual QByteArray GetOptionId(int index) const override;
		virtual bool IsOptionEnabled(int index) const override;

		// reimplemented (istd::IChangeable)
		virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS);

	private:
		TDeviceControllerCompWrap* m_parentPtr;
		DeviceInfos m_deviceInfos;
	};

private:
	I_FACT(IDeviceController, m_deviceFactoryCompPtr);

	typedef imod::TModelWrap<DeviceInfoList> DeviceInfoListModel;
	DeviceInfoListModel m_deviceInfoList;

	DeviceMap m_deviceMap;
	mutable QMutex m_deviceMapMutex;
};


template<class Base>
inline const iprm::IOptionsList& TDeviceControllerCompWrap<Base>::GetDeviceInfoList() const
{
	return m_deviceInfoList;
}


template<class Base>
inline const IDeviceInfo* TDeviceControllerCompWrap<Base>::GetDeviceInfo(const QByteArray& deviceId) const
{
	QMutexLocker lock(&m_deviceMapMutex);

	if (m_deviceMap.contains(deviceId)){
		DevicePtr devicePtr = m_deviceMap[deviceId];
		Q_ASSERT(devicePtr.IsValid());

		return devicePtr->GetDeviceInfo(deviceId);
	}

	return nullptr;
}

template<class Base>
inline int TDeviceControllerCompWrap<Base>::GetDeviceCapabilities(const QByteArray & deviceId) const
{
	QMutexLocker lock(&m_deviceMapMutex);

	if (m_deviceMap.contains(deviceId)){
		DevicePtr devicePtr = m_deviceMap[deviceId];
		Q_ASSERT(devicePtr.IsValid());

		return devicePtr->GetDeviceCapabilities(deviceId);
	}

	return 0;
}


template<class Base>
inline IDeviceController::DeviceState TDeviceControllerCompWrap<Base>::GetDeviceState(const QByteArray& deviceId) const
{
	QMutexLocker lock(&m_deviceMapMutex);

	if (m_deviceMap.contains(deviceId)){
		DevicePtr devicePtr = m_deviceMap[deviceId];
		Q_ASSERT(devicePtr.IsValid());

		return devicePtr->GetDeviceState(deviceId);
	}

	return IDeviceController::DS_NONE;
}


template<class Base>
inline bool TDeviceControllerCompWrap<Base>::InitializeDevice(const QByteArray& deviceId, const iprm::IParamsSet* paramsPtr)
{
	QMutexLocker lock(&m_deviceMapMutex);

	if (m_deviceMap.contains(deviceId)){
		DevicePtr devicePtr = m_deviceMap[deviceId];
		Q_ASSERT(devicePtr.IsValid());

		return devicePtr->InitializeDevice(deviceId, paramsPtr);
	}

	return false;
}


template<class Base>
inline bool TDeviceControllerCompWrap<Base>::StartDevice(const QByteArray& deviceId)
{
	QMutexLocker lock(&m_deviceMapMutex);

	if (m_deviceMap.contains(deviceId)){
		DevicePtr devicePtr = m_deviceMap[deviceId];
		Q_ASSERT(devicePtr.IsValid());

		return devicePtr->StartDevice(deviceId);
	}

	return false;
}


template<class Base>
inline bool TDeviceControllerCompWrap<Base>::StopDevice(const QByteArray& deviceId)
{
	QMutexLocker lock(&m_deviceMapMutex);

	if (m_deviceMap.contains(deviceId)){
		DevicePtr devicePtr = m_deviceMap[deviceId];
		Q_ASSERT(devicePtr.IsValid());

		return devicePtr->StopDevice(deviceId);
	}

	return false;
}


template<class Base>
inline QByteArrayList TDeviceControllerCompWrap<Base>::GetSupportedCommands(const QByteArray& deviceId) const
{
	QMutexLocker lock(&m_deviceMapMutex);

	if (m_deviceMap.contains(deviceId)){
		DevicePtr devicePtr = m_deviceMap[deviceId];
		Q_ASSERT(devicePtr.IsValid());

		return devicePtr->GetSupportedCommands(deviceId);
	}

	return QByteArrayList();
}


template<class Base>
inline int TDeviceControllerCompWrap<Base>::ExecuteCommand(
			const QByteArray& deviceId,
			const QByteArray& commandTypeId,
			const iprm::IParamsSet* commandParamsPtr)
{
	QMutexLocker lock(&m_deviceMapMutex);

	if (m_deviceMap.contains(deviceId)){
		DevicePtr devicePtr = m_deviceMap[deviceId];
		Q_ASSERT(devicePtr.IsValid());

		return devicePtr->ExecuteCommand(deviceId, commandTypeId, commandParamsPtr);
	}

	return iproc::IProcessor::TS_INVALID;
}


// protected methods

template<class Base>
inline typename TDeviceControllerCompWrap<Base>::DevicePtr TDeviceControllerCompWrap<Base>::FindDeviceById(const QByteArray& deviceId) const
{
	QMutexLocker lock(&m_deviceMapMutex);

	if (m_deviceMap.contains(deviceId)){
		return m_deviceMap[deviceId];
	}

	return DevicePtr();
}


template<class Base>
inline void TDeviceControllerCompWrap<Base>::UpdateDevices()
{
	DeviceInfos deviceInfos;
	DeviceMap deviceMap;

	if (EnumerateDevices(deviceInfos)){
		for (int deviceIndex = 0; deviceIndex < deviceInfos.count(); ++deviceIndex){
			QByteArray deviceId = deviceInfos[deviceIndex].id;
			Q_ASSERT(!deviceId.isEmpty());

			DevicePtr devicePtr = CreateDeviceInstance();
			if (devicePtr.IsValid()){
				if (SetupDevice(*devicePtr)){
					deviceMap[deviceId] = devicePtr;

					devicePtr->InitializeDevice(QByteArray());
				}
			}
		}
	}

	OnUpdateDevices(deviceInfos, deviceMap);
}


template<class Base>
inline typename TDeviceControllerCompWrap<Base>::DevicePtr TDeviceControllerCompWrap<Base>::CreateDeviceInstance() const
{
	if (m_deviceFactoryCompPtr.IsValid()){
		return DevicePtr(m_deviceFactoryCompPtr.CreateInstance());
	}

	return DevicePtr();
}


template<class Base>
inline void TDeviceControllerCompWrap<Base>::OnUpdateDevices(const DeviceInfos& deviceInfos, const DeviceMap& devices)
{
	m_deviceInfoList.SetDeviceInfos(deviceInfos);

	QMutexLocker lock(&m_deviceMapMutex);

	m_deviceMap = devices;
}


template<class Base>
inline QByteArray TDeviceControllerCompWrap<Base>::GetDeviceId(const imtdev::IDeviceController* devicePtr) const
{
	if (devicePtr != nullptr){
		QMutexLocker lock(&m_deviceMapMutex);

		for (DeviceMap::ConstIterator iter = m_deviceMap.constBegin(); iter != m_deviceMap.constEnd(); ++iter){
			if(devicePtr == iter.value().GetPtr()){
				return iter.key();
			}
		}
	}

	return QByteArray();
}

template<class Base>
inline QString TDeviceControllerCompWrap<Base>::GetDeviceName(const imtdev::IDeviceController* devicePtr) const
{
	QByteArray deviceId = GetDeviceId(devicePtr);
	if (!deviceId.isEmpty()){
		for (int i = 0; i < m_deviceInfoList.GetOptionsCount(); ++i){
			if (deviceId == m_deviceInfoList.GetOptionId(i)){
				return m_deviceInfoList.GetOptionName(i);
			}
		}
	}

	return QString();
}


// reimplemented (icomp::CComponentBase)

template<class Base>
inline void TDeviceControllerCompWrap<Base>::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (InitializeDeviceManager()){
		UpdateDevices();

		SetTracingEnabled(IsVerboseEnabled());
	}
}


template<class Base>
inline void TDeviceControllerCompWrap<Base>::OnComponentDestroyed()
{
	DeinitializeDeviceManager();

	BaseClass::OnComponentDestroyed();
}


// public methods of the embedded class DeviceInfoList

template<class Base>
inline TDeviceControllerCompWrap<Base>::DeviceInfoList::DeviceInfoList()
{
}


template<class Base>
inline void TDeviceControllerCompWrap<Base>::DeviceInfoList::SetParent(TDeviceControllerCompWrap* parent)
{
	m_parentPtr = parent;
}


template<class Base>
inline void TDeviceControllerCompWrap<Base>::DeviceInfoList::SetDeviceInfos(const DeviceInfos& deviceInfos)
{
	if (m_deviceInfos != deviceInfos){
		istd::CChangeNotifier changeNotifier(this);

		m_deviceInfos = deviceInfos;
	}
}


// reimplemented (iprm::IOptionsList)

template<class Base>
inline int TDeviceControllerCompWrap<Base>::DeviceInfoList::GetOptionsFlags() const
{
	return SCF_SUPPORT_UNIQUE_ID;
}


template<class Base>
inline int TDeviceControllerCompWrap<Base>::DeviceInfoList::GetOptionsCount() const
{
	return m_deviceInfos.count();
}


template<class Base>
inline QString TDeviceControllerCompWrap<Base>::DeviceInfoList::GetOptionName(int index) const
{
	return m_deviceInfos[index].name;
}


template<class Base>
inline QString TDeviceControllerCompWrap<Base>::DeviceInfoList::GetOptionDescription(int index) const
{
	return m_deviceInfos[index].description;
}


template<class Base>
inline QByteArray TDeviceControllerCompWrap<Base>::DeviceInfoList::GetOptionId(int index) const
{
	return m_deviceInfos[index].id;
}


template<class Base>
inline bool TDeviceControllerCompWrap<Base>::DeviceInfoList::IsOptionEnabled(int /*index*/) const
{
	return true;
}


// reimplemented (istd::IChangeable)

template<class Base>
inline bool TDeviceControllerCompWrap<Base>::DeviceInfoList::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_deviceInfos.clear();

	return true;
}


typedef TDeviceControllerCompWrap<IDeviceController> CDeviceControllerCompBase;


} // namespace imtdev


