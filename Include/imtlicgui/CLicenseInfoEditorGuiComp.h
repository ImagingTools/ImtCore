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

Q_SIGNALS:
	void EmitItemChanged();

protected:
	typedef QMap<QByteArray, QByteArrayList> DependencyMap;

	void OnFeaturePackageCollectionUpdate();
	void EnumerateMissingFeatures();
	void UpdateFeatureTree();
	void UpdateFeatureTreeCheckStates();
	QTreeWidgetItem* GetItem(const QByteArray& itemId);
	DependencyMap BuildDependencyMap(const imtbase::IObjectCollection& packageCollection);
	void ActivateDependencies(const QByteArrayList& featureIds);


	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;

private Q_SLOTS:
	void on_NameEdit_editingFinished();
	void on_IdEdit_editingFinished();
	void on_Features_itemChanged(QTreeWidgetItem *item, int column);
	void OnItemChanged();

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

	struct FeatureDescription
	{
		QByteArray id;
		QString name;
		QString description;
	};

	typedef QList<FeatureDescription> FeatureDescriptionList;

protected:
	bool m_isGuiModelInitialized;
	bool m_isCollectionRepresentationInitialized;

	// License related members
	QByteArrayList m_featureIds;

	// Feature package collection related members
	QMap<QByteArray, FeatureDescriptionList> m_packageFeatures;
	QMap<QByteArray, QString> m_packageNames;
	QByteArrayList m_missingFeatures;

	FeaturePackageCollectionObserver m_collectionObserver;
};


} // namespace imtlicgui


