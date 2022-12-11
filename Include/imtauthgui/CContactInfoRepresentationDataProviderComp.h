#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IContactInfo.h>
#include <imtrest/IRepresentationDataProvider.h>
#include <imtbase/IItemBasedRepresentationDataProvider.h>


namespace imtauthgui
{


/**
	Represenatation controller for contact info data model.
	\ingroup LicenseManagement
*/
class CContactInfoRepresentationDataProviderComp:
		public icomp::CComponentBase,
		public imtrest::IRepresentationDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CContactInfoRepresentationDataProviderComp);
		I_REGISTER_INTERFACE(imtrest::IRepresentationDataProvider);
		I_ASSIGN(m_itemBasedRepresentationDataProvider, "ItemBasedRepresentationDataProvider", "Item based representation data provider", true, "ItemBasedRepresentationDataProvider");
	I_END_COMPONENT;

	CContactInfoRepresentationDataProviderComp();

	// reimplemented (imtauthgui::IRepresentationDataProvider)
	virtual bool GetRepresentationData(imtrest::IRepresentationDataProvider::Format format,
				QByteArray& representationData,
				const QList<QByteArray> &query, const imtrest::QueryParams &params) override;
	virtual bool SetRepresentationData(
				imtrest::IRepresentationDataProvider::Format format,
				QByteArray& representationData,
				const QByteArray& commandId = "") override;
private:
	I_REF(imtbase::IItemBasedRepresentationDataProvider, m_itemBasedRepresentationDataProvider);


};


} // namespace imtauthgui


