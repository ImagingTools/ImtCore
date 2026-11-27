// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/CObjectCollection.h>
#include <imtbase/IComplexCollectionFilter.h>
#include <imtbase/TFilterableCollectionWrap.h>


namespace imttest
{


/**
	In-memory object collection for tests that applies the complex filter of a selection ("ComplexFilter")
	as the document delegates of the database do: equality on a field, combined with AND and OR.
	The values of the fields come from GetFieldValue, which knows DocumentId and TypeId; a collection of a product
	overrides it for the fields of its objects. Any other field matches nothing.
	It counts the saves of objects, and the saves with an operation context, which the history is written from.
*/
class CSimObjectCollectionComp:
			public icomp::CComponentBase,
			public imtbase::TFilterableCollectionWrap<imtbase::CObjectCollection>
{
public:
	using BaseClass		= icomp::CComponentBase;
	using BaseClass2	= imtbase::TFilterableCollectionWrap<imtbase::CObjectCollection>;

	I_BEGIN_COMPONENT(CSimObjectCollectionComp);
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(imtbase::IObjectCollectionInfo);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
	I_END_COMPONENT;

	CSimObjectCollectionComp();

	// reimplemented (imtbase::IObjectCollection)
	virtual bool SetObjectData(
				const Id& objectId,
				const istd::IChangeable& object,
				CompatibilityMode mode = CM_WITHOUT_REFS,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual imtbase::IObjectCollectionIterator* CreateObjectCollectionIterator(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;

	int saveCount = 0;
	int contextSaveCount = 0;
	/// Whether the collection cannot give an iterator, as a database that fails the selection.
	bool failsIterator = false;

protected:
	/**
		\brief Value of a field of an object, as the filter compares it. Empty for a field that is not known.
	*/
	virtual QByteArray GetFieldValue(const QByteArray& objectId, const QByteArray& fieldId) const;

	// reimplemented (imtbase::TFilterableCollectionWrap)
	virtual bool IsAcceptedByFilter(const QByteArray& objectId, const iprm::IParamsSet& filterParams) const override;

private:
	bool IsAcceptedByExpression(const QByteArray& objectId, const imtbase::IComplexCollectionFilter::FilterExpression& expression) const;
};


} // namespace imttest

