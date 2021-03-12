#include <imtlic/CFeaturePackageCollectionProxy.h>


// ImtCore includes
#include <istd/CChangeNotifier.h>


namespace imtlic
{


// public methods

// reimplemented (imtbase::IMultiSelection)

const iprm::IOptionsList* CFeaturePackageCollectionProxy::GetSelectionConstraints() const
{
	return false;
}


imtbase::IMultiSelection::Ids CFeaturePackageCollectionProxy::GetSelectedIds() const
{
	return m_selectedIds;
}


bool CFeaturePackageCollectionProxy::SetSelectedIds(const imtbase::IMultiSelection::Ids& selectedIds)
{
	if (m_selectedIds != selectedIds){
		istd::CChangeNotifier notifier(this);
		

		m_selectedIds = selectedIds;
	}

	return true;
}


} // namespace imtlic


