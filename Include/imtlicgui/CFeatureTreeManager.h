#pragma once


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QMap>


class QTreeWidget;
class QTreeWidgetItem;


namespace imtlicgui
{


class CFeatureTreeManager
{
protected:
	CFeatureTreeManager();
	void UpdateFeatureTree();
	void UpdateFeatureTreeCheckStates();
	QTreeWidgetItem* GetItem(const QByteArray& itemId);

public:
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

	struct FeatureDescription
	{
		QByteArray id;
		QString name;
	};
	typedef QList<FeatureDescription> FeatureDescriptionList;

	struct PackageDescription
	{
		FeatureDescriptionList features;
		QString name;
	};
	typedef QMap<QByteArray, PackageDescription> PackageDescriptionMap;

	typedef QMap<QByteArray, QByteArrayList> FeatureDependencyMap;

protected:
	QTreeWidget* m_featureTreeWidget;
	bool m_isFeatureTreeUpdateEnabled;

	PackageDescriptionMap m_packages;
	QByteArrayList m_excludedFeatures;
	QByteArrayList m_selectedFeatures;
	QByteArrayList m_missingFeatures;
	FeatureDescriptionList m_unsavedFeatures;
};


} // namespace imtlicgui


