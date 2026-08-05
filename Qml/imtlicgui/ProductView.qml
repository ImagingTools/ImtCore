import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import imtlicProductsSdl 1.0
import imtlicFeaturesSdl 1.0

ViewBase {
	id: productViewContainer
	contentColor: Style.baseColor

	property TreeItemModel allFeaturesModel: CachedFeatureCollection.collectionModel
	property TreeItemModel licensesModel: TreeItemModel {}
	property TreeItemModel productFeaturesViewModel: TreeItemModel {}
	property ProductData productData: model ? model : null
	property var featureTree: []
	property var selectedNode: null
	// Flattened content of the selected feature, shown in the right-hand panel.
	property var subFeatureEntries: []
	// m_features, split once and kept here: reassigning it is what re-evaluates
	// every "is this included" binding in the table and the panel.
	property var featureIds: []
	// The optional parts taken, as { featureId, subFeatureIds } - the shape the
	// product stores. A sub-feature id is only unique inside its own feature, so
	// it travels next to that feature instead of being glued onto it.
	property var optionalFeatures: []
	property bool canEdit: false

	Component.onCompleted: {
		canEdit = PermissionsController.checkPermission("ChangeProduct")
		CachedFeatureCollection.updateModel()
		CachedFeatureCollection.modelUpdated.connect(productViewContainer.onFeaturesChanged)
		multiPageView.updatePages()
		multiPageView.ensurePageLoaded(0)
		multiPageView.ensurePageLoaded(1)
	}

	Component.onDestruction: {
		CachedFeatureCollection.modelUpdated.disconnect(productViewContainer.onFeaturesChanged)
	}

	LocalizationEvent {
		onLocalizationChanged: productViewContainer.updateFeaturesGui()
	}

	function onFeaturesChanged() {
		updateFeaturesGui()
	}

	function updateModel() {
		if (!productData)
			return
		var generalPageInstance = multiPageView.getPageByIndex(0)
		if (generalPageInstance)
			generalPageInstance.updateModel()
	}

	function updateGui() {
		if (!productData)
			return
		var generalPageInstance = multiPageView.getPageByIndex(0)
		if (generalPageInstance)
			generalPageInstance.updateGui()
		updateFeaturesGui()
	}

	function selectedFeatureIds() {
		return productViewContainer.featureIds
	}

	function refreshFeatureIds() {
		if (!productData || !productData.m_features)
			featureIds = []
		else
			featureIds = productData.m_features.split(';')
		optionalFeatures = readOptionalFeatures()
	}

	function applyFeatureIds(ids) {
		featureIds = ids
		if (productData)
			productData.m_features = ids.join(';')
	}

	// The model side is a list of elements; here it is a plain array so the
	// bindings can read it without walking a model on every evaluation.
	function readOptionalFeatures() {
		let result = []
		if (!productData || !productData.m_optionalFeatures)
			return result
		let elements = productData.m_optionalFeatures
		for (let i = 0; i < elements.getItemsCount(); ++i) {
			let item = elements.get(i).item
			if (!item || !item.m_featureId)
				continue
			result.push({
				"featureId": item.m_featureId,
				"subFeatureIds": item.m_subFeatureIds ? item.m_subFeatureIds : []
			})
		}
		return result
	}

	// A field of its own on the product, because m_features is a list of feature
	// document ids and the server resolves every entry of it as one. A sub-feature
	// is a node inside such a document and has no document of its own, so it is
	// recorded here, beside the feature it belongs to.
	function applyOptionalFeatures(entries) {
		optionalFeatures = entries
		if (!productData)
			return
		// A product that never took an optional part carries no list at all, so
		// the list has to be created before the first entry can go into it.
		if (!productData.hasOptionalFeatures())
			productData.emplaceOptionalFeatures()
		let elements = productData.m_optionalFeatures
		if (!elements)
			return
		elements.clear()
		for (let i = 0; i < entries.length; ++i) {
			let element = productData.createOptionalFeaturesArrayElement()
			element.m_featureId = entries[i].featureId
			element.m_subFeatureIds = entries[i].subFeatureIds
			element.owner = productData
			element.connectProperties()
			elements.append({ "item": element })
		}
		// clear() and append() are silent, so the whole rewrite is announced once
		// here: without it an emptied list would never reach the document.
		productData.modelChanged([])
	}

	function updateFeaturesGui() {
		productFeaturesViewModel.clear()
		refreshFeatureIds()
		selectRootFeature(null)
		if (!productData || !productData.m_features) {
			featureTree = []
			updatePageBadges()
			return
		}

		let ids = selectedFeatureIds()
		for (let featureId of ids) {
			if (featureId.indexOf('/') >= 0)
				continue
			for (let i = 0; i < allFeaturesModel.getItemsCount(); ++i) {
				let id = allFeaturesModel.getData("id", i)
				if (featureId === id) {
					let index = productFeaturesViewModel.insertNewItem()
					productFeaturesViewModel.copyItemDataFromModel(index, allFeaturesModel, i)
					break
				}
			}
		}

		let tree = TreeModelBuilder.fromTreeItemModelByFields(productFeaturesViewModel, {
			key: FeatureItemTypeMetaInfo.s_id,
			children: FeatureItemTypeMetaInfo.s_subFeatures,
			columns: {
				id: FeatureItemTypeMetaInfo.s_id,
				featureName: FeatureItemTypeMetaInfo.s_featureName,
				featureId: FeatureItemTypeMetaInfo.s_featureId,
				description: FeatureItemTypeMetaInfo.s_description,
				optional: FeatureItemTypeMetaInfo.s_optional,
				isPermission: FeatureItemTypeMetaInfo.s_isPermission
			}
		})
		for (let i = 0; i < tree.length; ++i)
			decorateNode(tree[i], tree[i].data ? tree[i].data.id || "" : "")
		featureTree = tree
		updatePageBadges()
	}

	// Count on the Features tab of the page panel, so the size of the product
	// is visible without opening it.
	function updatePageBadges() {
		multiPageView.setPageBadge("Features", featureTree.length > 0 ? "" + featureTree.length : "")
	}

	function decorateNode(node, rootId) {
		node.rootId = rootId
		if (!node.children)
			return
		for (let i = 0; i < node.children.length; ++i)
			decorateNode(node.children[i], rootId)
	}

	function addFeature(featureId) {
		let ids = selectedFeatureIds().slice()
		if (ids.indexOf(featureId) < 0)
			ids.push(featureId)
		applyFeatureIds(ids)
	}

	function removeFeature(featureId) {
		let ids = selectedFeatureIds().slice()
		let index = ids.indexOf(featureId)
		if (index >= 0)
			ids.splice(index, 1)
		applyFeatureIds(ids)
	}

	function selectedRootFeatureId() {
		if (!selectedNode || !selectedNode.data)
			return ""
		return selectedNode.data.id || ""
	}

	function selectedFeatureName() {
		if (!selectedNode || !selectedNode.data)
			return ""
		return selectedNode.data.featureName || ""
	}

	function selectRootFeature(node) {
		selectedNode = node
		rebuildSubFeatureEntries()
	}

	function removeSelectedRootFeature() {
		removeRootFeatures(selectedNode ? [selectedNode] : [])
	}

	// Drops the features and everything picked underneath them in one pass.
	function removeRootFeatures(nodes) {
		if (!canEdit || !nodes || nodes.length === 0)
			return
		let doomed = []
		for (let i = 0; i < nodes.length; ++i) {
			let data = nodes[i] && nodes[i].data ? nodes[i].data : null
			if (data && data.id)
				doomed.push(data.id)
		}
		if (doomed.length === 0)
			return
		let ids = selectedFeatureIds()
		let remainingIds = []
		for (let j = 0; j < ids.length; ++j) {
			let keep = true
			for (let k = 0; k < doomed.length; ++k) {
				if (ids[j] === doomed[k] || ids[j].indexOf(doomed[k] + "/") === 0)
					keep = false
			}
			if (keep)
				remainingIds.push(ids[j])
		}
		let remainingOptional = []
		for (let m = 0; m < optionalFeatures.length; ++m) {
			let keepOptional = true
			for (let n = 0; n < doomed.length; ++n) {
				if (optionalFeatures[m].featureId === doomed[n])
					keepOptional = false
			}
			if (keepOptional)
				remainingOptional.push(optionalFeatures[m])
		}
		applyOptionalFeatures(remainingOptional)
		applyFeatureIds(remainingIds)
		updateFeaturesGui()
	}

	// Only the leaves of a product feature. A node that has children of its own
	// is a grouping - it grants whatever they grant - so listing it next to them
	// would count the same capability twice.
	function appendSubFeatureEntries(nodes, pathNames, rootId, target) {
		for (let i = 0; i < nodes.length; ++i) {
			let node = nodes[i]
			let data = node.data || {}
			let name = data.featureName || data.featureId || ""
			let currentPath = pathNames.concat([name])
			if (node.children && node.children.length > 0) {
				appendSubFeatureEntries(node.children, currentPath, rootId, target)
				continue
			}
			target.push({
				"featureName": name,
				"fullPath": currentPath.join(" / "),
				"featureId": data.featureId || "",
				"optional": data.optional === true,
				"rootId": rootId
			})
		}
	}

	function subFeatureEntriesOf(node) {
		let entries = []
		if (node)
			appendSubFeatureEntries(node.children || [], [], node.rootId || "", entries)
		return entries
	}

	function subFeatureCount(node) {
		return node && node.children ? node.children.length : 0
	}

	function rebuildSubFeatureEntries() {
		subFeatureEntries = subFeatureEntriesOf(selectedNode)
	}

	// The parts taken from one feature, by that feature's id.
	function takenSubFeatureIds(featureId) {
		for (let i = 0; i < optionalFeatures.length; ++i) {
			if (optionalFeatures[i].featureId === featureId)
				return optionalFeatures[i].subFeatureIds
		}
		return []
	}

	function entryIsIncluded(entry) {
		if (!entry)
			return false
		// Mandatory parts always come with the feature; optional ones are taken
		// one by one.
		if (!entry.optional)
			return true
		return takenSubFeatureIds(entry.rootId).indexOf(entry.featureId) >= 0
	}

	// Rebuilds the whole record with one sub-feature added or removed. Entries
	// that end up with nothing taken are dropped rather than left empty.
	function setSubFeatureTaken(featureId, subFeatureId, taken) {
		let result = []
		let handled = false
		for (let i = 0; i < optionalFeatures.length; ++i) {
			let current = optionalFeatures[i]
			if (current.featureId !== featureId) {
				result.push(current)
				continue
			}
			handled = true
			let ids = current.subFeatureIds.slice()
			let index = ids.indexOf(subFeatureId)
			if (taken && index < 0)
				ids.push(subFeatureId)
			else if (!taken && index >= 0)
				ids.splice(index, 1)
			if (ids.length > 0)
				result.push({ "featureId": featureId, "subFeatureIds": ids })
		}
		if (!handled && taken)
			result.push({ "featureId": featureId, "subFeatureIds": [subFeatureId] })
		applyOptionalFeatures(result)
	}

	function toggleSubFeature(entry) {
		if (!canEdit || !entry || !entry.optional)
			return
		setSubFeatureTaken(entry.rootId, entry.featureId, !entryIsIncluded(entry))
	}

	function optionalEntries() {
		let result = []
		for (let i = 0; i < subFeatureEntries.length; ++i) {
			if (subFeatureEntries[i].optional)
				result.push(subFeatureEntries[i])
		}
		return result
	}

	function allOptionalIncluded() {
		let entries = optionalEntries()
		if (entries.length === 0)
			return false
		for (let i = 0; i < entries.length; ++i) {
			if (!entryIsIncluded(entries[i]))
				return false
		}
		return true
	}

	function toggleAllOptional() {
		if (!canEdit || !selectedNode)
			return
		let entries = optionalEntries()
		let takeAll = !allOptionalIncluded()
		let featureId = selectedRootFeatureId()
		let ids = []
		if (takeAll) {
			for (let i = 0; i < entries.length; ++i)
				ids.push(entries[i].featureId)
		}
		let result = []
		for (let j = 0; j < optionalFeatures.length; ++j) {
			if (optionalFeatures[j].featureId !== featureId)
				result.push(optionalFeatures[j])
		}
		if (ids.length > 0)
			result.push({ "featureId": featureId, "subFeatureIds": ids })
		applyOptionalFeatures(result)
	}

	// "taken / total" over the optional parts of one product feature, so the
	// table shows at a glance which rows still have choices to make.
	function optionalSummary(node) {
		let entries = subFeatureEntriesOf(node)
		let total = 0
		let taken = 0
		for (let i = 0; i < entries.length; ++i) {
			if (!entries[i].optional)
				continue
			++total
			if (entryIsIncluded(entries[i]))
				++taken
		}
		if (total === 0)
			return "-"
		return taken + " / " + total
	}

	function openFeaturesDialog() {
		if (!canEdit || !productData)
			return
		ModalDialogManager.openDialog(featuresDialogComp, {
			"excludeFeatureIds": selectedFeatureIds(),
			"featuresModel": allFeaturesModel
		})
	}

	property Component featuresDialogComp: Component {
		FeaturesDialog {
			onFinished: {
				if (selectedIndexes.length === 0)
					return
				productViewContainer.productData.beginChanges()
				for (let i = 0; i < selectedIndexes.length; ++i) {
					let featureId = tableModel.getData("id", selectedIndexes[i])
					productViewContainer.addFeature(featureId)
				}
				productViewContainer.productData.endChanges()
				productViewContainer.updateFeaturesGui()
			}
		}
	}

	MultiPageView {
		id: multiPageView
		anchors.fill: parent
		panelWidth: Style.sizeHintXXS

		function updatePages() {
			multiPageView.clear()
			multiPageView.addPage("General", qsTr("General"), generalPageComp, "Icons/Settings")
			multiPageView.addPage("Features", qsTr("Features"), featuresPageComp, "Icons/FeaturePackage")
			multiPageView.currentIndex = 0
		}
	}

	Component {
		id: generalPageComp

		Item {
			id: generalPage
			anchors.fill: parent

			function updateGui() {
				generalGroup.updateGui()
			}

			function updateModel() {
				generalGroup.updateModel()
			}

			CustomScrollbar {
				id: scrollbar
				z: parent.z + 1
				anchors.right: parent.right
				anchors.top: flickable.top
				anchors.bottom: flickable.bottom
				secondSize: Style.marginM
				targetItem: flickable
			}

			Flickable {
				id: flickable
				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.right: scrollbar.left
				anchors.rightMargin: Style.marginXL
				contentWidth: width
				contentHeight: bodyColumn.height + 2 * Style.marginXL

				boundsBehavior: Flickable.StopAtBounds
				clip: true

				Column {
					id: bodyColumn
					anchors.horizontalCenter: parent.horizontalCenter
					width: Math.min(parent.width, Style.contentWidthMax)
					spacing: Style.marginXL

					GroupHeaderView {
						width: parent.width
						title: qsTr("General")
						groupView: generalGroup
					}

					GroupElementView {
						id: generalGroup
						width: parent.width

						TextInputElementView {
							id: productNameInput
							name: qsTr("Product name")
							placeHolderText: qsTr("Enter the product name")
							readOnly: !productViewContainer.canEdit
							onEditingFinished: productViewContainer.doUpdateModel()
							KeyNavigation.tab: categoryComboBox
						}

						ComboBoxElementView {
							id: categoryComboBox
							name: qsTr("Category")
							model: categoryModel
							nameId: "name"
							changeable: productViewContainer.canEdit
							onCurrentIndexChanged: productViewContainer.doUpdateModel()
						}

						function updateGui() {
							if (!productViewContainer.productData)
								return
							productNameInput.text = productViewContainer.productData.m_productName
							categoryComboBox.currentIndex = -1
							if (productViewContainer.productData.m_categoryId === "Software")
								categoryComboBox.currentIndex = 0
							else if (productViewContainer.productData.m_categoryId === "Hardware")
								categoryComboBox.currentIndex = 1
						}

						function updateModel() {
							if (!productViewContainer.productData)
								return
							productViewContainer.productData.m_productName = productNameInput.text
							productViewContainer.productData.m_productId = productNameInput.text.replace(/\s+/g, '')
							if (categoryComboBox.currentIndex === 0)
								productViewContainer.productData.m_categoryId = "Software"
							else if (categoryComboBox.currentIndex === 1)
								productViewContainer.productData.m_categoryId = "Hardware"
							else
								productViewContainer.productData.m_categoryId = ""
						}
					}
				}
			}
		}
	}

	TreeItemModel {
		id: categoryModel

		Component.onCompleted: {
			let index = categoryModel.insertNewItem()
			categoryModel.setData("id", "Software", index)
			categoryModel.setData("name", "Software", index)
			index = categoryModel.insertNewItem()
			categoryModel.setData("id", "Hardware", index)
			categoryModel.setData("name", "Hardware", index)
		}
	}

	Component {
		id: featuresPageComp

		Item {
			id: featuresPage
			anchors.fill: parent

			function updateGui() {
				productViewContainer.updateFeaturesGui()
			}

			function updateModel() {
			}

			// Column geometry, shared by the header and the rows so the two can
			// never drift. A column whose breakpoint is above the current table
			// width folds away and its share is handed to the columns that stay.
			property var columnFractions: [0.26, 0.18, 0.26, 0.12, 0.18]
			property var columnBreakpoints: [0, 380, 640, 300, 480]

			function columnVisible(index, width) {
				return width >= featuresPage.columnBreakpoints[index]
			}

			function columnWidth(index, width, spacing) {
				if (!featuresPage.columnVisible(index, width))
					return 0
				let sum = 0
				let count = 0
				for (let i = 0; i < featuresPage.columnFractions.length; ++i) {
					if (!featuresPage.columnVisible(i, width))
						continue
					sum += featuresPage.columnFractions[i]
					++count
				}
				return (width - (count - 1) * spacing) * featuresPage.columnFractions[index] / sum
			}

			property Component productFeaturesHeaderComp: Component {
				Row {
					id: headerRow
					anchors.fill: parent
					spacing: Style.spacingM

					Item {
						width: featuresPage.columnWidth(0, headerRow.width, headerRow.spacing)
						height: headerRow.height
						visible: width > 0
						BaseText {
							anchors.left: parent.left
							anchors.leftMargin: Style.marginXS
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							text: qsTr("Feature")
							font.family: Style.fontFamilyBold
							font.pixelSize: Style.fontSizeS
							color: Style.subtitleColor
						}
					}
					Item {
						width: featuresPage.columnWidth(1, headerRow.width, headerRow.spacing)
						height: headerRow.height
						visible: width > 0
						BaseText {
							anchors.left: parent.left
							anchors.leftMargin: Style.marginXS
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							text: qsTr("Feature ID")
							font.family: Style.fontFamilyBold
							font.pixelSize: Style.fontSizeS
							color: Style.subtitleColor
						}
					}
					Item {
						width: featuresPage.columnWidth(2, headerRow.width, headerRow.spacing)
						height: headerRow.height
						visible: width > 0
						BaseText {
							anchors.left: parent.left
							anchors.leftMargin: Style.marginXS
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							text: qsTr("Description")
							font.family: Style.fontFamilyBold
							font.pixelSize: Style.fontSizeS
							color: Style.subtitleColor
						}
					}
					Item {
						width: featuresPage.columnWidth(3, headerRow.width, headerRow.spacing)
						height: headerRow.height
						visible: width > 0
						BaseText {
							anchors.centerIn: parent
							width: parent.width
							horizontalAlignment: Text.AlignHCenter
							text: qsTr("Sub-features")
							font.family: Style.fontFamilyBold
							font.pixelSize: Style.fontSizeS
							color: Style.subtitleColor
						}
					}
					Item {
						width: featuresPage.columnWidth(4, headerRow.width, headerRow.spacing)
						height: headerRow.height
						visible: width > 0
						BaseText {
							anchors.centerIn: parent
							width: parent.width
							horizontalAlignment: Text.AlignHCenter
							text: qsTr("Optional parts")
							font.family: Style.fontFamilyBold
							font.pixelSize: Style.fontSizeS
							color: Style.subtitleColor
						}
					}
				}
			}

			// Product features are references to the shared feature collection, so
			// there is nothing to type here - the row is plain text and the whole of
			// it selects. What a product actually decides, which optional parts are
			// in, is edited in the panel on the right.
			property Component productFeatureRowComp: Component {
				Row {
					id: rowContent
					anchors.fill: parent
					spacing: Style.spacingM

					property var node: parent ? parent.node : null
					property var nodeData: rowContent.node && rowContent.node.data ? rowContent.node.data : null

					Item {
						width: featuresPage.columnWidth(0, rowContent.width, rowContent.spacing)
						height: rowContent.height
						visible: width > 0
						BaseText {
							anchors.left: parent.left
							anchors.leftMargin: Style.marginXS
							anchors.right: parent.right
							anchors.rightMargin: Style.marginXS
							anchors.verticalCenter: parent.verticalCenter
							text: rowContent.nodeData ? rowContent.nodeData.featureName || qsTr("Untitled feature") : ""
							font.family: Style.fontFamilyBold
							color: Style.textColor
							elide: Text.ElideRight
						}
					}
					Item {
						width: featuresPage.columnWidth(1, rowContent.width, rowContent.spacing)
						height: rowContent.height
						visible: width > 0
						BaseText {
							anchors.left: parent.left
							anchors.leftMargin: Style.marginXS
							anchors.right: parent.right
							anchors.rightMargin: Style.marginXS
							anchors.verticalCenter: parent.verticalCenter
							text: rowContent.nodeData ? rowContent.nodeData.featureId || "" : ""
							color: Style.subtitleColor
							elide: Text.ElideRight
						}
					}
					Item {
						width: featuresPage.columnWidth(2, rowContent.width, rowContent.spacing)
						height: rowContent.height
						visible: width > 0
						BaseText {
							anchors.left: parent.left
							anchors.leftMargin: Style.marginXS
							anchors.right: parent.right
							anchors.rightMargin: Style.marginXS
							anchors.verticalCenter: parent.verticalCenter
							text: rowContent.nodeData && rowContent.nodeData.description
								? rowContent.nodeData.description : qsTr("No description")
							color: rowContent.nodeData && rowContent.nodeData.description
								? Style.subtitleColor : Style.inactiveTextColor
							elide: Text.ElideRight
						}
					}

					// How much this feature brings with it, and how much of that is
					// still a choice - the two numbers the panel on the right details.
					Item {
						id: subfeaturesCell
						width: featuresPage.columnWidth(3, rowContent.width, rowContent.spacing)
						height: rowContent.height
						visible: width > 0

						property int count: productViewContainer.subFeatureCount(rowContent.node)

						Rectangle {
							anchors.centerIn: parent
							width: Math.max(Style.controlHeightS, countText.width + 2 * Style.marginS)
							height: Style.controlHeightS
							radius: height / 2
							visible: subfeaturesCell.count > 0
							color: Style.backgroundColor2
							border.color: Style.borderColor
							border.width: 1

							BaseText {
								id: countText
								anchors.centerIn: parent
								text: subfeaturesCell.count
								font.pixelSize: Style.fontSizeS
								font.family: Style.fontFamilyBold
								color: Style.titleColor
							}
						}

						BaseText {
							anchors.centerIn: parent
							visible: subfeaturesCell.count === 0
							text: "-"
							color: Style.inactiveTextColor
						}
					}

					Item {
						width: featuresPage.columnWidth(4, rowContent.width, rowContent.spacing)
						height: rowContent.height
						visible: width > 0
						BaseText {
							anchors.centerIn: parent
							width: parent.width
							horizontalAlignment: Text.AlignHCenter
							text: productViewContainer.optionalSummary(rowContent.node)
							color: Style.subtitleColor
							elide: Text.ElideRight
						}
					}
				}
			}

			// Right-hand table of the page: everything the selected product feature
			// brings along, each with its full path. Mandatory parts are shown checked
			// and locked; only the optional ones are a decision this product makes.
			property Component featureContentPanelComp: Component {
				CheckableListPanel {
					title: qsTr("Feature content")
					model: productViewContainer.subFeatureEntries
					contentActive: productViewContainer.selectedNode !== null
					revealKey: productViewContainer.selectedRootFeatureId()
					placeholderText: qsTr("No feature selected")
					placeholderDescription: qsTr("Pick a row on the left to choose which of its optional parts this product takes")
					nameColumnTitle: qsTr("Sub-feature")
					iconSource: "Icons/FeatureList"
					searchPlaceholder: qsTr("Search sub-features")
					emptyText: qsTr("No sub-features")
					emptyDescription: qsTr("This feature is included as a whole")
					footerText: qsTr("Which optional parts a license actually unlocks is decided on the Features page of that license")
					subtitle: productViewContainer.selectedFeatureName()
					actionText: productViewContainer.optionalEntries().length === 0 ? ""
						: productViewContainer.allOptionalIncluded() ? qsTr("Clear") : qsTr("Take all")
					actionEnabled: productViewContainer.canEdit
					entryTitleProvider: function(entry) { return entry.featureName }
					entrySubtitleProvider: function(entry) { return entry.fullPath }
					entryBadgeProvider: function(entry) { return entry.optional ? qsTr("Optional") : qsTr("Always included") }
					entrySearchableTextProvider: function(entry) { return entry.fullPath + " " + entry.featureId }
					entryCheckStateProvider: function(entry) { return productViewContainer.entryIsIncluded(entry) ? Qt.Checked : Qt.Unchecked }
					entryCheckEnabledProvider: function(entry) { return productViewContainer.canEdit && entry.optional }
					onToggleRequested: productViewContainer.toggleSubFeature(entry)
					onActionRequested: productViewContainer.toggleAllOptional()
				}
			}

			TreeExplorerView {
				id: treeExplorer
				anchors.fill: parent
				anchors.margins: Style.marginXL
				model: productViewContainer.featureTree
				rootTitle: productViewContainer.productData && productViewContainer.productData.m_productName
					? productViewContainer.productData.m_productName : qsTr("Product")
				createButtonText: qsTr("Add feature")
				emptyText: qsTr("No features added")
				emptyDescription: qsTr("Use Add feature to build this product")
				idleHintText: qsTr("Select a feature to choose which of its parts this product includes")
				selectedHintText: qsTr("Tick the optional parts on the right; the rest always comes with the feature")
				editable: productViewContainer.canEdit
				navigationEnabled: false
				renameVisible: false
				moveVisible: false
				editVisible: false
				rowIconVisible: false
				headerContentComponent: featuresPage.productFeaturesHeaderComp
				rowContentComponent: featuresPage.productFeatureRowComp
				sidePanelComponent: featuresPage.featureContentPanelComp
				textProvider: function(node) { return node && node.data ? node.data.featureName || qsTr("Untitled feature") : "" }
				descriptionProvider: function(node) { return node && node.data ? node.data.description || "" : "" }
				secondaryTextProvider: function(node) { return node && node.data ? node.data.featureId || "" : "" }
				onSelectedNodeChanged: productViewContainer.selectRootFeature(selectedNode)
				onCreateRequested: productViewContainer.openFeaturesDialog()
				onRemoveNodesRequested: productViewContainer.removeRootFeatures(nodes)
			}
		}
	}
}
