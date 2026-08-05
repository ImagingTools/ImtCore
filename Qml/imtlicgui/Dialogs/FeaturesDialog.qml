import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtlicFeaturesSdl 1.0

// Feature picker. Its body is the same CheckableListPanel that carries the
// Dependencies and Feature-content lists, so picking a feature here looks and
// behaves like ticking one there: search on top, name with its identifier as a
// chip, and a Select all / Clear command in the header.
Dialog {
	id: featuresDialog

	width: Style.sizeHintXXL
	backgroundColor: Style.baseColor

	title: qsTr("Select features")

	property var selectedIndexes: []
	property TreeItemModel featuresModel: TreeItemModel {}
	property TreeItemModel tableModel: TreeItemModel {}
	property var excludeFeatureIds: []
	// Rows of the picker: one per feature that may still be added.
	property var entries: []
	property var checkedIds: []

	Component.onCompleted: {
		// Add stays off until at least one row is ticked.
		addButton(Enums.ok, qsTr("Add"), false)
		addButton(Enums.cancel, qsTr("Cancel"), true)
		rebuildEntries()
	}

	onFeaturesModelChanged: rebuildEntries()

	onFinished: {
		if (buttonId !== Enums.ok)
			return
		let indexes = []
		for (let i = 0; i < featuresDialog.entries.length; ++i) {
			if (featuresDialog.checkedIds.indexOf(featuresDialog.entries[i].id) >= 0)
				indexes.push(featuresDialog.entries[i].index)
		}
		featuresDialog.selectedIndexes = indexes
	}

	Keys.onPressed: {
		if (event.key === Qt.Key_Return) {
			if (buttonIsEnabled(Enums.ok))
				featuresDialog.finished(Enums.ok)
		}
		else if (event.key === Qt.Key_Escape) {
			featuresDialog.finished(Enums.cancel)
		}
	}

	// tableModel stays the source of the indexes the caller reads back, so each
	// row carries the index it was copied to rather than a position in a view.
	function rebuildEntries() {
		featuresDialog.tableModel.clear()
		let rows = []
		for (let i = 0; i < featuresDialog.featuresModel.getItemsCount(); ++i) {
			let featureId = featuresDialog.featuresModel.getData(FeatureItemTypeMetaInfo.s_id, i)
			if (featuresDialog.excludeFeatureIds.indexOf(featureId) >= 0)
				continue
			let index = featuresDialog.tableModel.insertNewItem()
			featuresDialog.tableModel.copyItemDataFromModel(index, featuresDialog.featuresModel, i)
			rows.push({
				"id": featureId,
				"index": index,
				"featureName": featuresDialog.featuresModel.getData(FeatureItemTypeMetaInfo.s_featureName, i) || "",
				"featureId": featuresDialog.featuresModel.getData(FeatureItemTypeMetaInfo.s_featureId, i) || "",
				"description": featuresDialog.featuresModel.getData(FeatureItemTypeMetaInfo.s_description, i) || ""
			})
		}
		featuresDialog.entries = rows
		featuresDialog.checkedIds = []
		featuresDialog.setButtonEnabled(Enums.ok, false)
	}

	function isChecked(entryId) {
		return featuresDialog.checkedIds.indexOf(entryId) >= 0
	}

	function toggleEntry(entryId) {
		let ids = featuresDialog.checkedIds.slice()
		let index = ids.indexOf(entryId)
		if (index >= 0)
			ids.splice(index, 1)
		else
			ids.push(entryId)
		featuresDialog.checkedIds = ids
		featuresDialog.setButtonEnabled(Enums.ok, ids.length > 0)
	}

	function allChecked() {
		return featuresDialog.entries.length > 0
			&& featuresDialog.checkedIds.length === featuresDialog.entries.length
	}

	function toggleAll() {
		let ids = []
		if (!featuresDialog.allChecked()) {
			for (let i = 0; i < featuresDialog.entries.length; ++i)
				ids.push(featuresDialog.entries[i].id)
		}
		featuresDialog.checkedIds = ids
		featuresDialog.setButtonEnabled(Enums.ok, ids.length > 0)
	}

	contentComp: Component {
		Item {
			id: dialogBody
			width: featuresDialog.width
			height: Style.sizeHintL

			CheckableListPanel {
				anchors.fill: parent
				anchors.margins: Style.marginL
				title: qsTr("Available features")
				subtitle: featuresDialog.checkedIds.length > 0
					? qsTr("%1 selected").arg(featuresDialog.checkedIds.length) : ""
				model: featuresDialog.entries
				nameColumnTitle: qsTr("Feature")
				iconSource: "Icons/FeaturePackage"
				searchPlaceholder: qsTr("Search features")
				emptyText: qsTr("Nothing left to add")
				emptyDescription: qsTr("Every feature of the collection is already part of this product")
				actionText: featuresDialog.allChecked() ? qsTr("Clear") : qsTr("Select all")
				actionEnabled: featuresDialog.entries.length > 0
				entryTitleProvider: function(entry) { return entry.featureName }
				entrySubtitleProvider: function(entry) { return entry.description }
				entryBadgeProvider: function(entry) { return entry.featureId }
				entrySearchableTextProvider: function(entry) { return entry.featureName + " " + entry.featureId }
				entryCheckStateProvider: function(entry) { return featuresDialog.isChecked(entry.id) ? Qt.Checked : Qt.Unchecked }
				entryCheckEnabledProvider: function(entry) { return true }
				onToggleRequested: featuresDialog.toggleEntry(entry.id)
				onActionRequested: featuresDialog.toggleAll()
			}
		}
	}
}
