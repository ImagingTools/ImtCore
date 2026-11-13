import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtlicFeaturesSdl 1.0

Dialog {
	id: requirementsDialog

	width: Style.sizeHintXXL
	backgroundColor: Style.baseColor

	title: targetFeatureName !== ""
		? qsTr("Add requirements for %1").arg(targetFeatureName)
		: qsTr("Add requirements")

	property string targetFeaturePath: ""
	property string targetFeatureName: ""
	property string currentRootPath: ""
	property string currentRootName: ""
	property var alreadyRequiredPaths: []
	property var requirementEntries: []
	property var requirementGraph: []
	property var mandatoryLeafCounts: ({})

	property bool onlyCurrentRootFeature: true
	property var filteredEntries: []

	function updateFilteredEntries() {
		let source = requirementsDialog.requirementEntries || []
		if (!requirementsDialog.onlyCurrentRootFeature || requirementsDialog.currentRootPath === "") {
			requirementsDialog.filteredEntries = source
			return
		}
		let result = []
		for (let i = 0; i < source.length; ++i) {
			if (source[i].rootPath === requirementsDialog.currentRootPath)
				result.push(source[i])
		}
		requirementsDialog.filteredEntries = result
	}

	onRequirementEntriesChanged: updateFilteredEntries()
	onOnlyCurrentRootFeatureChanged: updateFilteredEntries()
	onCurrentRootPathChanged: updateFilteredEntries()

	property var checkedPaths: []
	property alias selectedPaths: requirementsDialog.checkedPaths

	Component.onCompleted: {
		addButton(Enums.ok, qsTr("Add"), false)
		addButton(Enums.cancel, qsTr("Cancel"), true)
		updateFilteredEntries()
	}

	onStarted: {
		requirementsDialog.setButtonEnabled(Enums.ok, requirementsDialog.checkedPaths.length > 0)
	}

	Keys.onPressed: {
		if (event.key === Qt.Key_Return) {
			if (buttonIsEnabled(Enums.ok))
				requirementsDialog.finished(Enums.ok)
		}
		else if (event.key === Qt.Key_Escape) {
			requirementsDialog.finished(Enums.cancel)
		}
	}

	function isAlreadyRequired(path) {
		return requirementsDialog.alreadyRequiredPaths.indexOf(path) >= 0
	}

	function isChecked(path) {
		return requirementsDialog.checkedPaths.indexOf(path) >= 0
	}

	function toggleEntry(path) {
		if (!requirementIsEnabled(path))
			return
		let paths = requirementsDialog.checkedPaths.slice()
		let index = paths.indexOf(path)
		if (index >= 0)
			paths.splice(index, 1)
		else
			paths.push(path)
		requirementsDialog.checkedPaths = paths
		requirementsDialog.setButtonEnabled(Enums.ok, paths.length > 0)
	}

	function isAncestorPath(featurePathValue) {
		let path = requirementsDialog.targetFeaturePath
		return path !== "" && featurePathValue !== "" && path.indexOf(featurePathValue + "/") === 0
	}

	function isDescendantPath(featurePathValue) {
		let path = requirementsDialog.targetFeaturePath
		return path !== "" && featurePathValue !== "" && featurePathValue.indexOf(path + "/") === 0
	}

	function requirementEntry(featurePathValue) {
		for (let i = 0; i < requirementsDialog.requirementGraph.length; ++i) {
			if (requirementsDialog.requirementGraph[i].featurePath === featurePathValue)
				return requirementsDialog.requirementGraph[i]
		}
		return null
	}

	function requirementReaches(fromPath, targetPath, visited) {
		if (fromPath === targetPath)
			return true
		if (visited[fromPath])
			return false
		visited[fromPath] = true
		let entry = requirementEntry(fromPath)
		if (!entry || entry.requirements === "")
			return false
		let paths = entry.requirements.split(';')
		for (let i = 0; i < paths.length; ++i) {
			if (requirementReaches(paths[i], targetPath, visited))
				return true
		}
		return false
	}

	function requirementIsEnabled(featurePathValue) {
		let path = requirementsDialog.targetFeaturePath
		if (path !== "" && featurePathValue === path)
			return false
		if (isAlreadyRequired(featurePathValue))
			return false
		if (path !== "" && isAncestorPath(featurePathValue))
			return false
		if (path !== "" && isDescendantPath(featurePathValue))
			return false
		if (path !== "" && requirementReaches(featurePathValue, path, {}))
			return false
		return true
	}

	function allAvailableChecked() {
		let availableCount = 0
		let entries = requirementsDialog.filteredEntries
		for (let i = 0; i < entries.length; ++i) {
			if (requirementIsEnabled(entries[i].featurePath))
				availableCount++
		}
		return availableCount > 0 && requirementsDialog.checkedPaths.length === availableCount
	}

	function toggleAll() {
		if (allAvailableChecked()) {
			requirementsDialog.checkedPaths = []
		} else {
			let paths = []
			let entries = requirementsDialog.filteredEntries
			for (let i = 0; i < entries.length; ++i) {
				let p = entries[i].featurePath
				if (requirementIsEnabled(p))
					paths.push(p)
			}
			requirementsDialog.checkedPaths = paths
		}
		requirementsDialog.setButtonEnabled(Enums.ok, requirementsDialog.checkedPaths.length > 0)
	}

	contentComp: Component {
		Item {
			id: dialogBody
			width: requirementsDialog.width
			height: Style.sizeHintL + filterBar.height + Style.marginS

			Item {
				id: filterBar
				anchors.top: parent.top
				anchors.topMargin: Style.marginM
				anchors.left: parent.left
				anchors.leftMargin: Style.marginL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginL
				height: Style.controlHeightS

				CheckBox {
					id: rootFilterCheck
					anchors.left: parent.left
					anchors.verticalCenter: parent.verticalCenter
					text: requirementsDialog.currentRootName !== ""
						? qsTr("Only features of root '%1'").arg(requirementsDialog.currentRootName)
						: qsTr("Only features of this Root feature")
					checkState: requirementsDialog.onlyCurrentRootFeature ? Qt.Checked : Qt.Unchecked
					onCheckStateChanged: {
						let isChecked = (checkState === Qt.Checked)
						if (requirementsDialog.onlyCurrentRootFeature !== isChecked)
							requirementsDialog.onlyCurrentRootFeature = isChecked
					}
				}
			}

			CheckableListPanel {
				objectName: "AvailableRequirementsPanel"
				anchors.top: filterBar.bottom
				anchors.topMargin: Style.marginS
				anchors.left: parent.left
				anchors.leftMargin: Style.marginL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginL
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginL
				title: qsTr("Available features")
				subtitle: requirementsDialog.checkedPaths.length > 0
					? qsTr("%1 selected").arg(requirementsDialog.checkedPaths.length) : ""
				model: requirementsDialog.filteredEntries
				nameColumnTitle: qsTr("Feature")
				iconSource: "Icons/Dependencies"
				searchPlaceholder: qsTr("Search features")
				emptyText: qsTr("No features to require")
				emptyDescription: requirementsDialog.onlyCurrentRootFeature
					? qsTr("No other available features in this root package. Turn off the filter above to see features from other packages.")
					: qsTr("Every feature of the collection is already required or unavailable")
				actionText: requirementsDialog.allAvailableChecked() ? qsTr("Clear") : qsTr("Select all")
				actionEnabled: requirementsDialog.filteredEntries.length > 0
				entryTitleProvider: function(entry) { return entry.featureName }
				entrySubtitleProvider: function(entry) { return entry.fullPath !== "" ? entry.fullPath : entry.featurePath }
				entryBadgeProvider: function(entry) {
					if (requirementsDialog.isAlreadyRequired(entry.featurePath))
						return qsTr("Already required")
					if (requirementsDialog.isDescendantPath(entry.featurePath))
						return qsTr("Sub-feature")
					if (requirementsDialog.isAncestorPath(entry.featurePath))
						return qsTr("Parent")
					if (requirementsDialog.targetFeaturePath !== "" && requirementsDialog.requirementReaches(entry.featurePath, requirementsDialog.targetFeaturePath, {}))
						return qsTr("Cyclic")
					return ""
				}
				entryBadgeToneProvider: function(entry) {
					if (requirementsDialog.targetFeaturePath !== "" && requirementsDialog.requirementReaches(entry.featurePath, requirementsDialog.targetFeaturePath, {}))
						return "warning"
					return "neutral"
				}
				entrySearchableTextProvider: function(entry) { return entry.fullPath + " " + entry.featurePath }
				entryCheckStateProvider: function(entry) { return requirementsDialog.isChecked(entry.featurePath) ? Qt.Checked : Qt.Unchecked }
				entryCheckEnabledProvider: function(entry) { return requirementsDialog.requirementIsEnabled(entry.featurePath) }
				onToggleRequested: requirementsDialog.toggleEntry(entry.featurePath)
				onActionRequested: requirementsDialog.toggleAll()
			}
		}
	}
}

