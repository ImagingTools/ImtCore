#pragma once


// ACF includes
#include <istd/TPointerVector.h>
#include <iprm/IParamsSet.h>
#include <iprm/IEnableableParam.h>
#include <imtgui/ILayoutFittableModelEditor.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>

#include <GeneratedFiles/imtgui/ui_CMultiParamsSetGuiComp.h>


namespace imtgui
{


class CMultiParamsSetGuiComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CMultiParamsSetGuiComp, iprm::IParamsSet>
{
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CMultiParamsSetGuiComp, iprm::IParamsSet> BaseClass;

	I_BEGIN_COMPONENT(CMultiParamsSetGuiComp);
		I_ASSIGN_MULTI_0(m_idsAttrPtr, "Ids", "List of parameter IDs according to observed parameter set. \n Use '*' sign to  attach all model", true);
		I_ASSIGN_MULTI_0(m_objectIdsAttrPtr, "ObjectIds", "List of IDs of created elements", true);
		I_ASSIGN_MULTI_1(m_labelTextsAttrPtr, "Labels", "List of parameter's Descriptions \n Leave empty string to get description from model \n Use whitespace symbols to create label without text", true, "");
		I_ASSIGN_MULTI_0(m_uiActivatorsCompPtr, "UiActivators", "Optional activators for each ui (enable/disable)", false);
		I_ASSIGN_TO(m_uiActivatorsModelCompPtr, m_uiActivatorsCompPtr, false);
	I_END_COMPONENT;

	CMultiParamsSetGuiComp();

protected:
	virtual void UpdateUiState(const istd::IChangeable::ChangeSet& changeSet, const iprm::IEnableableParam* aUiActivatorPtr);

protected:
	virtual ILayoutFittableModelEditor* CreateEditorFromModel(const imod::IModel* modelPtr, const QString& objectName = QString()) const;

	// reimplemented (imod::IModelEditor)
	virtual void SetReadOnly(bool state) override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void OnGuiModelAttached() override;
	virtual bool OnModelDetached(imod::IModel* modelPtr) override;

private:
	I_MULTIATTR(QByteArray, m_idsAttrPtr);
	I_MULTIATTR(QByteArray, m_objectIdsAttrPtr);
	I_MULTITEXTATTR(m_labelTextsAttrPtr);

	I_MULTIREF(iprm::IEnableableParam, m_uiActivatorsCompPtr);
	I_MULTIREF(imod::IModel, m_uiActivatorsModelCompPtr);

	istd::TPointerVector<ILayoutFittableModelEditor> m_modelEditors;

	imtbase::TModelUpdateBinder<iprm::IEnableableParam, CMultiParamsSetGuiComp> m_uiActivatorsObserver;
};


} // namespace imtgui


