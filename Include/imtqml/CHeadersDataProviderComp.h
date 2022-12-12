#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <iqt/ITranslationManager.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtgql/IItemBasedRepresentationDataProvider.h>
#include <imtbase/imtbase.h>


namespace imtqml
{


/**
	Returns headers for a collection of objects.
*/
class CHeadersDataProviderComp:
		public icomp::CComponentBase,
		public imtgql::IItemBasedRepresentationDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CHeadersDataProviderComp);
		I_REGISTER_INTERFACE(imtgql::IItemBasedRepresentationDataProvider);
		I_ASSIGN(m_headersModelIdAttrPtr, "HeadersModelId", "Headers model-ID", true, "");
		I_ASSIGN_MULTI_0(m_headersIdsAttrPtr, "HeadersIds", "Headers Ids", true);
		I_ASSIGN_MULTI_0(m_headersSearchByFilterAttrPtr, "SearchByFilterIds", "Search by filter", false);
		I_ASSIGN_MULTI_0(m_headersNamesAttrPtr, "HeadersNames", "Headers names", true);
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT;

	// reimplemented (imtgql::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetRepresentation(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields, const imtgql::IGqlContext* gqlContext) override;

private:
	I_ATTR(QByteArray, m_headersModelIdAttrPtr);
	I_MULTIATTR(QString, m_headersIdsAttrPtr);
	I_MULTIATTR(QString, m_headersSearchByFilterAttrPtr);
	I_MULTITEXTATTR(m_headersNamesAttrPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
};


} // namespace imtqml


