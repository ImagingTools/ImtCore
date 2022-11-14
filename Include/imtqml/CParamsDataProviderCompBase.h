#pragma once


// ACF includes
#include <imod/TModelWrap.h>
#include <iqt/ITranslationManager.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IContactInfo.h>
#include <imtbase/IItemBasedRepresentationDataProvider.h>
#include <imtgql/IGqlMutationDataControllerDelegate.h>
#include <imtgql/CUserOptionsProviderBase.h>


namespace imtqml
{


/**
	Basic implementation for the controller of the application page representation data model.
*/
class CParamsDataProviderCompBase:
			public icomp::CComponentBase,
			public imtgql::CUserOptionsProviderBase,
			public imtbase::IItemBasedRepresentationDataProvider,
			public imtgql::IGqlMutationDataControllerDelegate
{
public:
	typedef icomp::CComponentBase BaseClass;

	enum ComponentType
	{
		/**
			Component type is Unknown.
		*/
		CT_UNKNOWN,

		/**
			Component type is Text.
		*/
		CT_TEXT_INPUT,

		/**
			Component type is Integer.
		*/
		CT_INTEGER_INPUT,

		/**
			Component type is ComboBox.
		*/
		CT_COMBOBOX,

		/**
			Component type is Button.
		*/
		CT_BUTTON,

		/**
			Component type is Text Label.
		*/
		CT_TEXT_LABEL
	};

	I_BEGIN_COMPONENT(CParamsDataProviderCompBase);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_REGISTER_INTERFACE(imtgql::IGqlMutationDataControllerDelegate);
		I_ASSIGN(m_paramIdAttrPtr, "ParamId", "ID of the param", true, "");
		I_ASSIGN(m_paramNameAttrPtr, "ParamName", "Name of the param", false, "");
		I_ASSIGN(m_paramComponentTypeAttrPtr, "ComponentType", "Type of component\n0 - UNKNOWN\n1 - TEXT\n2 - INTEGER\n3 - COMBOBOX\n4 - BUTTON\n5 - TEXT LABEL", false, 0);
		I_ASSIGN_MULTI_0(m_paramSubElementsCompPtr, "ParamSubElements", "Subelements of the param", false);
		I_ASSIGN_MULTI_0(m_mutationDataDelegateCompPtr, "MutationDataDelegate", "Mutation data delegate", false);
		I_ASSIGN(m_parameterCompPtr, "Parameter", "Parameter of params data", false, "");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT;

protected:
	const imtbase::CTreeItemModel* GetElementModel(const QByteArray& elemementId, const imtbase::CTreeItemModel* elementsModelPtr) const;

	// reimplemented (imtbase::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields, const imtgql::IGqlContext* gqlContext) override;

	// reimplemented (imtgql::IGqlMutationDataControllerDelegate)
	virtual imtbase::CTreeItemModel* UpdateBaseModelFromRepresentation(
			const QList<imtgql::CGqlObject>& params,
			imtbase::CTreeItemModel* baseModel,
			const imtgql::IGqlContext* gqlContext) override;

protected:
	I_ATTR(QByteArray, m_paramIdAttrPtr);
	I_TEXTATTR(m_paramNameAttrPtr);
	I_ATTR(int, m_paramComponentTypeAttrPtr);
	I_MULTIREF(imtbase::IItemBasedRepresentationDataProvider, m_paramSubElementsCompPtr);
	I_MULTIREF(imtgql::IGqlMutationDataControllerDelegate, m_mutationDataDelegateCompPtr);
	I_REF(iser::ISerializable, m_parameterCompPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
};


} // namespace imtqml


