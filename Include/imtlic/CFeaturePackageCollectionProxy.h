#pragma once


// ImtCore includes
#include <imtbase/IMultiSelection.h>
#include <imtbase/CStaticObjectCollection.h>
#include <imtlic/IFeaturePackage.h>
#include <imtlic/CFeaturePackage.h>


namespace imtlic
{


class CFeaturePackageCollectionProxy:
			public imtbase::CStaticObjectCollection,
			virtual public imtbase::IMultiSelection
{
public:
	
	// reimplemented (imtbase::IMultiSelection)
	virtual const iprm::IOptionsList* GetSelectionConstraints() const override;
	virtual imtbase::IMultiSelection::Ids GetSelectedIds() const override;
	virtual bool SetSelectedIds(const imtbase::IMultiSelection::Ids& selectedIds) override;

private:
	imtbase::IMultiSelection::Ids m_selectedIds;
};


} // namespace imtlic


