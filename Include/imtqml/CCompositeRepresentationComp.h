#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtgql/IItemBasedRepresentationDataProvider.h>
#include <imtgql/IGqlModelEditor.h>


namespace imtqml
{


/**
	An implementation for a data model controller
	that combines data from multiple models.
*/
class CCompositeRepresentationComp:
		public icomp::CComponentBase,
		public imtgql::IItemBasedRepresentationDataProvider,
		public imtgql::IGqlModelEditor
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtgql::IItemBasedRepresentationDataProvider BaseClass2;

	I_BEGIN_COMPONENT(CCompositeRepresentationComp);
		I_REGISTER_INTERFACE(imtgql::IItemBasedRepresentationDataProvider);
		I_REGISTER_INTERFACE(imtgql::IGqlModelEditor);
		I_ASSIGN(m_commandIdAttrPtr, "CommandId", "Command Id", true, "");
		I_ASSIGN_MULTI_0(m_representationDataProviderCompPtr, "RepresentationDataProvider", "List of data providers for join", false);
		I_ASSIGN_MULTI_0(m_mutationDataDelegateCompPtr, "MutationDataDelegate", "Mutation data delegate", false);
	I_END_COMPONENT;

	// reimplemented (imtgql::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetRepresentation(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields, const imtgql::IGqlContext* gqlContext) override;
	virtual bool UpdateModelFromRepresentation(
				const QList<imtgql::CGqlObject>& params,
				imtbase::CTreeItemModel* baseModel,
				const imtgql::IGqlContext* gqlContext) override;
private:
	I_ATTR(QByteArray, m_commandIdAttrPtr);
	I_MULTIREF(imtgql::IItemBasedRepresentationDataProvider, m_representationDataProviderCompPtr);
	I_MULTIREF(imtgql::IGqlModelEditor, m_mutationDataDelegateCompPtr);
};


} // namespace imtqml


