import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtauthgui 1.0
import imtlicgui 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtlicLicensesSdl 1.0
import imtlicProductsSdl 1.0
import imtlicFeaturesSdl 1.0

// A license is a product, a pick of that product's features, and the licenses
// it inherits from. General holds the first; the Features page holds the other
// two side by side, laid out like FeatureEditor and ProductView.
ViewBase {
	id: licenseEditor
	contentColor: Style.baseColor

	property TreeItemModel allFeaturesModel: CachedFeatureCollection.collectionModel
	property LicenseDefinitionData licenseData: model ? model : null
	// Rows of the features table, in TreeModelBuilder node shape so that
	// TreeExplorerView can render them: the payload sits in data.entry.
	property var featureNodes: []
	property var licenseEntries: []
	// m_features and m_parentLicenses, split once and kept here: reassigning
	// them is what re-evaluates every "is this ticked" binding on the page.
	property var featureIds: []
	property var parentLicenseIds: []
	property string productId: ""
	property bool canEdit: false

	Component.onCompleted: {
		canEdit = PermissionsController.checkPermission("ChangeLicenseDefinition")
		CachedFeatureCollection.updateModel()
		CachedProductCollection.updateModel()
		CachedLicenseCollection.updateModel()
		CachedFeatureCollection.modelUpdated.connect(licenseEditor.onCollectionsChanged)
		CachedLicenseCollection.modelUpdated.connect(licenseEditor.onCollectionsChanged)
		multiPageView.updatePages()
		multiPageView.ensurePageLoaded(0)
		multiPageView.ensurePageLoaded(1)
		multiPageView.ensurePageLoaded(2)
	}

	Component.onDestruction: {
		CachedFeatureCollection.modelUpdated.disconnect(licenseEditor.onCollectionsChanged)
		CachedLicenseCollection.modelUpdated.disconnect(licenseEditor.onCollectionsChanged)
	}

	LocalizationEvent {
		onLocalizationChanged: licenseEditor.rebuildEntries()
	}

	function onCollectionsChanged() {
		rebuildEntries()
	}

	function updateGui() {
		if (!licenseData)
			return
		// Ids first: filling the General page moves the product combo box, and
		// setProductId() only knows the move is not a real change if it can
		// compare against the product this license was loaded with. Refreshing
		// afterwards was clearing m_features on every open.
		refreshIds()
		var generalPageInstance = multiPageView.getPageByIndex(0)
		if (generalPageInstance)
			generalPageInstance.updateGui()
		rebuildEntries()
	}

	function updateModel() {
		if (!licenseData)
			return
		var generalPageInstance = multiPageView.getPageByIndex(0)
		if (generalPageInstance)
			generalPageInstance.updateModel()
	}

	function refreshIds() {
		if (!licenseData) {
			featureIds = []
			parentLicenseIds = []
			productId = ""
			return
		}
		featureIds = licenseData.m_features ? licenseData.m_features.split(';') : []
		parentLicenseIds = licenseData.m_parentLicenses ? licenseData.m_parentLicenses.split(';') : []
		productId = licenseData.m_productId || ""
	}

	function applyFeatureIds(ids) {
		featureIds = ids
		if (licenseData)
			licenseData.m_features = ids.join(';')
		updatePageBadges()
	}

	function applyParentLicenseIds(ids) {
		parentLicenseIds = ids
		if (licenseData)
			licenseData.m_parentLicenses = ids.join(';')
		rebuildInheritedEntries()
		updatePageBadges()
	}

	function setProductId(newProductId) {
		if (newProductId === productId)
			return
		productId = newProductId
		if (licenseData)
			licenseData.m_productId = newProductId
		// Both picks belong to the old product, so neither survives the switch.
		applyFeatureIds([])
		applyParentLicenseIds([])
		rebuildEntries()
	}

	function productName() {
		for (let i = 0; i < CachedProductCollection.collectionModel.getItemsCount(); ++i) {
			let id = CachedProductCollection.collectionModel.getData(ProductItemTypeMetaInfo.s_id, i)
			if (id === licenseEditor.productId)
				return CachedProductCollection.collectionModel.getData(ProductItemTypeMetaInfo.s_productName, i)
		}
		return ""
	}

	function productFeatureIds() {
		for (let i = 0; i < CachedProductCollection.collectionModel.getItemsCount(); ++i) {
			let id = CachedProductCollection.collectionModel.getData(ProductItemTypeMetaInfo.s_id, i)
			if (id !== licenseEditor.productId)
				continue
			let features = CachedProductCollection.collectionModel.getData(ProductItemTypeMetaInfo.s_features, i)
			return features ? features.split(';') : []
		}
		return []
	}

	function rebuildEntries() {
		rebuildFeatureNodes()
		rebuildLicenseEntries()
		rebuildInheritedEntries()
	}

	// The features the license's product offers, as a tree: the top level is the
	// product's features, and opening one shows what is inside it. A whole
	// feature is granted by its own id; an optional part inside one is granted
	// by "<featureUuid>/<subFeatureId>", which the license stores verbatim.
	function rebuildFeatureNodes() {
		let wanted = productFeatureIds()
		let tree = TreeModelBuilder.fromTreeItemModelByFields(allFeaturesModel, {
			key: FeatureItemTypeMetaInfo.s_id,
			children: FeatureItemTypeMetaInfo.s_subFeatures,
			columns: {
				id: FeatureItemTypeMetaInfo.s_id,
				featureName: FeatureItemTypeMetaInfo.s_featureName,
				featureId: FeatureItemTypeMetaInfo.s_featureId,
				description: FeatureItemTypeMetaInfo.s_description,
				optional: FeatureItemTypeMetaInfo.s_optional
			}
		})
		let nodes = []
		for (let i = 0; i < tree.length; ++i) {
			let data = tree[i].data || {}
			let id = data.id || ""
			if (wanted.indexOf(id) < 0)
				continue
			nodes.push(buildFeatureNode(tree[i], id, true))
		}
		featureNodes = nodes
		updatePageBadges()
	}

	function buildFeatureNode(treeNode, rootId, isRoot) {
		let data = treeNode.data || {}
		let name = data.featureName || data.featureId || ""
		// The root of a branch is granted under the feature's document id; a
		// node inside it under the composite key.
		let grantId = isRoot ? rootId : rootId + "/" + (data.featureId || "")
		let sourceChildren = treeNode.children || []
		let children = []
		for (let i = 0; i < sourceChildren.length; ++i)
			children.push(buildFeatureNode(sourceChildren[i], rootId, false))
		return {
			"key": grantId,
			"text": name,
			"children": children,
			"data": {
				"entry": {
					"id": grantId,
					"rootId": rootId,
					"isRoot": isRoot,
					"optional": data.optional === true,
					"isLeaf": children.length === 0,
					"featureName": name,
					"featureId": data.featureId || "",
					"description": data.description || "",
					"subFeatureCount": children.length
				}
			}
		}
	}

	// What a tick means, per kind of row:
	//   a product feature      - granted by this license, on its own id
	//   a mandatory part       - comes with its feature, nothing to decide
	//   an optional part       - granted separately, once the feature is granted
	//   a part that has parts  - a grouping; its children carry the decision
	function entryIsGranted(entry) {
		if (!entry)
			return false
		if (entry.isRoot)
			return featureIsIncluded(entry.id)
		if (!entry.isLeaf || !entry.optional)
			return featureIsIncluded(entry.rootId)
		return featureIds.indexOf(entry.id) >= 0
	}

	function entryIsChangeable(entry) {
		if (!canEdit || !entry)
			return false
		if (entry.isRoot)
			return true
		return entry.isLeaf && entry.optional && featureIsIncluded(entry.rootId)
	}

	function toggleEntry(entry) {
		if (!entryIsChangeable(entry))
			return
		if (entry.isRoot) {
			toggleFeature(entry.id)
			return
		}
		let ids = featureIds.slice()
		let index = ids.indexOf(entry.id)
		if (index >= 0)
			ids.splice(index, 1)
		else
			ids.push(entry.id)
		applyFeatureIds(ids)
	}

	// Why a row cannot be ticked, in the words the table shows next to it.
	function entryStateText(entry) {
		if (!entry || entry.isRoot)
			return ""
		if (!entry.isLeaf)
			return qsTr("Set inside")
		if (!entry.optional)
			return qsTr("Always included")
		if (!featureIsIncluded(entry.rootId))
			return qsTr("Grant the feature first")
		return qsTr("Optional")
	}

	// Counts on the page panel: how many features the product offers, and how
	// many of them this license grants.
	function updatePageBadges() {
		// Only whole features are counted; the optional parts granted inside
		// them carry a slash and are not features of the product.
		let granted = 0
		for (let i = 0; i < featureIds.length; ++i) {
			if (featureIds[i] !== "" && featureIds[i].indexOf('/') < 0)
				++granted
		}
		multiPageView.setPageBadge("Features", featureNodes.length > 0
			? granted + " / " + featureNodes.length : "")
		multiPageView.setPageBadge("Inheritance", parentLicenseIds.length > 0
			? "" + parentLicenseIds.length : "")
	}

	// Sibling licenses of the same product, minus this one and minus anything
	// that already inherits from it - inheriting both ways is a cycle.
	function rebuildLicenseEntries() {
		let entries = []
		let ownId = licenseData ? licenseData.m_id : ""
		for (let i = 0; i < CachedLicenseCollection.collectionModel.getItemsCount(); ++i) {
			let id = CachedLicenseCollection.collectionModel.getData(LicenseItemTypeMetaInfo.s_id, i)
			let parents = CachedLicenseCollection.collectionModel.getData(LicenseItemTypeMetaInfo.s_parentLicenses, i) || ""
			if (id === ownId || parents.split(';').indexOf(ownId) >= 0)
				continue
			let productUuid = CachedLicenseCollection.collectionModel.getData(LicenseItemTypeMetaInfo.s_productUuid, i)
			if (productUuid !== licenseEditor.productId)
				continue
			entries.push({
				"id": id,
				"licenseName": CachedLicenseCollection.collectionModel.getData(LicenseItemTypeMetaInfo.s_licenseName, i) || "",
				"licenseId": CachedLicenseCollection.collectionModel.getData(LicenseItemTypeMetaInfo.s_licenseId, i) || ""
			})
		}
		licenseEntries = entries
	}

	function nodeEntry(node) {
		return node && node.data ? node.data.entry : null
	}

	function featureIsIncluded(entryId) {
		return featureIds.indexOf(entryId) >= 0
	}

	function toggleFeature(entryId) {
		if (!canEdit || entryId === "")
			return
		let ids = []
		let wasIncluded = featureIsIncluded(entryId)
		for (let i = 0; i < featureIds.length; ++i) {
			let id = featureIds[i]
			// Dropping a feature drops the optional parts picked inside it,
			// otherwise they would linger as grants for something not granted.
			if (wasIncluded && (id === entryId || id.indexOf(entryId + "/") === 0))
				continue
			ids.push(id)
		}
		if (!wasIncluded)
			ids.push(entryId)
		applyFeatureIds(ids)
	}

	// --- inherited features ----------------------------------------------- //
	//
	// Inheriting a license means inheriting everything it grants, and everything
	// its own parents grant. The panel beside the table spells that out, so the
	// ticks on the left can be read as "and these on top".

	property var inheritedEntries: []

	function licenseRow(licenseId) {
		for (let i = 0; i < CachedLicenseCollection.collectionModel.getItemsCount(); ++i) {
			if (CachedLicenseCollection.collectionModel.getData(LicenseItemTypeMetaInfo.s_id, i) === licenseId)
				return i
		}
		return -1
	}

	function featureNameOf(featureId) {
		for (let i = 0; i < allFeaturesModel.getItemsCount(); ++i) {
			if (allFeaturesModel.getData(FeatureItemTypeMetaInfo.s_id, i) === featureId)
				return allFeaturesModel.getData(FeatureItemTypeMetaInfo.s_featureName, i) || featureId
		}
		return featureId
	}

	function featureCodeOf(featureId) {
		for (let i = 0; i < allFeaturesModel.getItemsCount(); ++i) {
			if (allFeaturesModel.getData(FeatureItemTypeMetaInfo.s_id, i) === featureId)
				return allFeaturesModel.getData(FeatureItemTypeMetaInfo.s_featureId, i) || ""
		}
		return ""
	}

	// Walks the parents of the parents too, and remembers which license first
	// brought a feature in - "seen" is what keeps a cycle from looping forever.
	function collectInherited(ids, seen, origins, order) {
		for (let i = 0; i < ids.length; ++i) {
			let licenseId = ids[i]
			if (licenseId === "" || seen[licenseId] === true)
				continue
			seen[licenseId] = true
			let row = licenseRow(licenseId)
			if (row < 0)
				continue
			let licenseName = CachedLicenseCollection.collectionModel.getData(LicenseItemTypeMetaInfo.s_licenseName, row) || licenseId
			let features = CachedLicenseCollection.collectionModel.getData(LicenseItemTypeMetaInfo.s_features, row) || ""
			let featureList = features === "" ? [] : features.split(';')
			for (let j = 0; j < featureList.length; ++j) {
				let featureId = featureList[j]
				if (featureId === "" || origins[featureId] !== undefined)
					continue
				origins[featureId] = licenseName
				order.push(featureId)
			}
			let parents = CachedLicenseCollection.collectionModel.getData(LicenseItemTypeMetaInfo.s_parentLicenses, row) || ""
			if (parents !== "")
				collectInherited(parents.split(';'), seen, origins, order)
		}
	}

	function rebuildInheritedEntries() {
		let seen = {}
		let origins = {}
		let order = []
		collectInherited(parentLicenseIds, seen, origins, order)
		let entries = []
		for (let i = 0; i < order.length; ++i) {
			let featureId = order[i]
			entries.push({
				"id": featureId,
				"featureName": featureNameOf(featureId),
				"featureId": featureCodeOf(featureId),
				"licenseName": origins[featureId]
			})
		}
		inheritedEntries = entries
	}

	// Granted twice: once by this license and once by a license it inherits.
	function isAlsoGrantedHere(entryId) {
		return featureIds.indexOf(entryId) >= 0
	}
	function licenseIsInherited(entryId) {
		return parentLicenseIds.indexOf(entryId) >= 0
	}

	function toggleParentLicense(entryId) {
		if (!canEdit || entryId === "")
			return
		let ids = parentLicenseIds.slice()
		let index = ids.indexOf(entryId)
		if (index >= 0)
			ids.splice(index, 1)
		else
			ids.push(entryId)
		applyParentLicenseIds(ids)
	}

	function clearParentLicenses() {
		if (!canEdit)
			return
		applyParentLicenseIds([])
	}

	MultiPageView {
		id: multiPageView
		anchors.fill: parent
		panelWidth: Style.sizeHintXXS

		function updatePages() {
			multiPageView.clear()
			multiPageView.addPage("General", qsTr("General"), generalPageComp, "Icons/Settings")
			multiPageView.addPage("Features", qsTr("Features"), featuresPageComp, "Icons/FeaturePackage")
			multiPageView.addPage("Inheritance", qsTr("Inherited licenses"), inheritancePageComp, "Icons/License")
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
							id: licenseNameInput
							name: qsTr("License name")
							placeHolderText: qsTr("Enter the license name")
							readOnly: !licenseEditor.canEdit
							onEditingFinished: {
								if (licenseIdInput.text === "")
									licenseIdInput.text = text.replace(/\s+/g, '')
								licenseEditor.doUpdateModel()
							}
							KeyNavigation.tab: licenseIdInput
						}

						TextInputElementView {
							id: licenseIdInput
							name: qsTr("License ID")
							placeHolderText: qsTr("Enter the license ID")
							readOnly: !licenseEditor.canEdit
							onEditingFinished: licenseEditor.doUpdateModel()
							KeyNavigation.tab: descriptionInput
							KeyNavigation.backtab: licenseNameInput
						}

						TextInputElementView {
							id: descriptionInput
							name: qsTr("Description")
							placeHolderText: qsTr("Enter the description")
							readOnly: !licenseEditor.canEdit
							onEditingFinished: licenseEditor.doUpdateModel()
							KeyNavigation.backtab: licenseIdInput
						}

						ComboBoxElementView {
							id: productComboBox
							name: qsTr("Product")
							model: CachedProductCollection.collectionModel
							nameId: "productName"
							changeable: licenseEditor.canEdit
							onCurrentIndexChanged: {
								if (licenseEditor.guiIsBlocked())
									return
								licenseEditor.setProductId(generalGroup.currentProductId())
								licenseEditor.doUpdateModel()
							}
						}

						function currentProductId() {
							if (productComboBox.currentIndex < 0)
								return ""
							return CachedProductCollection.collectionModel.getData(
								ProductItemTypeMetaInfo.s_id, productComboBox.currentIndex)
						}

						function updateGui() {
							if (!licenseEditor.licenseData)
								return
							licenseNameInput.text = licenseEditor.licenseData.m_licenseName || ""
							licenseIdInput.text = licenseEditor.licenseData.m_licenseId || ""
							descriptionInput.text = licenseEditor.licenseData.m_description || ""
							productComboBox.currentIndex = -1
							for (let i = 0; i < CachedProductCollection.collectionModel.getItemsCount(); ++i) {
								let id = CachedProductCollection.collectionModel.getData(ProductItemTypeMetaInfo.s_id, i)
								if (id === licenseEditor.licenseData.m_productId) {
									productComboBox.currentIndex = i
									break
								}
							}
						}

						function updateModel() {
							if (!licenseEditor.licenseData)
								return
							licenseEditor.licenseData.m_licenseName = licenseNameInput.text
							licenseEditor.licenseData.m_licenseId = licenseIdInput.text
							licenseEditor.licenseData.m_description = descriptionInput.text
						}
					}
				}
			}
		}
	}

	Component {
		id: featuresPageComp

		Item {
			id: featuresPage
			anchors.fill: parent

			function updateGui() {
				licenseEditor.refreshIds()
				licenseEditor.rebuildEntries()
			}

			function updateModel() {
			}

			// Column geometry, shared by the header and the rows so the two can
			// never drift. A column whose breakpoint is above the current table
			// width folds away and its share is handed to the columns that stay.
			property var columnFractions: [0.10, 0.28, 0.20, 0.28, 0.14]
			property var columnBreakpoints: [0, 0, 380, 620, 300]

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

			property Component featuresHeaderComp: Component {
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
							text: qsTr("In license")
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
							text: qsTr("Feature")
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
							text: qsTr("Feature ID")
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
						width: featuresPage.columnWidth(4, headerRow.width, headerRow.spacing)
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
				}
			}

			// The features come from the product, so nothing here is typed in: the
			// only decision the row carries is the tick in its first column.
			property Component featureRowComp: Component {
				Row {
					id: rowContent
					anchors.fill: parent
					spacing: Style.spacingM

					property var node: parent ? parent.node : null
					property var entry: licenseEditor.nodeEntry(rowContent.node)

					Item {
						width: featuresPage.columnWidth(0, rowContent.width, rowContent.spacing)
						height: rowContent.height
						visible: width > 0

						CheckBox {
							anchors.left: parent.left
							anchors.leftMargin: Style.marginXS
							anchors.verticalCenter: parent.verticalCenter
							visible: rowContent.entry !== null && (rowContent.entry.isRoot
								|| (rowContent.entry.isLeaf && rowContent.entry.optional))
							isActive: licenseEditor.entryIsChangeable(rowContent.entry)
							checkState: licenseEditor.entryIsGranted(rowContent.entry) ? Qt.Checked : Qt.Unchecked
							mouseArea.enabled: false
						}

						// A row that carries no decision of its own says why
						// instead of showing a box nobody can tick.
						BaseText {
							anchors.left: parent.left
							anchors.leftMargin: Style.marginXS
							anchors.right: parent.right
							anchors.rightMargin: Style.marginXS
							anchors.verticalCenter: parent.verticalCenter
							visible: rowContent.entry !== null && !rowContent.entry.isRoot
								&& !(rowContent.entry.isLeaf && rowContent.entry.optional)
							text: licenseEditor.entryStateText(rowContent.entry)
							font.pixelSize: Style.fontSizeS
							color: Style.inactiveTextColor
							elide: Text.ElideRight
						}

						MouseArea {
							anchors.fill: parent
							enabled: licenseEditor.entryIsChangeable(rowContent.entry)
							cursorShape: Qt.PointingHandCursor
							onClicked: licenseEditor.toggleEntry(rowContent.entry)
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
							text: rowContent.entry ? rowContent.entry.featureName : ""
							font.family: Style.fontFamilyBold
							color: Style.textColor
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
							text: rowContent.entry ? rowContent.entry.featureId : ""
							color: Style.subtitleColor
							elide: Text.ElideRight
						}
					}
					Item {
						width: featuresPage.columnWidth(3, rowContent.width, rowContent.spacing)
						height: rowContent.height
						visible: width > 0
						BaseText {
							anchors.left: parent.left
							anchors.leftMargin: Style.marginXS
							anchors.right: parent.right
							anchors.rightMargin: Style.marginXS
							anchors.verticalCenter: parent.verticalCenter
							text: rowContent.entry && rowContent.entry.description
								? rowContent.entry.description : qsTr("No description")
							color: rowContent.entry && rowContent.entry.description
								? Style.subtitleColor : Style.inactiveTextColor
							elide: Text.ElideRight
						}
					}
					Item {
						id: subfeaturesCell
						width: featuresPage.columnWidth(4, rowContent.width, rowContent.spacing)
						height: rowContent.height
						visible: width > 0

						property int count: rowContent.entry ? rowContent.entry.subFeatureCount : 0
						// Opening a feature is how its optional parts are reached,
						// and there is nothing to reach until it is granted.
						property bool openable: subfeaturesCell.count > 0
							&& licenseEditor.entryIsGranted(rowContent.entry)

						Rectangle {
							anchors.centerIn: parent
							width: Math.max(Style.controlHeightS, countText.width + 2 * Style.marginS)
							height: Style.controlHeightS
							radius: height / 2
							visible: subfeaturesCell.count > 0
							color: subfeaturesMouse.containsMouse ? Style.titleColor : Style.backgroundColor2
							border.color: Style.borderColor
							border.width: 1

							BaseText {
								id: countText
								anchors.centerIn: parent
								text: subfeaturesCell.count
								font.pixelSize: Style.fontSizeS
								font.family: Style.fontFamilyBold
								color: subfeaturesMouse.containsMouse ? Style.baseColor
									: subfeaturesCell.openable ? Style.titleColor : Style.inactiveTextColor
							}
						}

						BaseText {
							anchors.centerIn: parent
							visible: subfeaturesCell.count === 0
							text: "-"
							color: Style.inactiveTextColor
						}

						MouseArea {
							id: subfeaturesMouse
							anchors.fill: parent
							hoverEnabled: true
							enabled: subfeaturesCell.openable
							cursorShape: Qt.PointingHandCursor
							onClicked: treeExplorer.navigateInto(rowContent.node)
						}
					}
				}
			}

			// Right-hand table of the page: what this license gets for free from
			// the licenses it inherits. Read-only - the ticks that produce it are
			// on the Inherited licenses page.
			property Component inheritedFeaturesPanelComp: Component {
				CheckableListPanel {
					title: qsTr("Inherited features")
					subtitle: licenseEditor.inheritedEntries.length > 0
						? qsTr("from %1 licenses").arg(licenseEditor.parentLicenseIds.length) : ""
					model: licenseEditor.inheritedEntries
					contentActive: licenseEditor.parentLicenseIds.length > 0
					revealKey: licenseEditor.parentLicenseIds.join(';')
					placeholderText: qsTr("Nothing inherited")
					placeholderDescription: qsTr("Licenses picked on the Inherited licenses page bring their features here, and this license grants them too")
					nameColumnTitle: qsTr("Feature")
					iconSource: "Icons/License"
					searchPlaceholder: qsTr("Search inherited features")
					emptyText: qsTr("The inherited licenses grant nothing")
					emptyDescription: qsTr("They carry no features of their own yet")
					footerText: qsTr("Granted on top of the features ticked on the left")
					entryTitleProvider: function(entry) { return entry.featureName }
					entrySubtitleProvider: function(entry) { return qsTr("from %1").arg(entry.licenseName) }
					entryBadgeProvider: function(entry) {
						return licenseEditor.isAlsoGrantedHere(entry.id) ? qsTr("Also ticked here") : ""
					}
					entrySearchableTextProvider: function(entry) { return entry.featureName + " " + entry.featureId + " " + entry.licenseName }
					entryCheckStateProvider: function(entry) { return Qt.Checked }
					entryCheckEnabledProvider: function(entry) { return false }
				}
			}
			TreeExplorerView {
				id: treeExplorer
				anchors.fill: parent
				anchors.margins: Style.marginXL
				model: licenseEditor.featureNodes
				rootTitle: licenseEditor.productName() !== "" ? licenseEditor.productName() : qsTr("No product")
				emptyText: licenseEditor.productId === "" ? qsTr("No product chosen")
					: qsTr("This product has no features")
				emptyDescription: licenseEditor.productId === ""
					? qsTr("Pick a product on the General page first") : ""
				idleHintText: qsTr("Tick the features this license grants; open one to reach its optional parts")
				selectedHintText: qsTr("Open a feature to grant its optional parts one by one")
				editable: licenseEditor.canEdit
				createVisible: false
				removeVisible: false
				multiSelectEnabled: false
				renameVisible: false
				moveVisible: false
				editVisible: false
				rowIconVisible: false
				headerContentComponent: featuresPage.featuresHeaderComp
				rowContentComponent: featuresPage.featureRowComp
				sidePanelComponent: featuresPage.inheritedFeaturesPanelComp
				// Only a granted feature can be opened - there is nothing to
				// decide inside one that is not granted at all.
				containerProvider: function(node) {
					let entry = licenseEditor.nodeEntry(node)
					return entry !== null && entry.subFeatureCount > 0 && licenseEditor.entryIsGranted(entry)
				}
				textProvider: function(node) { return licenseEditor.nodeEntry(node) ? licenseEditor.nodeEntry(node).featureName : "" }
				descriptionProvider: function(node) { return licenseEditor.nodeEntry(node) ? licenseEditor.nodeEntry(node).description : "" }
				secondaryTextProvider: function(node) { return licenseEditor.nodeEntry(node) ? licenseEditor.nodeEntry(node).featureId : "" }
			}
		}
	}

	Component {
		id: inheritancePageComp

		Item {
			id: inheritancePage
			anchors.fill: parent

			function updateGui() {
				licenseEditor.refreshIds()
				licenseEditor.rebuildEntries()
			}

			function updateModel() {
			}

			// A page of its own rather than a panel beside the features: what a
			// license inherits is a different question from what it grants.
			CheckableListPanel {
				anchors.fill: parent
				anchors.margins: Style.marginXL
				title: qsTr("Inherited licenses")
				subtitle: licenseEditor.productName()
				model: licenseEditor.licenseEntries
				contentActive: licenseEditor.productId !== ""
				revealKey: licenseEditor.productId
				placeholderText: qsTr("No product chosen")
				placeholderDescription: qsTr("Pick a product on the General page first")
				nameColumnTitle: qsTr("License")
				iconSource: "Icons/License"
				searchPlaceholder: qsTr("Search licenses")
				emptyText: qsTr("Nothing to inherit from")
				emptyDescription: qsTr("Only other licenses of the same product can be inherited")
				actionText: qsTr("Clear")
				actionEnabled: licenseEditor.canEdit && licenseEditor.parentLicenseIds.length > 0
				entryTitleProvider: function(entry) { return entry.licenseName }
				entrySubtitleProvider: function(entry) { return entry.licenseId }
				entrySearchableTextProvider: function(entry) { return entry.licenseName + " " + entry.licenseId }
				entryCheckStateProvider: function(entry) { return licenseEditor.licenseIsInherited(entry.id) ? Qt.Checked : Qt.Unchecked }
				entryCheckEnabledProvider: function(entry) { return licenseEditor.canEdit }
				onToggleRequested: licenseEditor.toggleParentLicense(entry.id)
				onActionRequested: licenseEditor.clearParentLicenses()
			}
		}
	}
}
