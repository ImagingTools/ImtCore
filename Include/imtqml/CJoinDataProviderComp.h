#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IItemBasedRepresentationDataProvider.h>
#include <imtqml/CPageDataEnumProviderComp.h>
#include <imtgql/IGqlMutationDataControllerDelegate.h>


namespace imtqml
{


/**
	An implementation for a data model controller
	that combines data from multiple models.
*/
class CJoinDataProviderComp:
		public icomp::CComponentBase,
		public imtbase::IItemBasedRepresentationDataProvider,
		public imtgql::IGqlMutationDataControllerDelegate
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtbase::IItemBasedRepresentationDataProvider BaseClass2;
	typedef imtqml::CPageDataEnumProviderComp PageEnum;

	I_BEGIN_COMPONENT(CJoinDataProviderComp);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_REGISTER_INTERFACE(imtgql::IGqlMutationDataControllerDelegate);
		I_ASSIGN(m_commandIdAttrPtr, "CommandId", "Command Id", true, "");
		I_ASSIGN_MULTI_0(m_representationDataProviderCompPtr, "RepresentationDataProvider", "List of data providers for join", false);
		I_ASSIGN_MULTI_0(m_mutationDataDelegateCompPtr, "MutationDataDelegate", "Mutation data delegate", false);
	I_END_COMPONENT;

	CJoinDataProviderComp();

	// reimplemented (imtbase::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields) override;
	virtual imtbase::CTreeItemModel* UpdateBaseModelFromRepresentation(
				const QList<imtgql::CGqlObject>& params,
				imtbase::CTreeItemModel* baseModel) override;
private:
	I_ATTR(QByteArray, m_commandIdAttrPtr);
	I_MULTIREF(imtbase::IItemBasedRepresentationDataProvider, m_representationDataProviderCompPtr);
	I_MULTIREF(imtgql::IGqlMutationDataControllerDelegate, m_mutationDataDelegateCompPtr);

};


} // namespace imtqml


