#pragma once


// Qt includes
#include <QtCore/QVector>

// ImtCore includes
#include <imtbase/IReferenceCollection.h>
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


/**
	Simple implementation for a collection of object references.
*/
class CSimpleReferenceCollection: virtual public IReferenceCollection
{
public:
	CSimpleReferenceCollection();

	/**
		Register the source object collection.
	*/
	void RegisterSourceCollection(const imtbase::IObjectCollection* sourceCollectionInfoPtr);

	/**
		Get access to the source object collection.
	*/
	const imtbase::IObjectCollection* GetSourceCollection() const;

	// reimplemented (IReferenceCollection)
	virtual bool InsertReference(const Id& objectId) override;
	virtual bool RemoveReference(const Id& objectId) override;
	virtual bool RemoveAllReferences() override;

	// reimplemented (IObjectCollectionInfo)
	virtual const iprm::IOptionsList * GetObjectTypesInfo() const override;
	virtual Id GetObjectTypeId(const QByteArray& objectId) const override;
	virtual idoc::MetaInfoPtr GetDataMetaInfo(const Id& objectId) const override;

	// reimplemented (imtbase::ICollectionInfo)
	virtual int GetElementsCount(
				const iprm::IParamsSet* selectionParamPtr = nullptr,
				const Id& parentId = Id(),
				int iterationFlags = IF_RECURSIVE | IF_LEAF_ONLY) const override;
	virtual Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				const Id& parentId = Id(),
				int iterationFlags = IF_RECURSIVE | IF_LEAF_ONLY) const override;
	virtual Id GetParentId(const Id& elementId) const override;
	virtual Ids GetElementPath(const Id& elementId) const override;
	virtual bool IsBranch(const Id& elementId) const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;
	virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId) const override;
	virtual bool SetElementName(const Id& elementId, const QString& name) override;
	virtual bool SetElementDescription(const Id& elementId, const QString& description) override;
	virtual bool SetElementEnabled(const Id& elementId, bool isEnabled = true) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const istd::IChangeable& object) const override;
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	int FindReference(const QByteArray& id) const;

private:
	struct Reference
	{
		QByteArray id;
		QString name;
		QString description;
		QByteArray typeId;

		bool operator ==(const Reference& other) const
		{
			return (id == other.id) && (name == other.name) && (description == other.description) && (typeId == other.typeId);
		}
		
		bool operator !=(const Reference& other) const
		{
			return !operator==(other);
		}
	};
	typedef QVector<Reference> References;
	
	References m_references;

	const imtbase::IObjectCollection* m_sourceCollectionInfoPtr;
};


} // namespace imtbase


