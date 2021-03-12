#include <imtlicgui/CFeatureTreeManager.h>


// Qt includes
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>


namespace imtlicgui
{


// public methods

CFeatureTreeManager::CFeatureTreeManager()
	:m_featureTreeWidget(nullptr),
	m_isFeatureTreeUpdateEnabled(false)
{
}


void CFeatureTreeManager::UpdateFeatureTree()
{
	if (m_featureTreeWidget != nullptr){
		QSignalBlocker blocker(m_featureTreeWidget);

		m_featureTreeWidget->clear();

		if (m_isFeatureTreeUpdateEnabled){
			QByteArrayList packageKeys = m_packages.keys();
			for (const QByteArray& packageKey : packageKeys){
				QTreeWidgetItem* packageItemPtr = new QTreeWidgetItem({m_packages[packageKey].name});
				m_featureTreeWidget->addTopLevelItem(packageItemPtr);
				packageItemPtr->setData(0, DR_ITEM_TYPE, IT_PACKAGE);
				packageItemPtr->setData(0, DR_ITEM_ID, packageKey);

				const FeatureDescriptionList& featureDescriptionList = m_packages[packageKey].features;
				for (const FeatureDescription& featureDescription : featureDescriptionList){
					if (m_excludedFeatures.contains(featureDescription.id)){
						continue;
					}

					QTreeWidgetItem* featureItemPtr = new QTreeWidgetItem({featureDescription.name});
					packageItemPtr->addChild(featureItemPtr);
					featureItemPtr->setData(0, DR_ITEM_ID, featureDescription.id);
					featureItemPtr->setData(0, DR_ITEM_TYPE, IT_FEATURE);
				}

				if (packageItemPtr->childCount() == 0){
					m_featureTreeWidget->takeTopLevelItem(m_featureTreeWidget->indexOfTopLevelItem(packageItemPtr));
					delete packageItemPtr;
				}
			}

			if (!m_unsavedFeatures.isEmpty()){
				QTreeWidgetItem* groupItemPtr = new QTreeWidgetItem({QObject::tr("Unsaved Features")});
				m_featureTreeWidget->addTopLevelItem(groupItemPtr);
				groupItemPtr->setData(0, DR_ITEM_TYPE, IT_PACKAGE);
				groupItemPtr->setData(0, DR_ITEM_ID, "UNSAVED_FEATURES");
				groupItemPtr->setForeground(0, QBrush(Qt::green));

				int unsavedFeatureCount = m_unsavedFeatures.count();
				for (int i = 0; i < unsavedFeatureCount; i++){
					QTreeWidgetItem* featureItemPtr = new QTreeWidgetItem({m_unsavedFeatures[i].name});
					groupItemPtr->addChild(featureItemPtr);
					featureItemPtr->setData(0, DR_ITEM_TYPE, IT_FEATURE);
					featureItemPtr->setData(0, DR_ITEM_ID, m_unsavedFeatures[i].id);
					featureItemPtr->setForeground(0, QBrush(Qt::green));
				}
			}

			if (!m_missingFeatures.isEmpty()){
				QTreeWidgetItem* groupItemPtr = new QTreeWidgetItem({QObject::tr("Missing features")});
				m_featureTreeWidget->addTopLevelItem(groupItemPtr);
				groupItemPtr->setData(0, DR_ITEM_TYPE, IT_PACKAGE);
				groupItemPtr->setData(0, DR_ITEM_ID, "MISSING_FEATURES");
				groupItemPtr->setForeground(0, QBrush(Qt::red));

				for (const QByteArray& featureId : m_missingFeatures){
					QTreeWidgetItem* featureItemPtr = new QTreeWidgetItem({QObject::tr("ID: %1").arg(QString(featureId))});
					groupItemPtr->addChild(featureItemPtr);
					featureItemPtr->setData(0, DR_ITEM_TYPE, IT_FEATURE);
					featureItemPtr->setData(0, DR_ITEM_ID, featureId);
					featureItemPtr->setForeground(0, QBrush(Qt::red));
				}
			}

			m_featureTreeWidget->expandAll();
		}
	}
}


void CFeatureTreeManager::UpdateFeatureTreeCheckStates()
{
	if (m_featureTreeWidget != nullptr){
		QSignalBlocker blocker(m_featureTreeWidget);

		int groupCount = m_featureTreeWidget->topLevelItemCount();

		for (int groupIndex = 0; groupIndex < groupCount; groupIndex++){
			QTreeWidgetItem* groupItemPtr = m_featureTreeWidget->topLevelItem(groupIndex);

			int featureCount = groupItemPtr->childCount();
			for (int featureIndex = 0; featureIndex < featureCount; featureIndex++){
				QTreeWidgetItem* featureItemPtr = groupItemPtr->child(featureIndex);

				QByteArray featureId = featureItemPtr->data(0, DR_ITEM_ID).toByteArray();
				bool isFeatureSelected = m_selectedFeatures.contains(featureId);
				featureItemPtr->setCheckState(0, isFeatureSelected ? Qt::Checked : Qt::Unchecked);
			}
		}

		for (int groupIndex = 0; groupIndex < groupCount; groupIndex++){
			QTreeWidgetItem* groupItemPtr = m_featureTreeWidget->topLevelItem(groupIndex);

			bool isAllFeaturesChecked = true;
			bool isAllFeaturesUnchecked = true;

			int featureCount = groupItemPtr->childCount();
			for (int featureIndex = 0; featureIndex < featureCount; featureIndex++){
				QTreeWidgetItem* featureItemPtr = groupItemPtr->child(featureIndex);

				if (featureItemPtr->checkState(0) == Qt::Checked){
					isAllFeaturesUnchecked = false;
				}
				else if (featureItemPtr->checkState(0) == Qt::Unchecked){
					isAllFeaturesChecked = false;
				}
			}

			if (isAllFeaturesChecked){
				groupItemPtr->setCheckState(0, Qt::Checked);
			}
			else if (isAllFeaturesUnchecked){
				groupItemPtr->setCheckState(0, Qt::Unchecked);
			}
			else{
				groupItemPtr->setCheckState(0, Qt::PartiallyChecked);
			}
		}

		if (!m_missingFeatures.isEmpty()){
			QTreeWidgetItem* groupItemPtr = GetItem("MISSING_FEATURES");
			Q_ASSERT(groupItemPtr != nullptr);

			if (groupItemPtr != nullptr){
				groupItemPtr->setCheckState(0, Qt::Checked);

				int featureCount = groupItemPtr->childCount();
				for (int featureIndex = 0; featureIndex < featureCount; featureIndex++){
					QTreeWidgetItem* featureItemPtr = groupItemPtr->child(featureIndex);
					featureItemPtr->setCheckState(0, Qt::Checked);
				}
			}
		}
	}
}


QTreeWidgetItem* CFeatureTreeManager::GetItem(const QByteArray& itemId)
{
	if (m_featureTreeWidget != nullptr){
		int groupCount = m_featureTreeWidget->topLevelItemCount();
		for (int groupIndex = 0; groupIndex < groupCount; groupIndex++){
			QTreeWidgetItem* groupItemPtr = m_featureTreeWidget->topLevelItem(groupIndex);
			if (groupItemPtr->data(0, DR_ITEM_ID).toByteArray() == itemId){
				return groupItemPtr;
			}

			int featureCount = groupItemPtr->childCount();
			for (int featureIndex = 0; featureIndex < featureCount; featureIndex++){
				QTreeWidgetItem* featurteItemPtr = groupItemPtr->child(featureIndex);
				if (featurteItemPtr->data(0, DR_ITEM_ID).toByteArray() == itemId){
					return featurteItemPtr;
				}
			}
		}
	}

	return nullptr;
}


} // namespace imtlicgui


