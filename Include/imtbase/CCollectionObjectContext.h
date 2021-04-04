#pragma once


// Qt includes


// ImtCore includes
#include <imtbase/ICollectionObjectContext.h>
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


/**
	Implementation of a general collection object context information.
	\ingroup Collection
*/
class CCollectionObjectContext: virtual public ICollectionObjectContext
{
public:
	CCollectionObjectContext();

	virtual void SetObjectContext(const imtbase::IObjectCollection* objectCollectionPtr, const QByteArray& objectId);

	// reimplemented (ICollectionObjectContext)
	virtual const imtbase::IObjectCollection* GetObjectCollectionPtr() const override;
	virtual QByteArray GetObjectId() const override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	const IObjectCollection* m_objectCollectionPtr;
	QByteArray m_objectId;
};


} // namespace imtbase


