#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IItemBasedRepresentationDataProvider.h>


namespace imtqml
{


/**
	Returns object view path for a collection of objects.
*/
class CObjectViewDataProviderComp:
		public icomp::CComponentBase,
		public imtbase::IItemBasedRepresentationDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CObjectViewDataProviderComp);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_ASSIGN(m_objectViewModelIdAttrPtr, "ObjectViewModelId", "Object view model ID", true, "");
		I_ASSIGN(m_objectViewAttrPtr, "ObjectViewPath", "Object view path", true, "");
	I_END_COMPONENT;

	// reimplemented (imtbase::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields) override;

private:
	I_ATTR(QByteArray, m_objectViewModelIdAttrPtr);
	I_ATTR(QByteArray, m_objectViewAttrPtr);
};


} // namespace imtqml


