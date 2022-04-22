#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IItemBasedRepresentationDataProvider.h>


namespace imtqml
{


/**
	Returns headers for a collection of objects
*/
class CHeadersDataProviderComp:
		public icomp::CComponentBase,
		public imtbase::IItemBasedRepresentationDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtbase::IItemBasedRepresentationDataProvider BaseClass2;

	I_BEGIN_COMPONENT(CHeadersDataProviderComp);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_ASSIGN(m_headersModelIdAttrPtr, "HeadersModelId", "Headers model-ID", true, "");
		I_ASSIGN_MULTI_0(m_headersIdsAttrPtr, "HeadersIds", "Headers Ids", true);
		I_ASSIGN_MULTI_0(m_headersNamesAttrPtr, "HeadersNames", "Headers names", true);
	I_END_COMPONENT;

	CHeadersDataProviderComp();

	// reimplemented (imtbase::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields) override;

private:
	I_ATTR(QByteArray, m_headersModelIdAttrPtr);
	I_MULTIATTR(QString, m_headersIdsAttrPtr);
	I_MULTITEXTATTR(m_headersNamesAttrPtr);
};


} // namespace imtqml


