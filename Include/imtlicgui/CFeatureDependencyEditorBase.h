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

	virtual void FeatureTreeItemChanged() = 0;

protected:
	typedef QMap<QByteArray, QByteArrayList> DependencyMap;

	void OnFeaturePackageCollectionUpdate();
	void EnumerateMissingDependencies();
	void UpdateFeatureTree();
	void UpdateFeatureTreeCheckStates();
	QTreeWidgetItem* GetItem(const QByteArray& itemId);
	DependencyMap BuildDependencyMap(const imtbase::IObjectCollection& packageCollection);
	bool HasDependency(const DependencyMap& dependencyMap, const QByteArray& fromFeatureId, const QByteArray& toFeatureId);

	virtual void UpdateFeaturePackageModel() = 0;

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

	QByteArray m_featureId;
	QByteArrayList m_dependencies;
	FeaturePackageCollectionObserver m_packageCollectionObserver;

	// Feature package collection related members
	QMap<QByteArray, FeatureDescriptionList> m_packageFeatures;
	QMap<QByteArray, QString> m_packageNames;
	QByteArrayList m_missingDependencies;
};


} // namespace imtlicgui


