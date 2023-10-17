#pragma once


// ACF includes
#include <istd/TComposedFactory.h>
#include <icomp/CComponentBase.h>
#include <istd/TDelPtr.h>
#include <istd/TPointerVector.h>

// ImtCore includes
#include <imtbase/CFilterCollectionProxy.h>


namespace imtbase
{


/**
	Implementation of a cached data object collection.
	\ingroup Collection
*/
class CCachedObjectCollectionComp:
			public icomp::CComponentBase,
			public imtbase::IObjectCollection
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCachedObjectCollectionComp);
		I_REGISTER_INTERFACE(IObjectCollection);
		I_REGISTER_INTERFACE(IObjectCollectionInfo);
		I_REGISTER_INTERFACE(ICollectionInfo);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
		I_ASSIGN(m_isCacheObjectPtr, "IsCacheObject", "Is object data cached", true, false);
		I_ASSIGN(m_cacheLimitPtr, "CacheLimit", "Limiting cached stack data", true, 1000);
	I_END_COMPONENT;


	CCachedObjectCollectionComp();

	void SetOperationFlags(int flags, const QByteArray& objectId = QByteArray());

	// reimplemented (IObjectCollection)
	virtual const IRevisionController* GetRevisionController() const override;
	virtual const ICollectionDataController* GetDataController() const override;
	virtual int GetOperationFlags(const Id& objectId = Id()) const override;
	virtual Id InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				DataPtr defaultValuePtr = DataPtr(),
				const Id& proposedElementId = Id(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RemoveElement(const Id& elementId, const IOperationContext* operationContextPtr = nullptr) override;
	virtual const istd::IChangeable* GetObjectPtr(const Id& objectId) const override;
	virtual bool GetObjectData(const Id& objectId, DataPtr& dataPtr) const override;
	virtual bool SetObjectData(const Id& objectId, const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS, const IOperationContext* operationContextPtr = nullptr) override;
	virtual IObjectCollection* CreateSubCollection(int offset, int count, const iprm::IParamsSet *selectionParamsPtr) const override;
	virtual imtbase::IObjectCollectionIterator* CreateObjectCollectionIterator(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;

	// reimplemented (IObjectCollectionInfo)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual QByteArray GetObjectTypeId(const Id& objectId) const override;
	virtual idoc::MetaInfoPtr GetDataMetaInfo(const Id& objectId) const override;

	// reimplemented (ICollectionInfo)
	virtual int GetElementsCount(const iprm::IParamsSet* selectionParamPtr = nullptr) const override;
	virtual Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual bool GetSubsetInfo(
				ICollectionInfo& subsetInfo,
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual QVariant GetElementInfo(const Id& elementId, int infoType) const override;
	virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId) const override;
	virtual bool SetElementName(const Id& elementId, const QString& name) override;
	virtual bool SetElementDescription(const Id& elementId, const QString& description) override;
	virtual bool SetElementEnabled(const Id& elementId, bool isEnabled = true) override;

protected:
	struct PaginationData
	{
		PaginationData(int aOffset, int aCount, const QByteArray& aSelectionParamsData, IObjectCollection* aCachePtr)
			: offset(aOffset), count(aCount), selectionParamsData(aSelectionParamsData), cachePtr(aCachePtr)
		{
		}
		int offset;
		int count;
		QByteArray selectionParamsData;
		istd::TDelPtr<IObjectCollection> cachePtr;
	};

	PaginationData* CheckCache(
			int offset = 0,
			int count = -1,
			const iprm::IParamsSet* selectionParamsPtr = nullptr) const;

private:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_ATTR(bool, m_isCacheObjectPtr);
	I_ATTR(int, m_cacheLimitPtr);

	mutable istd::TPointerVector<PaginationData> m_paginationDataList;
	int m_operationFlags;
	iprm::IParamsSet* m_selectionParamsCachePtr;
	mutable QByteArray m_selectionParamsData;
};


} // namespace imtbase


