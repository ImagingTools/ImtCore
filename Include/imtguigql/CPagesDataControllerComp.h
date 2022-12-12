#pragma once


// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>

#include <imtgql/IGqlModelEditor.h>
#include <imtgql/IItemBasedRepresentationDataProvider.h>


namespace imtguigql
{


class CPagesDataControllerComp: public imtgql::CGqlRepresentationDataControllerCompBase
{
public:
	typedef imtgql::CGqlRepresentationDataControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CPagesDataControllerComp);
		I_ASSIGN(m_pagesDataProviderCompPtr, "PagesDataProviderCompPtr", "List of pages providers for join", true, "");
		I_ASSIGN(m_mutationDataDelegateCompPtr, "MutationDataDelegate", "Mutation data delegate", true, "");
		I_ASSIGN(m_decoratorSourceAttrPtr, "DecoratorSource", "Source of the decorator page", false, "");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlRepresentationDataController)
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtgql::IItemBasedRepresentationDataProvider, m_pagesDataProviderCompPtr);
	I_REF(imtgql::IGqlModelEditor, m_mutationDataDelegateCompPtr);
	I_TEXTATTR(m_decoratorSourceAttrPtr);
};


} // namespace imtguigql


