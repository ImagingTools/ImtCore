#pragma once


// ACF include
#include <imod/CModelUpdateBridge.h>
#include <imod/TModelWrap.h>
#include <istd/TComposedFactory.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtdev/ICompositeDeviceInstanceInfoEditor.h>
#include <imtdev/IDeviceInstanceInfoEditor.h>
#include <imtdev/CDeviceInstanceInfoBase.h>


namespace imtdev
{


class CCompositeDeviceInstanceInfoBase : public CDeviceInstanceInfoBase, virtual public ICompositeDeviceInstanceInfoEditor
{
public:
	typedef CDeviceInstanceInfoBase BaseClass;

	CCompositeDeviceInstanceInfoBase();
	~CCompositeDeviceInstanceInfoBase();

	// reimplemented (ICompositeDeviceInstanceInfoEditor)
	virtual QByteArray AddSubDevice(
		const QByteArray& deviceTypeId,
		const QString& name,
		const QString& description = QString(),
		const istd::IChangeable* defaultValuePtr = nullptr,
		const QByteArray& proposedId = QByteArray()) override;
	virtual bool RemoveSubDevice(const QByteArray& id) override;

	// reimplemented (ICompositeDeviceInstanceInfo)
	virtual const ICompositeDeviceStaticInfo* GetCompositeStaticInfo() const override;
	virtual QSet<QByteArray> GetSupportedSubDeviceTypeIds() const override;
	virtual const imtbase::ICollectionInfo& GetSubDeviceList() const override;
	virtual const IDeviceInstanceInfo* GetSubDeviceInstanceInfo(const QByteArray& subDeviceId) const override;

	// reimplemented (istd::IChangeable)
	int GetSupportedOperations() const override;
	bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	virtual IDeviceInstanceInfo* CreateDeviceInstanceInfo(const QByteArray& deviceTypeId) const;

private:
	class SubDeviceCollectionInfo : virtual public imtbase::ICollectionInfo
	{
	public:
		void SetParent(CCompositeDeviceInstanceInfoBase* parentPtr);

		// reimpolemented (imtbase::ICollectionInfo)
		int GetElementsCount(
			const iprm::IParamsSet* selectionParamsPtr = nullptr,
			ilog::IMessageConsumer* logPtr = nullptr) const override;
		Ids GetElementIds(
			int offset = 0,
			int count = -1,
			const iprm::IParamsSet* selectionParamsPtr = nullptr,
			ilog::IMessageConsumer* logPtr = nullptr) const override;
		bool GetSubsetInfo(
			ICollectionInfo& subsetInfo,
			int offset = 0,
			int count = -1,
			const iprm::IParamsSet* selectionParamsPtr = nullptr,
			ilog::IMessageConsumer* logPtr = nullptr) const override;
		QVariant GetElementInfo(
			const Id& elementId, int infoType, ilog::IMessageConsumer* logPtr = nullptr) const override;
		idoc::MetaInfoPtr GetElementMetaInfo(
			const Id& elementId, ilog::IMessageConsumer* logPtr = nullptr) const override;
		bool SetElementName(
			const Id& elementId, const QString& name, ilog::IMessageConsumer* logPtr = nullptr) override;
		bool SetElementDescription(
			const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr = nullptr) override;
		bool SetElementEnabled(
			const Id& elementId, bool isEnabled = true, ilog::IMessageConsumer* logPtr = nullptr) override;

	private:
		CCompositeDeviceInstanceInfoBase* m_parentPtr;
	};

private:
	void RemoveAllSubDevices();

private:
	struct SubDeviceItem
	{
		QString name;
		QString description;
		DeviceInstanceInfoPtr instanceInfoPtr;
	};

	mutable CompositeDeviceStaticInfoPtr m_compositeStaticInfoPtr;
	QMap<QByteArray, SubDeviceItem> m_subDeviceMap;
	imod::TModelWrap<SubDeviceCollectionInfo> m_subDeviceCollection;
};


} // namespace imtdev
