#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtdev/IDeviceController.h>


namespace imtdev
{


class CDeviceInstanceListAdapterComp:
			public icomp::CComponentBase,
			public imtbase::ICollectionInfo
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceInstanceListAdapterComp);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
		I_ASSIGN(m_deviceControllerCompPtr, "DeviceController", "Device controller", false, "DeviceController");
	I_END_COMPONENT;

	CDeviceInstanceListAdapterComp();

	// reimplemented (imtbase::ICollectionInfo)
	virtual int GetElementsCount(const iprm::IParamsSet* selectionParamsPtr = nullptr, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual Ids GetElementIds(int offset = 0, int count = -1, const iprm::IParamsSet* selectionParamsPtr = nullptr, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual bool GetSubsetInfo(ICollectionInfo& subsetInfo, int offset = 0, int count = -1, const iprm::IParamsSet* selectionParamsPtr = nullptr, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual QVariant GetElementInfo(const Id& elementId, int infoType, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual bool SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementEnabled(const Id& elementId, bool isEnabled = true, ilog::IMessageConsumer* logPtr = nullptr) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_REF(IDeviceController, m_deviceControllerCompPtr);

	imod::CModelUpdateBridge m_updateBridge;
};


} // namespace imtdev


