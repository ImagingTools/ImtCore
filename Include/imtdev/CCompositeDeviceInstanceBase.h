#pragma once


// ACF include
#include <imod/CModelUpdateBridge.h>
#include <imod/TModelWrap.h>
#include <istd/TComposedFactory.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtdev/IEditableCompositeDeviceInstance.h>
#include <imtdev/CDeviceInstanceBase.h>


namespace imtdev
{


class CCompositeDeviceInstanceBase : public CDeviceInstanceBase, virtual public IEditableCompositeDeviceInstance
{
public:
	typedef CDeviceInstanceBase BaseClass;

	CCompositeDeviceInstanceBase();
	~CCompositeDeviceInstanceBase();

	// reimplemented (IEditableCompositeDeviceInstance)
	virtual QByteArray AddSubDevice(
		const QByteArray& deviceTypeId,
		const QString& name,
		const QString& description = QString(),
		const istd::IChangeable* defaultValuePtr = nullptr,
		const QByteArray& proposedId = QByteArray()) override;
	virtual bool RemoveSubDevice(const QByteArray& id) override;

	// reimplemented (ICompositeDeviceInstance)
	virtual const ICompositeDeviceSpecification* GetCompositeDeviceSpecification() const override;
	virtual QSet<QByteArray> GetSupportedSubDeviceTypeIds() const override;
	virtual const imtbase::ICollectionInfo& GetSubDeviceList() const override;
	virtual const IDeviceInstance* GetSubDeviceInstance(const QByteArray& subDeviceId) const override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	virtual IDeviceInstance* CreateDeviceInstance(const QByteArray& deviceTypeId) const;

private:
	class SubDeviceCollectionInfo : virtual public imtbase::ICollectionInfo
	{
	public:
		void SetParent(CCompositeDeviceInstanceBase* parentPtr);

		// reimpolemented (imtbase::ICollectionInfo)
		virtual int GetElementsCount(
			const iprm::IParamsSet* selectionParamsPtr = nullptr,
			ilog::IMessageConsumer* logPtr = nullptr) const override;
		virtual Ids GetElementIds(
			int offset = 0,
			int count = -1,
			const iprm::IParamsSet* selectionParamsPtr = nullptr,
			ilog::IMessageConsumer* logPtr = nullptr) const override;
		virtual bool GetSubsetInfo(
			ICollectionInfo& subsetInfo,
			int offset = 0,
			int count = -1,
			const iprm::IParamsSet* selectionParamsPtr = nullptr,
			ilog::IMessageConsumer* logPtr = nullptr) const override;
		virtual QVariant GetElementInfo(
					const Id& elementId,
					int infoType,
					ilog::IMessageConsumer* logPtr = nullptr) const override;
		virtual idoc::MetaInfoPtr GetElementMetaInfo(
					const Id& elementId,
					ilog::IMessageConsumer* logPtr = nullptr) const override;
		virtual bool SetElementName(
					const Id& elementId,
					const QString& name,
					ilog::IMessageConsumer* logPtr = nullptr) override;
		virtual bool SetElementDescription(
					const Id& elementId,
					const QString& description,
					ilog::IMessageConsumer* logPtr = nullptr) override;
		virtual bool SetElementEnabled(
					const Id& elementId,
					bool isEnabled = true,
					ilog::IMessageConsumer* logPtr = nullptr) override;

	private:
		CCompositeDeviceInstanceBase* m_parentPtr;
	};

private:
	void RemoveAllSubDevices();

private:
	struct SubDeviceItem
	{
		QString name;
		QString description;
		DeviceInstancePtr instancePtr;
	};

	QMap<QByteArray, SubDeviceItem> m_subDeviceMap;
	imod::TModelWrap<SubDeviceCollectionInfo> m_subDeviceCollection;
};


} // namespace imtdev


