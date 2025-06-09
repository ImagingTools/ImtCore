#include <imtdev/CDeviceIdBasedAttributesComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtdev/IDeviceController.h>
#include <imtdev/IDeviceStaticInfo.h>


namespace imtdev
{


// public methods

CDeviceIdBasedAttributesComp::CDeviceIdBasedAttributesComp()
	:m_selectionObserver(*this),
	m_stateProviderObserver(*this)
{
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CDeviceIdBasedAttributesComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_selectionCompPtr.IsValid()){
		m_selectionObserver.RegisterObject(m_selectionCompPtr.GetPtr(), &CDeviceIdBasedAttributesComp::OnSelectionChanged);
	}

	if (m_stateProviderCompPtr.IsValid()){
		m_stateProviderObserver.RegisterObject(m_stateProviderCompPtr.GetPtr(), &CDeviceIdBasedAttributesComp::OnDeviceStateChanged);
	}
}


void CDeviceIdBasedAttributesComp::OnComponentDestroyed()
{
	m_stateProviderObserver.UnregisterAllObjects();
	m_selectionObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// private methods

DeviceInstanceInfoPtr CDeviceIdBasedAttributesComp::GetDeviceInstanceInfo(const QByteArray& deviceId) const
{
	if (!deviceId.isEmpty() && m_controllerCompPtr.IsValid()){
		DeviceInstanceInfoPtr deviceInstanceInfoPtr = m_controllerCompPtr->GetDeviceInstanceInfo(deviceId);
		if (deviceInstanceInfoPtr != nullptr){
			return deviceInstanceInfoPtr;
		}

		return m_controllerCompPtr->GetDeviceInstanceInfo(deviceId);
	}

	return nullptr;
}


void CDeviceIdBasedAttributesComp::UpdateModel()
{
	istd::CChangeNotifier notifier(this);

	ResetData();

	if (m_selectionCompPtr.IsValid() && m_controllerCompPtr.IsValid()){
		imtbase::ISelection::Ids ids = m_selectionCompPtr->GetSelectedIds();
		if (ids.count() != 1){
			return;
		}

		QByteArray deviceId = *ids.begin();

		DeviceInstanceInfoPtr instanceInfoPtr = GetDeviceInstanceInfo(deviceId);
		if (instanceInfoPtr != nullptr){
			const iattr::IAttributesProvider* attributesPtr = instanceInfoPtr->GetAttributes();
			if (attributesPtr != nullptr){
				CopyFrom(*attributesPtr);
			}
		}
	}
}


void CDeviceIdBasedAttributesComp::OnSelectionChanged(const istd::IChangeable::ChangeSet& changeSet, const imtbase::ISelection* objectPtr)
{
	UpdateModel();
}


void CDeviceIdBasedAttributesComp::OnDeviceStateChanged(const istd::IChangeable::ChangeSet& changeSet, const imtdev::IDeviceStateProvider* objectPtr)
{
	UpdateModel();
}


} // namespace imtdev


