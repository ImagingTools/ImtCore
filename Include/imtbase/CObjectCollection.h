#pragma once


// ACF includes
#include <istd/TComposedFactory.h>

// ImtCore includes
#include <imtbase/CObjectCollectionBase.h>


namespace imtbase
{


/**
	Implementation of a general data object collection.
	\ingroup Collection
*/
class CObjectCollection:
			public CObjectCollectionBase,
			public istd::TComposedFactory<istd::IChangeable>
{
public:
	typedef CObjectCollectionBase BaseClass;
	typedef istd::TComposedFactory<istd::IChangeable> BaseClass2;

	CObjectCollection();

	void SetOperationFlags(int flags, const QByteArray& objectId = QByteArray());

	// reimplemented (imtbase::IObjectCollection)
	int GetOperationFlags(const QByteArray& objectId = QByteArray()) const override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

protected:
	// reimplemented (CObjectCollectionBase)
	virtual istd::IChangeable* CreateObjectInstance(const QByteArray& typeId) const;

private:
	int m_operationFlags;
};


} // namespace imtbase


