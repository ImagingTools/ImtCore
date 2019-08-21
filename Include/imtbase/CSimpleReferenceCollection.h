#pragma once


// Qt includes
#include <QtCore/QVector>

// Acula includes
#include <imtbase/IReferenceCollection.h>


namespace imtbase
{


class CSimpleReferenceCollection: virtual public imtbase::IReferenceCollection
{
public:
	// reimplemented (IReferenceCollection)
	virtual bool InsertReference(const Id& resourceId) override;
	virtual bool RemoveReference(const Id& resourceId) override;
	virtual bool RemoveAllReferences() override;

	// reimplemented (IObjectCollectionInfo)
	virtual const iprm::IOptionsList * GetObjectTypesInfo() const override;
	virtual Id GetObjectTypeId(const QByteArray & objectId) const override;

	// reimplemented (imtbase::ICollectionInfo)
	virtual Ids GetElementIds() const override;
	virtual QString GetElementName(const QByteArray& elementId) const override;
	virtual QString GetElementDescription(const QByteArray& elementId) const override;
	virtual bool IsElementEnabled(const QByteArray& elementId) const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
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
	};
	typedef QVector<Reference> References;
	
	References m_references;
};


} // namespace imtbase


