#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/IMultiSelection.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtlicgui/IFeatureItemStateHandler.h>
#include <GeneratedFiles/imtlicgui/ui_CFeatureTreeGuiComp.h>


namespace imtlicgui
{


class CFeatureTreeGuiComp: public iqtgui::TDesignerGuiObserverCompBase<
			Ui::CFeatureTreeGuiComp, imtbase::IObjectCollection>
{
	Q_OBJECT
public:
	typedef public iqtgui::TDesignerGuiObserverCompBase<
				Ui::CFeatureTreeGuiComp, imtbase::IObjectCollection> BaseClass;

	I_BEGIN_COMPONENT(CFeatureTreeGuiComp);
		I_ASSIGN(m_showFeatureStatesAttrPtr, "ShowFeatureStates", "Show feature selection states", true, true);
		I_ASSIGN(m_featureItemStateHandlerCompPtr, "FeatureItemStateHandler", "Feature item state handler", false, "");
		I_ASSIGN(m_featureSelectionCompPtr, "FeatureSelectionModel", "Feature selection model", true, "FeatureSelectionModel");
		I_ASSIGN_TO(m_featureSelectionModelCompPtr, m_featureSelectionCompPtr, true);
		I_ASSIGN(m_featureStateCompPtr, "FeatureStateModel", "Feature state model", true, "FeatureStateModel");
		I_ASSIGN_TO(m_featureStateModelCompPtr, m_featureStateCompPtr, true);
	I_END_COMPONENT;

	CFeatureTreeGuiComp();

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated();
	virtual void OnGuiDestroyed();

private:
	QTreeWidgetItem* FindItem(const QByteArray& itemId);

	void OnFeatureSelectionChanged(
				const istd::IChangeable::ChangeSet& /*changeSet*/,
				const imtbase::IMultiSelection* selectionPtr);
	void OnFeatureStateChanged(
				const istd::IChangeable::ChangeSet& /*changeSet*/,
				const imtbase::IMultiSelection* statePtr);
	void UpdateFeatureSelections();
	void UpdateFeatureStates();

private Q_SLOTS:
	void on_Features_itemChanged(QTreeWidgetItem *item, int column);

private:
	enum DataRole
	{
		DR_ITEM_ID = Qt::UserRole,
		DR_ITEM_TYPE
	};

	enum ItemType
	{
		IT_PACKAGE = 0,
		IT_FEATURE
	};

private:
	I_ATTR(bool, m_showFeatureStatesAttrPtr);
	I_REF(imtlicgui::IFeatureItemStateHandler, m_featureItemStateHandlerCompPtr);
	I_REF(imtbase::IMultiSelection, m_featureSelectionCompPtr);
	I_REF(imod::IModel, m_featureSelectionModelCompPtr);
	I_REF(imtbase::IMultiSelection, m_featureStateCompPtr);
	I_REF(imod::IModel, m_featureStateModelCompPtr);

	imtbase::TModelUpdateBinder<imtbase::IMultiSelection, CFeatureTreeGuiComp> m_featureSelectionObserver;
	imtbase::TModelUpdateBinder<imtbase::IMultiSelection, CFeatureTreeGuiComp> m_featureStateObserver;

	QByteArrayList m_selectedFeatures;
	QByteArrayList m_disabledFeatures;
};


} // namespace imtlicgui


