#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/CHierarchicalCommand.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/IFeatureInfoProvider.h>
#include <GeneratedFiles/imtlicgui/ui_CFeaturePackageGuiComp.h>


namespace imtlicgui
{


/**
	Editor for the product-related licensing.
	\ingroup LicenseManagement
*/
class CFeaturePackageGuiComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CFeaturePackageGuiComp, imtlic::IFeatureInfoProvider>,
			virtual public ibase::ICommandsProvider
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CFeaturePackageGuiComp, imtlic::IFeatureInfoProvider> BaseClass;

	I_BEGIN_COMPONENT(CFeaturePackageGuiComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_ASSIGN(m_objectCollectionViewCompPtr, "ObjectCollectionView", "Object collection view", true, "ObjectCollectionView");
		I_ASSIGN_TO(m_objectCollectionObserverCompPtr, m_objectCollectionViewCompPtr, true);
	I_END_COMPONENT;

	CFeaturePackageGuiComp();

Q_SIGNALS:
	void EmitFeatureTreeItemChanged();

protected:
	typedef QMap<QByteArray, QByteArrayList> DependencyMap;

	void OnFeaturePackageCollectionUpdate();
	void EnumerateMissingDependencies();
	void UpdateFeatureList();
	void UpdateFeatureTree();
	void UpdateFeatureTreeCheckStates();
	QTreeWidgetItem* GetItem(const QByteArray& itemId);
	DependencyMap BuildDependencyMap(const imtbase::IObjectCollection& packageCollection);
	bool HasDependency(const DependencyMap& dependencyMap, const QByteArray& fromFeatureId, const QByteArray& toFeatureId);

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated();
	virtual void OnGuiDestroyed();
	virtual void OnGuiRetranslate() override;

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const;

private Q_SLOTS:
	void on_FeatureTree_itemChanged(QTreeWidgetItem *item, int column);
	virtual void OnShowCollectionEditor();
	virtual void OnShowFeatureDependencyEditor();
	virtual void OnFeatureListSelectionChanged();
	virtual void OnFeatureTreeItemChanged();

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
		FeaturePackageCollectionObserver(CFeaturePackageGuiComp& parent);

		// reimplemented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;
	private:
		CFeaturePackageGuiComp& m_parent;
	};

	struct FeatureDescription
	{
		QByteArray id;
		QString name;
		QString description;
	};

	typedef QList<FeatureDescription> FeatureDescriptionList;

protected:
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_showCollectionEditorCommand;
	iqtgui::CHierarchicalCommand m_showDependenciesEditorCommand;

	bool m_isGuiModelInitialized;
	bool m_isCollectionRepresentationInitialized;

	QByteArrayList m_dependencies;

	// Feature package collection related members
	QMap<QByteArray, FeatureDescriptionList> m_packageFeatures;
	QMap<QByteArray, QString> m_packageNames;
	QByteArrayList m_missingDependencies;

	FeaturePackageCollectionObserver m_collectionObserver;

private:
	I_REF(iqtgui::IGuiObject, m_objectCollectionViewCompPtr);
	I_REF(imod::IObserver, m_objectCollectionObserverCompPtr);
};


} // namespace imtlicgui


