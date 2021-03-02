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

	class FeaturePackageCollectionObserver: public imod::TSingleModelObserverBase<imtbase::IObjectCollection>
	{
	public:
		FeaturePackageCollectionObserver(CLicenseInfoEditorGuiComp& parent);

		// reimplemented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;
	private:
		CLicenseInfoEditorGuiComp& m_parent;
	};

	class ItemChangedSignalBlocker
	{
	public:
		ItemChangedSignalBlocker(int* counter)
		{
			m_counter = counter;

			if (m_counter != nullptr){
				(*m_counter)++;
			}
		}

		~ItemChangedSignalBlocker()
		{
			if (m_counter != nullptr){
				(*m_counter)--;
			}
		}

	private:
		int* m_counter;
	};

protected:
	void OnFeaturePackageCollectionUpdate();
	void UpdateTreeItemCheckStates();
	void RemoveMissingFeatures();
	QTreeWidgetItem* GetItem(const QByteArray& itemId);

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const;

private Q_SLOTS:
	void on_NameEdit_editingFinished();
	void on_IdEdit_editingFinished();
	void on_Features_itemChanged(QTreeWidgetItem *item, int column);

protected:
	bool m_isGuiModelInitialized;
	bool m_isCollectionRepresentationInitialized;
	int m_itemChangedSignalBlockCounter;

	// License related members
	QByteArrayList m_featureIds;

	// Feature package collection related members
	QMap<QByteArray, QByteArrayList> m_collectionRepresentation;
	FeaturePackageCollectionObserver m_collectionObserver;
};


} // namespace imtlicgui


