#pragma once


// Acf includes
#include <iprm/IEnableableParam.h>
#include <iprm/IOptionsList.h>
#include <iprm/ISelectionParam.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/ILayerController.h>
#include <imtbase/CStaticObjectCollection.h>


namespace imtloggui
{


class CLayerComp:
			public icomp::CComponentBase,
			public imtbase::CStaticObjectCollection,
			virtual public iprm::IOptionsList,
			virtual public iprm::ISelectionParam,
			virtual public imtloggui::ILayerController,
			virtual public iprm::IEnableableParam
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtbase::CStaticObjectCollection BaseClass2;

	I_BEGIN_COMPONENT(CLayerComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(iprm::IOptionsList);
		I_REGISTER_INTERFACE(iprm::ISelectionParam);
		I_REGISTER_INTERFACE(imtloggui::ILayerController);
		I_REGISTER_INTERFACE(iprm::IEnableableParam);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "RepresentationIds", "Representation ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "RepresentationNames", "Representation names", false);
		I_ASSIGN_MULTI_0(m_representationCompPtr, "Representations", "Representations", false);
		I_ASSIGN_MULTI_0(m_representationControllerCompPtr, "RepresentationControllers", "Representation controllers", false);
	I_END_COMPONENT

	CLayerComp();

	// reimplemented (iprm::IEnableableParam)
	virtual bool IsEnabled() const override;
	virtual bool IsEnablingAllowed() const override;
	virtual bool SetEnabled(bool isEnabled = true) override;

	// reimplemented (imtloggui::ILayerController)
	virtual QByteArray GetActiveRepresentationId() const override;
	virtual bool SetActiveRepresentationId(const QByteArray& representationId) override;

	// reimplemented (iprm::ISelectionParam)
	virtual const IOptionsList* GetSelectionConstraints() const override;
	virtual int GetSelectedOptionIndex() const override;
	virtual bool SetSelectedOptionIndex(int index) override;
	virtual ISelectionParam* GetSubselection(int index) const override;

	// reimplemented (iprm::IOptionsList)
	virtual int GetOptionsFlags() const override;
	virtual int GetOptionsCount() const override;
	virtual QString GetOptionName(int index) const override;
	virtual QString GetOptionDescription(int index) const override;
	virtual QByteArray GetOptionId(int index) const override;
	virtual bool IsOptionEnabled(int index) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_MULTIREF(istd::IChangeable, m_representationCompPtr);
	I_MULTIREF(iprm::IEnableableParam, m_representationControllerCompPtr);

	QByteArray m_activeRepresentationId;
	bool m_isEnabled;
};


} // namespace imtloggui


