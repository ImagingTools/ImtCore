#pragma once


// Qt includes
#include <QtWidgets/QTreeWidget>

// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/IFeatureInfoProvider.h>


namespace imtlicgui
{


/**
	Editor for the product-related licensing.
	\ingroup LicenseManagement
*/
class CFeatureDependencyEditorBase: virtual public istd::IPolymorphic
{
public:
	CFeatureDependencyEditorBase();

protected:
	typedef QMap<QByteArray, QByteArrayList> DependencyMap;

	void OnFeaturePackageCollectionUpdate();
	void EnumerateMissingDependencies();
	void UpdateFeatureTree();
	void UpdateFeatureTreeCheckStates();
	QTreeWidgetItem* GetItem(const QByteArray& itemId);
	void BuildDependencyMap(const imtbase::IObjectCollection& packageCollection);
	bool HasDependency(const DependencyMap& dependencyMap, const QByteArray& fromFeatureId, const QByteArray& toFeatureId);

	virtual void UpdateFeaturePackageModel() = 0;
	virtual void FeatureTreeItemChanged() = 0;

	void on_FeatureTree_itemChanged(QTreeWidgetItem *item, int column);

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
		FeaturePackageCollectionObserver();

		void SetParent(CFeatureDependencyEditorBase* m_parentPtr);

		// reimplemented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;
	private:
		CFeatureDependencyEditorBase* m_parentPtr;
	};

	struct FeatureDescription
	{
		QByteArray id;
		QString name;
		QString description;
	};

	typedef QList<FeatureDescription> FeatureDescriptionList;

protected:
	QTreeWidget* m_treeWidgetPtr;

	bool m_isGuiModelInitialized;
	bool m_isCollectionRepresentationInitialized;

	FeaturePackageCollectionObserver m_packageCollectionObserver;

	// Selected package related members
	FeatureDescriptionList m_features;
	DependencyMap m_dependencies;
	QByteArrayList m_missingDependencies;

	// Selected feature related members
	QByteArray m_selectedFeatureId;

	// Feature package collection related members
	QMap<QByteArray, QString> m_packageNameMap;
	QMap<QByteArray, FeatureDescriptionList> m_packageFeatureMap;
	DependencyMap m_packageDependenciyMap;
};


} // namespace imtlicgui


