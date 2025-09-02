#include <imtdev/CDeviceIdBasedAttributesComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtdev/IDeviceController.h>
#include <imtdev/IDeviceSpecification.h>


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

DeviceInstancePtr CDeviceIdBasedAttributesComp::GetDeviceInstance(const QByteArray& deviceId) const
{
	DeviceInstancePtr deviceInstanceInfoPtr = m_controllerCompPtr->GetDeviceInstance(deviceId);
	if (deviceInstanceInfoPtr != nullptr){
		return deviceInstanceInfoPtr;
	}

	return m_controllerCompPtr->GetDeviceInstance(deviceId);
}


void CDeviceIdBasedAttributesComp::UpdateModel()
{
	m_staticAttrs.ResetData();
	m_instanceAttrs.ResetData();

	if (m_selectionCompPtr.IsValid() && m_controllerCompPtr.IsValid()){
		imtbase::ISelection::Ids ids = m_selectionCompPtr->GetSelectedIds();
		if (ids.count() != 1){
			return;
		}

		QByteArray deviceId = *ids.begin();

		const IDeviceSpecification* staticInfoPtr = nullptr;

		DeviceInstancePtr instancePtr = GetDeviceInstance(deviceId);
		if (instancePtr != nullptr){
			staticInfoPtr = &instancePtr->GetDeviceSpecification();

			const iattr::IAttributesProvider* attributesPtr = instancePtr->GetAttributes();
			if (attributesPtr != nullptr){
				m_instanceAttrs.CopyFrom(*attributesPtr);
			}
		}

		if (staticInfoPtr == nullptr){
			staticInfoPtr = m_controllerCompPtr->GetDeviceStaticInfo(deviceId);
		}

		if (staticInfoPtr != nullptr){
			const iattr::IAttributesProvider* attributesPtr = staticInfoPtr->GetAttributes();
			if (attributesPtr != nullptr){
				m_staticAttrs.CopyFrom(*attributesPtr);
			}
		}
	}
}


void CDeviceIdBasedAttributesComp::OnSelectionChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const imtbase::ISelection* /*objectPtr*/)
{
	UpdateModel();
}


void CDeviceIdBasedAttributesComp::OnDeviceStateChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const imtdev::IDeviceStateProvider* /*objectPtr*/)
{
	UpdateModel();
}


} // namespace imtdev


