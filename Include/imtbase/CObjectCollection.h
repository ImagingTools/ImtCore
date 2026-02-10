// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	virtual int GetOperationFlags(const QByteArray& objectId = QByteArray()) const override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

protected:
	// reimplemented (CObjectCollectionBase)
	virtual istd::IChangeableUniquePtr CreateObjectInstance(const QByteArray& typeId) const override;
	virtual IObjectCollection* CreateSubCollectionInstance() const override;

private:
	int m_operationFlags;
};


} // namespace imtbase


