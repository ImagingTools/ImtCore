#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/ILayerController.h>
#include <imtbase/IObjectProvider.h>
#include <imtbase/CStaticObjectCollection.h>


namespace imtloggui
{


class CLayerComp:
			public icomp::CComponentBase,
			public imtbase::CStaticObjectCollection,
			virtual public imtloggui::ILayerController,
			virtual public imtbase::IObjectProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLayerComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "RepresentationIds", "Representation ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "RepresentationNames", "Representation names", false);
		I_ASSIGN_MULTI_0(m_representationCompPtr, "Representations", "Representations", false);
	I_END_COMPONENT

	// reimplemented (imtloggui::ILayerController)
	virtual QByteArray GetActiveRepresentationId() const override;
	virtual bool SetActiveRepresentationId(const QByteArray& representationId) override;

	// reimplemented (imtbase::IObjectProvider)
	virtual const istd::IChangeable* GetDataObject() const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_MULTIREF(istd::IChangeable, m_representationCompPtr);

	QByteArray m_activeRepresentationId;
};


} // namespace imtloggui


