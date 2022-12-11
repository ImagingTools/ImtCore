#pragma once


// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/IGqlContext.h>
#include <imtgql/CGqlObject.h>


namespace imtbase
{


class IItemBasedRepresentationDataProvider: virtual public istd::IPolymorphic
{
public:
	virtual QByteArray GetModelId() const = 0;
	virtual CTreeItemModel* GetTreeItemModel(
				const QList<imtgql::CGqlObject>& params,
				const QByteArrayList& fields,
				const imtgql::IGqlContext* gqlContext = nullptr) = 0;
};


} // namespace imtbase


