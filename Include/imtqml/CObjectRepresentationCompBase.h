#pragma once


// ACF includes
#include <imod/TModelWrap.h>
#include <iqt/ITranslationManager.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtgql/IItemBasedRepresentationDataProvider.h>
#include <imtgql/IGqlModelEditor.h>
#include <imtauth/IContactInfo.h>


namespace imtqml
{


/**
	Basic implementation for object representation.
*/
class CObjectRepresentationCompBase:
			public icomp::CComponentBase,
			public imtgql::IItemBasedRepresentationDataProvider,
			public imtgql::IGqlModelEditor
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CObjectRepresentationCompBase);
		I_REGISTER_INTERFACE(imtgql::IItemBasedRepresentationDataProvider);
		I_REGISTER_INTERFACE(imtgql::IGqlModelEditor);
		I_ASSIGN(m_paramIdAttrPtr, "ParamId", "ID of the param", true, "");
		I_ASSIGN(m_paramNameAttrPtr, "ParamName", "Name of the param", false, "");
		I_ASSIGN(m_qmlPathAttrPtr, "PathToQml", "Path to qml component", false, "");
		I_ASSIGN_MULTI_0(m_mutationDataDelegateCompPtr, "MutationDataDelegate", "Mutation data delegate", false);
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT;

protected:
	const imtbase::CTreeItemModel* GetElementModel(const QByteArray& elemementId, const imtbase::CTreeItemModel* elementsModelPtr) const;

	// reimplemented (imtgql::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetRepresentation(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields, const imtgql::IGqlContext* gqlContext) override;

	// reimplemented (imtgql::IGqlModelEditor)
	virtual bool UpdateModelFromRepresentation(
			const QList<imtgql::CGqlObject>& params,
			imtbase::CTreeItemModel* baseModel,
			const imtgql::IGqlContext* gqlContext) override;

protected:
	I_ATTR(QByteArray, m_paramIdAttrPtr);
	I_TEXTATTR(m_paramNameAttrPtr);
	I_TEXTATTR(m_qmlPathAttrPtr);
	I_MULTIREF(imtgql::IGqlModelEditor, m_mutationDataDelegateCompPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
};


} // namespace imtqml


