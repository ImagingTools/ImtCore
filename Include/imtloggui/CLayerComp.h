#pragma once


// Acf includes
#include <imod/TSingleModelObserverBase.h>
#include <imod/CModelProxy.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/ILayerController.h>
#include <imtbase/CStaticObjectCollection.h>


namespace imtloggui
{


class CLayerComp:
			public icomp::CComponentBase,
			public imtbase::CStaticObjectCollection,
			virtual public imtloggui::ILayerController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLayerComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_SUBELEMENT(RepresentationProxy);
		I_REGISTER_SUBELEMENT_INTERFACE(RepresentationProxy, imod::IModel, ExtractRepresentationProxy);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "RepresentationIds", "Representation ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "RepresentationNames", "Representation names", false);
		I_ASSIGN_MULTI_0(m_representationCompPtr, "Representations", "Representations", false);
	I_END_COMPONENT

	// reimplemented (icomp::CComponentBase)
	virtual QByteArray GetActiveRepresentationId() const override;
	virtual bool SetActiveRepresentationId(const QByteArray& representationId) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	template <typename InterfaceType>
	static InterfaceType* ExtractRepresentationProxy(CLayerComp& component)
	{
		return &component.m_representationProxy;
	}

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_MULTIREF(istd::IChangeable, m_representationCompPtr);

	QByteArray m_activerRepresentationId;
	imod::CModelProxy m_representationProxy;
};


} // namespace imtloggui


