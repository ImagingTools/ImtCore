#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/IFeatureInfoProvider.h>
#include <imtlic/ILicenseInfo.h>
#include <GeneratedFiles/imtlicgui/ui_CLicenseInfoEditorGuiComp.h>


namespace imtlicgui
{


/**
	Editor for the license info.
	\ingroup LicenseManagement
*/
class CLicenseInfoEditorGuiComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CLicenseInfoEditorGuiComp, imtlic::ILicenseInfo>
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CLicenseInfoEditorGuiComp, imtlic::ILicenseInfo> BaseClass;

	I_BEGIN_COMPONENT(CLicenseInfoEditorGuiComp);
	I_END_COMPONENT;

	CLicenseInfoEditorGuiComp();

protected:
	enum ItemData
	{
		ID_ITEM_ID = Qt::UserRole,
		ID_ITEM_TYPE
	};

	enum ItemType
	{
		IT_PACKAGE = 0,
		IT_FEATURE
	};

	class FeaturePackageCollectionObserver: public imod::TSingleModelObserverBase<imtbase::IObjectCollection>
	{
	public:
		FeaturePackageCollectionObserver(CLicenseInfoEditorGuiComp& parent);

		// reimplemented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;
	private:
		CLicenseInfoEditorGuiComp& m_parent;
	};

protected:
	void OnFeaturePackageCollectionUpdate();
	void SynchronizeFeatureItems();
	QTreeWidgetItem* FindChildItem(const QByteArray& featureId);

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const;

protected:
	FeaturePackageCollectionObserver m_collectionObserver;
	QByteArrayList m_featureIds;

private Q_SLOTS:
	void on_NameEdit_editingFinished();
	void on_IdEdit_editingFinished();
	void on_Features_itemChanged(QTreeWidgetItem *item, int column);
};


} // namespace imtlicgui


