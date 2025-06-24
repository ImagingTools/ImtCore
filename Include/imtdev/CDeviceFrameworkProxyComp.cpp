#include <imtdev/CDeviceFrameworkProxyComp.h>


namespace imtdev
{


// public methods

CDeviceFrameworkProxyComp::CDeviceFrameworkProxyComp()
	:m_deviceController(*this),
	m_observer(*this),
	m_updateBridge(&m_deviceStateProvider, imod::CModelUpdateBridge::UF_SOURCE)
{
	m_deviceStateProvider.SetParent(this);
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CDeviceFrameworkProxyComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_deviceFrameworkCompPtr.IsValid()){
		imod::IModel* modelPtr = nullptr;
		
		modelPtr = dynamic_cast<imod::IModel*>(m_deviceFrameworkCompPtr->GetDeviceTypeList());
		if (modelPtr != nullptr){
			m_observer.RegisterModel(modelPtr, MI_TYPE_LIST);
		}

		modelPtr = dynamic_cast<imod::IModel*>(m_deviceFrameworkCompPtr->GetDeviceList());
		if (modelPtr != nullptr){
			m_observer.RegisterModel(modelPtr, MI_DEVICE_LIST);
		}

		modelPtr = dynamic_cast<imod::IModel*>(m_deviceFrameworkCompPtr->GetExtendedDeviceList());
		if (modelPtr != nullptr){
			m_observer.RegisterModel(modelPtr, MI_EXTENDED_DEVICE_LIST);
		}

		modelPtr = dynamic_cast<imod::IModel*>(m_deviceFrameworkCompPtr->GetDeviceStateProvider());
		if (modelPtr != nullptr){
			modelPtr->AttachObserver(&m_updateBridge);
		}
	}
}


void CDeviceFrameworkProxyComp::OnComponentDestroyed()
{
	m_updateBridge.EnsureModelsDetached();

	m_observer.UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// protected methods of the embedded class Observer

CDeviceFrameworkProxyComp::Observer::Observer(CDeviceFrameworkProxyComp& parent)
	:m_parent(parent)
{
}


// protected methods of the embedded class Observer

void CDeviceFrameworkProxyComp::Observer::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	switch (modelId){
	case MI_TYPE_LIST:
		m_parent.m_deviceTypeList.CopyFrom(*GetObjectAt<imtbase::ICollectionInfo>(MI_TYPE_LIST));
		break;
	case MI_DEVICE_LIST:
		m_parent.m_deviceList.CopyFrom(*GetObjectAt<imtbase::ICollectionInfo>(MI_DEVICE_LIST));
		break;
	case MI_EXTENDED_DEVICE_LIST:
		m_parent.m_extendedDeviceList.CopyFrom(*GetObjectAt<imtbase::ICollectionInfo>(MI_EXTENDED_DEVICE_LIST));
		break;
	}
}


// public methods of the embedded class DeviceStateProvider

void CDeviceFrameworkProxyComp::DeviceStateProvider::SetParent(CDeviceFrameworkProxyComp* parentPtr)
{
	m_parentPtr = parentPtr;
}


// reimplemented (IDeviceStateProvider)

IDeviceStateProvider::DeviceState CDeviceFrameworkProxyComp::DeviceStateProvider::GetDeviceState(const QByteArray& deviceId) const
{
	Q_ASSERT(m_parentPtr != nullptr);

	if (m_parentPtr->m_deviceFrameworkCompPtr.IsValid()){
		IDeviceStateProvider* providerPtr = m_parentPtr->m_deviceFrameworkCompPtr->GetDeviceStateProvider();
		if (providerPtr != nullptr){
			return providerPtr->GetDeviceState(deviceId);
		}
	}

	return DS_NONE;
}


// public methods of the embedded class DeviceController

CDeviceFrameworkProxyComp::DeviceController::DeviceController(CDeviceFrameworkProxyComp& parent)
	:m_parent(parent)
{
}


// reimplemented (IDeviceController)

const QByteArrayList& CDeviceFrameworkProxyComp::DeviceController::GetSupportedDeviceTypeIds() const
{
	IDeviceController* controllerPtr = GetDeviceController();

	if (controllerPtr != nullptr){
		return controllerPtr->GetSupportedDeviceTypeIds();
	}

	static QByteArrayList empty;

	return empty;
}


const IDeviceStaticInfo* CDeviceFrameworkProxyComp::DeviceController::GetDeviceStaticInfo(const QByteArray& deviceTypeId) const
{
	IDeviceController* controllerPtr = GetDeviceController();

	if (controllerPtr != nullptr){
		return controllerPtr->GetDeviceStaticInfo(deviceTypeId);
	}

	return nullptr;
}


const imtbase::ICollectionInfo& CDeviceFrameworkProxyComp::DeviceController::GetDeviceInstanceList() const
{
	IDeviceController* controllerPtr = GetDeviceController();

	if (controllerPtr != nullptr){
		return controllerPtr->GetDeviceInstanceList();
	}

	static imtbase::CCollectionInfo emptyList;

	return emptyList;
}


DeviceInstanceInfoPtr CDeviceFrameworkProxyComp::DeviceController::GetDeviceInstanceInfo(const QByteArray& deviceId) const
{
	IDeviceController* controllerPtr = GetDeviceController();

	if (controllerPtr != nullptr){
		return controllerPtr->GetDeviceInstanceInfo(deviceId);
	}

	return nullptr;
}


const IDeviceStateProvider& CDeviceFrameworkProxyComp::DeviceController::GetDeviceStateProvider() const
{
	IDeviceController* controllerPtr = GetDeviceController();

	Q_ASSERT(controllerPtr != nullptr);
	if (controllerPtr != nullptr){
		return controllerPtr->GetDeviceStateProvider();
	}

	static imod::TModelWrap<DeviceStateProvider> stateProvider;

	return stateProvider;
}


DeviceAccessorPtr CDeviceFrameworkProxyComp::DeviceController::OpenDevice(const QByteArray& deviceId, const iprm::IParamsSet* paramsPtr)
{
	IDeviceController* controllerPtr = GetDeviceController();

	if (controllerPtr != nullptr){
		return controllerPtr->OpenDevice(deviceId, paramsPtr);
	}

	return nullptr;
}


bool CDeviceFrameworkProxyComp::DeviceController::CloseDevice(const QByteArray& deviceId)
{
	IDeviceController* controllerPtr = GetDeviceController();

	if (controllerPtr != nullptr){
		return controllerPtr->CloseDevice(deviceId);
	}

	return false;
}


// reimplemented (IDeviceEnumerator)

IDeviceEnumerator::StartResult CDeviceFrameworkProxyComp::DeviceController::StartEnumeration(IResultHandler* /*resultHandlerPtr*/)
{
	return SR_FAILED;
}


void CDeviceFrameworkProxyComp::DeviceController::CancelEnumeration()
{
}


// private methods of the embedded class (DeviceController)

IDeviceController* CDeviceFrameworkProxyComp::DeviceController::GetDeviceController() const
{
	if (m_parent.m_deviceFrameworkCompPtr.IsValid()){
		return m_parent.m_deviceFrameworkCompPtr->GetDeviceController();
	}

	return nullptr;
}


} // namespace imtdev


