import QtQuick 2.12
import Qt5Compat.GraphicalEffects
import QtGraphicalEffects 1.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtauthgui 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtlicFeaturesSdl 1.0

ViewBase {
	id: featureEditor
	contentColor: Style.baseColor

	property TreeItemModel featureCollectionModel: TreeItemModel {}
	property string featureId: ""
	property FeatureData featureData: model
	property var featureTree: []
	property var requirementEntries: []
	property var requirementGraph: []
	property var selectedFeature: null
	property var activeFeature: null
	property bool canEdit: false
	// Part of the product this editor works in; scopes its permission checks and
	// travels with the requests its data providers send.
	property string permissionPath: ""
	// Full path most recently put on the clipboard, so every copy control can
	// confirm what it did by itself instead of raising a dialog.
	property string copiedPath: ""
	// Path of the feature being edited; every sub-feature path grows from it.
	property string rootFeaturePath: featureData && featureData.m_featureId
		? "/" + featureData.m_featureId : ""

	Component.onCompleted: {
		canEdit = PermissionsController.checkPermission("ChangeFeature")
		featureCollectionModel.copy(CachedFeatureCollection.collectionModel)
		rebuildRequirementEntries()
		CachedFeatureCollection.updateModel()
		__updateCommandStates()
		multiPageView.updatePages()
		// Both pages are loaded up-front (rather than lazily on first visit) because
		// the root-level functions below (rebuildFeatureTree, updateActiveFeature, etc.)
		// reach the Subfeatures page via multiPageView.getPageById("Subfeatures"), which
		// only returns a live instance once the page has been loaded.
		multiPageView.ensurePageLoaded(0)
		multiPageView.ensurePageLoaded(1)
	}

	// A feature is addressed by the path of identifiers that leads to it, e.g.
	// "/Administration/EditUser/AddUser". Requirements and permissions are written
	// in exactly that form, so nothing has to guess which branch an identifier came from.
	TextEdit {
		id: clipboardProxy
		width: 0
		height: 0
		visible: false
		readOnly: true
	}

	Timer {
		id: copiedHintTimer
		interval: 2000
		onTriggered: featureEditor.copiedPath = ""
	}

	function featurePath(item) {
		if (!item || rootFeaturePath === "")
			return ""
		if (item === featureData)
			return rootFeaturePath
		return findFeaturePath(featureTree, item, rootFeaturePath)
	}

	function findFeaturePath(nodes, item, prefix) {
		for (let i = 0; i < nodes.length; ++i) {
			let current = sourceItem(nodes[i])
			if (!current)
				continue
			let path = prefix + "/" + (current.m_featureId || "")
			if (current === item)
				return path
			let childPath = findFeaturePath(nodes[i].children || [], item, path)
			if (childPath !== "")
				return childPath
		}
		return ""
	}

	function copyFeaturePath(item) {
		let path = featurePath(item)
		if (path === "")
			return
		clipboardProxy.text = path
		clipboardProxy.selectAll()
		clipboardProxy.copy()
		clipboardProxy.select(0, 0)
		copiedPath = path
		copiedHintTimer.restart()
	}

	function copyIconSource(path) {
		return "qrc:/" + Style.getIconPath(featureEditor.copiedPath !== "" && featureEditor.copiedPath === path
			? "Icons/Ok" : "Icons/Copy", Icon.State.On, Icon.Mode.Normal)
	}

	LocalizationEvent {
		onLocalizationChanged: featureEditor.rebuildFeatureTree(featureEditor.selectedFeature)
	}

	onFeatureDataChanged: {
		if (!featureData)
			return
		if (!featureData.hasSubFeatures())
			featureData.emplaceSubFeatures()
		selectedFeature = null
		rebuildFeatureTree(null)
		updateActiveFeature()
	}

	function __updateCommandStates() {
		if (!commandsController)
			return
		commandsController.setCommandIsEnabled("InsertFeature", canEdit && featureData !== null)
		commandsController.setCommandIsEnabled("RemoveFeature", canEdit && selectedFeature !== null)
	}

	// Count on the Subfeatures tab of the page panel, so the size of the tree
	// is visible without opening it.
	function updatePageBadges() {
		multiPageView.setPageBadge("Subfeatures", featureTree.length > 0 ? "" + featureTree.length : "")
	}

	function rebuildFeatureTree(itemToSelect) {
		if (!featureData || !featureData.m_subFeatures) {
			featureTree = []
			updatePageBadges()
			return
		}
		featureTree = TreeModelBuilder.fromBaseModelByFields(featureData.m_subFeatures, {
			key: "m_featureId",
			text: "m_featureName",
			children: "m_subFeatures",
			columns: {
				featureId: "m_featureId",
				description: "m_description",
				optional: "m_optional",
				isPermission: "m_isPermission",
				requirements: "m_requirements"
			}
		})
		updatePageBadges()
		if (itemToSelect) {
			let node = findNodeBySourceItem(featureTree, itemToSelect)
			let page = multiPageView.getPageById("Subfeatures")
			if (node && page)
				page.selectNode(node)
		}
	}

	function findNodeBySourceItem(nodes, item) {
		for (let i = 0; i < nodes.length; ++i) {
			let node = nodes[i]
			if (node.data && node.data.sourceItem === item)
				return node
			let child = findNodeBySourceItem(node.children || [], item)
			if (child)
				return child
		}
		return null
	}

	function sourceItem(node) {
		return node && node.data ? node.data.sourceItem : null
	}

	// Node accessors that go through to the live model item, so the explorer's
	// breadcrumb, search and default row text follow inline edits without a
	// tree rebuild (a rebuild recreates the delegates and drops the caret).
	function featureName(node) {
		let item = sourceItem(node)
		return item && item.m_featureName ? item.m_featureName : qsTr("Untitled feature")
	}

	function featureIdOf(node) {
		let item = sourceItem(node)
		return item && item.m_featureId ? item.m_featureId : ""
	}

	function featureDescription(node) {
		let item = sourceItem(node)
		return item && item.m_description ? item.m_description : ""
	}

	// A feature without a name or an identifier is not usable yet: it cannot be
	// referenced, so it is not allowed to own subfeatures either. Everything
	// that would descend into it or add to it checks this first.
	function itemIsComplete(item) {
		return item !== null && item !== undefined
			&& item.m_featureName !== "" && item.m_featureId !== ""
	}

	function nodeIsComplete(node) {
		return itemIsComplete(sourceItem(node))
	}

	function subtreeHasIssues(node) {
		let children = node && node.children ? node.children : []
		for (let i = 0; i < children.length; ++i) {
			if (!nodeIsComplete(children[i]) || subtreeHasIssues(children[i]))
				return true
		}
		return false
	}

	function incompleteCount(nodes) {
		let count = 0
		let list = nodes || []
		for (let i = 0; i < list.length; ++i) {
			if (!nodeIsComplete(list[i]))
				++count
		}
		return count
	}

	// The level being listed belongs to the last navigated node, or to the
	// feature being edited when we are at the top.
	function levelAcceptsChildren(stack) {
		if (!stack || stack.length === 0)
			return itemIsComplete(featureData)
		return nodeIsComplete(stack[stack.length - 1])
	}

	function parentItem(node) {
		return node ? sourceItem(node) : featureData
	}

	function childModel(node) {
		let item = parentItem(node)
		if (!item)
			return null
		if (!item.hasSubFeatures())
			item.emplaceSubFeatures()
		return item.m_subFeatures
	}

	function removeItem(modelToEdit, item) {
		if (!modelToEdit || !item)
			return false
		for (let i = 0; i < modelToEdit.getItemsCount(); ++i) {
			let wrapper = modelToEdit.get(i)
			if (wrapper && wrapper.item === item) {
				modelToEdit.remove(i)
				return true
			}
		}
		return false
	}

	// Names and identifiers of the level a new row would join, so the defaults
	// can be made unique instead of colliding with what is already there.
	function siblingNames(parentNode) {
		let taken = []
		let source = parentNode ? (parentNode.children || []) : featureTree
		for (let i = 0; i < source.length; ++i) {
			let item = sourceItem(source[i])
			if (item)
				taken.push(item.m_featureName)
		}
		return taken
	}

	function siblingIds(parentNode) {
		let taken = []
		let source = parentNode ? (parentNode.children || []) : featureTree
		for (let i = 0; i < source.length; ++i) {
			let item = sourceItem(source[i])
			if (item)
				taken.push(item.m_featureId)
		}
		return taken
	}

	// A Feature-ID only has to be unique among the siblings that share its
	// parent - not across the whole tree - since identity comes from the full
	// path, and two branches are free to reuse the same leaf id.
	function siblingIdConflict(parentNode, candidateId, excludeItem) {
		if (candidateId === "")
			return false
		let source = parentNode ? (parentNode.children || []) : featureTree
		for (let i = 0; i < source.length; ++i) {
			let sibling = sourceItem(source[i])
			if (sibling && sibling !== excludeItem && sibling.m_featureId === candidateId)
				return true
		}
		return false
	}

	// "Feature Name", then "Feature Name (2)", "Feature Name (3)" - the same
	// rule a file manager uses, so the numbering needs no explaining.
	function uniqueName(base, taken) {
		if (taken.indexOf(base) < 0)
			return base
		let index = 2
		while (taken.indexOf(base + " (" + index + ")") >= 0)
			++index
		return base + " (" + index + ")"
	}

	// The identifier follows the same numbering, sanitized: "FeatureName2".
	function uniqueId(base, taken) {
		if (base === "")
			return ""
		if (taken.indexOf(base) < 0)
			return base
		let index = 2
		while (taken.indexOf(base + index) >= 0)
			++index
		return base + index
	}

	function createSubFeature(parentNode) {
		let subFeatures = childModel(parentNode)
		if (!canEdit || !subFeatures || !featureData)
			return
		if (!itemIsComplete(parentItem(parentNode)))
			return
		let name = uniqueName(qsTr("Feature Name"), siblingNames(parentNode))
		let newFeature = featureData.createSubFeaturesArrayElement()
		newFeature.m_featureName = name
		newFeature.m_featureId = uniqueId(sanitizeId(name), siblingIds(parentNode))
		newFeature.m_isPermission = true
		subFeatures.addElement(newFeature)
		// Deliberately not selected: appearing already ticked is not something
		// the user asked for, and it hides whatever they had ticked before.
		rebuildFeatureTree(null)
	}

	// BaseModel raises the owner's modelChanged from insertElement() and
	// removeElement(); the plain ListModel remove() it wraps raises nothing.
	// Taking rows out with remove() therefore changed the list on screen and
	// left the document believing nothing had happened - no dirty flag, no undo
	// step. A batch raises it once, so removing five rows is one step.
	function notifyListChanged(listModel) {
		if (listModel && listModel.owner && listModel.owner.modelChanged)
			listModel.owner.modelChanged([])
	}

	// Rows are matched to indexes in one pass before anything is taken out, and
	// then removed from the back: removing shifts every index after it.
	function removeFeatureNodes(nodes, parentNode) {
		if (!canEdit || !nodes || nodes.length === 0)
			return
		let childs = childModel(parentNode)
		if (!childs)
			return
		let items = []
		for (let i = 0; i < nodes.length; ++i) {
			let item = sourceItem(nodes[i])
			if (item)
				items.push(item)
		}
		let indexes = []
		for (let row = 0; row < childs.getItemsCount(); ++row) {
			let wrapper = childs.get(row)
			if (wrapper && items.indexOf(wrapper.item) >= 0)
				indexes.push(row)
		}
		if (indexes.length === 0)
			return
		for (let k = indexes.length - 1; k >= 0; --k)
			childs.remove(indexes[k])
		notifyListChanged(childs)
		selectedFeature = null
		rebuildFeatureTree(null)
		updateActiveFeature()
		__updateCommandStates()
	}

	function moveFeatureNode(node, oldParentNode, newParentNode) {
		let item = sourceItem(node)
		let oldModel = childModel(oldParentNode)
		let newModel = childModel(newParentNode)
		if (!canEdit || !item || !oldModel || !newModel || oldModel === newModel)
			return
		if (siblingIdConflict(newParentNode, item.m_featureId, null))
			return
		let movedItem = item.copyMe()
		if (!movedItem || !removeItem(oldModel, item))
			return
		// addElement() announces the arrival; the departure went through the
		// silent remove(), so the level it left is announced here.
		notifyListChanged(oldModel)
		newModel.addElement(movedItem)
		selectedFeature = null
		rebuildFeatureTree(movedItem)
		updateActiveFeature()
	}

	function selectFeatureNode(node) {
		selectedFeature = sourceItem(node)
		updateActiveFeature()
		__updateCommandStates()
	}

	// Requirements belong to the row the user picked in the Subfeatures table;
	// with nothing selected the requirements panel has no subject and stays hidden.
	function updateActiveFeature() {
		activeFeature = selectedFeature
	}

	function activeFeaturePath() {
		return featurePath(activeFeature)
	}

	function rebuildRequirementEntries() {
		if (!featureCollectionModel)
			return
		let tree = TreeModelBuilder.fromTreeItemModelByFields(featureCollectionModel, {
			key: FeatureItemTypeMetaInfo.s_featureId,
			children: FeatureItemTypeMetaInfo.s_subFeatures,
			columns: {
				featureName: FeatureItemTypeMetaInfo.s_featureName,
				featureId: FeatureItemTypeMetaInfo.s_featureId,
				optional: FeatureItemTypeMetaInfo.s_optional,
				requirements: FeatureItemTypeMetaInfo.s_requirements
			}
		})
		let graph = []
		appendRequirementGraphNodes(tree, "", graph)
		requirementGraph = graph

		let leaves = []
		appendRequirementEntries(tree, [], "", "", "", leaves)
		requirementEntries = leaves
		rebuildMandatoryLeafCounts()
	}

	// How many parts of each feature are not optional. A product takes a feature
	// whole, so requiring any part of one takes all of these along - that is what
	// the picker warns about before a requirement is written.
	property var mandatoryLeafCounts: ({})

	function rebuildMandatoryLeafCounts() {
		let counts = ({})
		for (let i = 0; i < requirementEntries.length; ++i) {
			let entry = requirementEntries[i]
			if (entry.optional || entry.rootPath === entry.featurePath)
				continue
			counts[entry.rootPath] = (counts[entry.rootPath] ? counts[entry.rootPath] : 0) + 1
		}
		mandatoryLeafCounts = counts
	}

	function requirementEntryOf(featurePathValue) {
		for (let i = 0; i < requirementEntries.length; ++i) {
			if (requirementEntries[i].featurePath === featurePathValue)
				return requirementEntries[i]
		}
		return null
	}

	// What else a product has to swallow to satisfy this requirement: the parts
	// of the same feature that are not optional and so cannot be left out.
	function requirementBringsAlong(featurePathValue) {
		let entry = requirementEntryOf(featurePathValue)
		if (!entry || entry.rootPath === entry.featurePath)
			return 0
		let mandatory = mandatoryLeafCounts[entry.rootPath] ? mandatoryLeafCounts[entry.rootPath] : 0
		return entry.optional ? mandatory : mandatory - 1
	}

	function appendRequirementGraphNodes(nodes, parentPath, target) {
		for (let i = 0; i < nodes.length; ++i) {
			let node = nodes[i]
			let data = node.data || {}
			let id = data[FeatureItemTypeMetaInfo.s_featureId] || ""
			if (id === "")
				continue
			let path = parentPath + "/" + id
			target.push({
				"featurePath": path,
				"requirements": data[FeatureItemTypeMetaInfo.s_requirements] || ""
			})
			if (node.children && node.children.length > 0)
				appendRequirementGraphNodes(node.children, path, target)
		}
	}

	// Flattens the shared feature collection into one flat row per leaf feature,
	// each carrying the full path it sits at - a plain list reads better here
	// than a tree, because a requirement is picked by what it is, not by where.
	function appendRequirementEntries(nodes, pathNames, parentPath, rootPath, rootName, target) {
		for (let i = 0; i < nodes.length; ++i) {
			let node = nodes[i]
			let data = node.data || {}
			let id = data[FeatureItemTypeMetaInfo.s_featureId] || ""
			if (id === "")
				continue
			let name = data[FeatureItemTypeMetaInfo.s_featureName] || id
			let currentNames = pathNames.concat([name])
			let path = parentPath + "/" + id
			// The feature a part belongs to, which is what a product takes.
			let currentRootPath = rootPath === "" ? path : rootPath
			let currentRootName = rootName === "" ? name : rootName

			if (node.children && node.children.length > 0) {
				appendRequirementEntries(node.children, currentNames, path, currentRootPath, currentRootName, target)
				continue
			}
			target.push({
				"featureName": name,
				"fullPath": currentNames.join(" / "),
				"featurePath": path,
				"optional": data[FeatureItemTypeMetaInfo.s_optional] === true,
				"rootPath": currentRootPath,
				"rootName": currentRootName
			})
		}
	}

	function requirementName(featurePathValue) {
		for (let i = 0; i < requirementEntries.length; ++i) {
			if (requirementEntries[i].featurePath === featurePathValue)
				return requirementEntries[i].featureName
		}
		return featurePathValue
	}

	// Preview for the table's Requirements column: the first names plus "+N",
	// which says what a feature pulls in without opening the panel.
	function requirementSummary(item) {
		if (!item || !item.m_requirements)
			return ""
		let paths = item.m_requirements.split(';')
		let names = []
		for (let i = 0; i < paths.length && i < 2; ++i)
			names.push(requirementName(paths[i]))
		if (paths.length > 2)
			return names.join(", ") + "  +" + (paths.length - 2)
		return names.join(", ")
	}

	// Feature-ID is derived from the name: keep letters, digits and underscores.
	function sanitizeId(text) {
		return text ? text.replace(/[^A-Za-z0-9_]/g, '') : ""
	}

	// Lines for the "N incomplete" hover panel: what is missing, per row.
	function incompleteDetails(nodes) {
		let details = []
		let list = nodes || []
		for (let i = 0; i < list.length; ++i) {
			let item = sourceItem(list[i])
			if (!item || itemIsComplete(item))
				continue
			if (item.m_featureName === "" && item.m_featureId === "")
				details.push(qsTr("Unnamed row - name and Feature ID are missing"))
			else if (item.m_featureName === "")
				details.push(qsTr("%1 - name is missing").arg(item.m_featureId))
			else
				details.push(qsTr("%1 - Feature ID is missing").arg(item.m_featureName))
		}
		return details
	}

	// A feature that owns sub-features is a grouping, not a capability: what it
	// grants is whatever its children grant. Requirements, Optional and
	// Permission therefore only apply to the leaves of the tree.
	function isLeafNode(node) {
		return node !== null && node !== undefined
			&& (!node.children || node.children.length === 0)
	}

	function subFeatureCount(node) {
		return node && node.children ? node.children.length : 0
	}

	function directRequirements() {
		if (!activeFeature || !activeFeature.m_requirements)
			return []
		return activeFeature.m_requirements.split(';')
	}

	function isDirectRequirement(featurePathValue) {
		return directRequirements().indexOf(featurePathValue) >= 0
	}

	function requirementCount(item) {
		if (!item || !item.m_requirements)
			return 0
		return item.m_requirements.split(';').length
	}

	// An ancestor is spelled out by the path itself: everything the active
	// feature hangs below is a prefix of its own path.
	function isAncestorPath(featurePathValue) {
		let path = activeFeaturePath()
		return path !== "" && featurePathValue !== "" && path.indexOf(featurePathValue + "/") === 0
	}

	function requirementEntry(featurePathValue) {
		for (let i = 0; i < requirementGraph.length; ++i) {
			if (requirementGraph[i].featurePath === featurePathValue)
				return requirementGraph[i]
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
		let path = activeFeaturePath()
		if (!canEdit || path === "" || featurePathValue === path)
			return false
		if (isAncestorPath(featurePathValue))
			return false
		return !requirementReaches(featurePathValue, path, {})
	}

	// Chip next to a requirement row: why the entry cannot be picked.
	function requirementBadge(featurePathValue) {
		let path = activeFeaturePath()
		if (path === "")
			return ""
		if (featurePathValue === path)
			return qsTr("Itself")
		if (isAncestorPath(featurePathValue))
			return qsTr("Parent")
		if (requirementReaches(featurePathValue, path, {}))
			return qsTr("Cycle")
		// A product takes a feature whole. Requiring a part of one therefore
		// takes its mandatory siblings along, whether they are wanted or not -
		// said here, where the requirement is written, rather than discovered
		// later in a product that grew by itself.
		let bringsAlong = requirementBringsAlong(featurePathValue)
		if (bringsAlong > 0) {
			let entry = requirementEntryOf(featurePathValue)
			return qsTr("Brings all of %1").arg(entry ? entry.rootName : "")
		}
		return ""
	}

	function toggleRequirement(featurePathValue) {
		if (!requirementIsEnabled(featurePathValue))
			return
		let paths = directRequirements()
		let index = paths.indexOf(featurePathValue)
		if (index >= 0)
			paths.splice(index, 1)
		else
			paths.push(featurePathValue)
		activeFeature.m_requirements = paths.join(';')
	}

	function clearRequirements() {
		if (!canEdit || !activeFeature)
			return
		activeFeature.m_requirements = ""
	}

	function updateGui() {
		if (!featureData)
			return
		var generalPageInstance = multiPageView.getPageByIndex(0)
		if (generalPageInstance)
			generalPageInstance.updateGui()
		rebuildFeatureTree(selectedFeature)
		updateActiveFeature()
	}

	function updateModel() {
		if (!featureData)
			return
		var generalPageInstance = multiPageView.getPageByIndex(0)
		if (generalPageInstance)
			generalPageInstance.updateModel()
	}

	commandsDelegateComp: Component {
		ViewCommandsDelegateBase {
			view: featureEditor
			onCommandActivated: {
				let page = multiPageView.getPageById("Subfeatures")
				if (!page)
					return
				if (commandId === "InsertFeature")
					featureEditor.createSubFeature(page.currentParentNode())
				else if (commandId === "RemoveFeature")
						featureEditor.removeFeatureNodes(page.commandTargets(), page.currentParentNode())
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
			multiPageView.addPage("Subfeatures", qsTr("Sub-features"), subfeaturesPageComp, "Icons/FeaturePackage")
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
							id: featureNameInput
							// Both of these must be filled in before the feature can be
							// given sub-features, so the page says so where they are
							// rather than leaving the reader to infer it from a command
							// that never lights up.
							name: qsTr("Feature Name") + " *"
							description: featureNameInput.text === "" ? qsTr("Required. A feature needs a name before it can have sub-features.") : ""
							descriptionColor: Style.errorTextColor
							placeHolderText: qsTr("Enter the feature name")
							readOnly: !featureEditor.canEdit
							onEditingFinished: {
								if (featureIdInput.text === "")
									featureIdInput.text = text.replace(/\s+/g, '')
								featureEditor.doUpdateModel()
							}
							KeyNavigation.tab: featureIdInput
						}

						TextInputElementView {
							id: featureIdInput
							name: qsTr("Feature ID") + " *"
							description: featureIdInput.text === "" ? qsTr("Required. Filled in from the name if you leave it empty.") : ""
							descriptionColor: Style.errorTextColor
							placeHolderText: qsTr("Enter the feature ID")
							readOnly: !featureEditor.canEdit
							onEditingFinished: featureEditor.doUpdateModel()
							KeyNavigation.tab: descriptionInput
							KeyNavigation.backtab: featureNameInput
						}

						TextInputElementView {
							id: descriptionInput
							name: qsTr("Description")
							placeHolderText: qsTr("Enter the description")
							readOnly: !featureEditor.canEdit
							onEditingFinished: featureEditor.doUpdateModel()
							KeyNavigation.backtab: featureIdInput
						}

						SwitchElementView {
							id: optionalSwitch
							name: qsTr("Optional")
							readOnly: !featureEditor.canEdit
							onCheckedChanged: featureEditor.doUpdateModel()
						}

						SwitchElementView {
							id: permissionSwitch
							name: qsTr("Permission")
							readOnly: !featureEditor.canEdit
							onCheckedChanged: featureEditor.doUpdateModel()
						}

						// The identifier alone does not say which branch it belongs to;
						// requirements and permissions are written as the whole path, so
						// the page shows it and hands it over in one click.
						ElementView {
							id: fullPathElement
							width: parent.width
							name: qsTr("Full path")
							description: qsTr("How this feature is addressed by requirements and permissions")

							controlComp: Component {
								Item {
									id: fullPathControl
									// A Row here would size to the whole path and overflow past the
									// panel instead of shrinking, so the control gets a hard cap and
									// the text elides inside it like every other path field does.
									width: Math.min(implicitWidth, fullPathElement.controlWidth)
									implicitWidth: fullPathText.implicitWidth + Style.spacingS + copyFullPathButton.width
									height: Style.controlHeightM
									clip: true

									ToolButton {
										id: copyFullPathButton
										anchors.right: parent.right
										anchors.verticalCenter: parent.verticalCenter
										width: Style.buttonWidthM
										height: width
										enabled: featureEditor.rootFeaturePath !== ""
										iconSource: featureEditor.copyIconSource(featureEditor.rootFeaturePath)
										tooltipText: qsTr("Copy the full path")
										onClicked: featureEditor.copyFeaturePath(featureEditor.featureData)
									}

									BaseText {
										id: fullPathText
										anchors.left: parent.left
										anchors.verticalCenter: parent.verticalCenter
										width: Math.max(0, parent.width - copyFullPathButton.width - Style.spacingS)
										clip: true
										elide: Text.ElideLeft
										text: featureEditor.rootFeaturePath !== "" ? featureEditor.rootFeaturePath : "-"
										font.family: Style.fontFamilyBold
										color: featureEditor.rootFeaturePath !== "" ? Style.textColor : Style.inactiveTextColor
									}
								}
							}
						}

						function updateGui() {
							if (!featureEditor.featureData)
								return
							featureNameInput.text = featureEditor.featureData.m_featureName || ""
							featureIdInput.text = featureEditor.featureData.m_featureId || ""
							descriptionInput.text = featureEditor.featureData.m_description || ""
							optionalSwitch.setChecked(featureEditor.featureData.m_optional)
							permissionSwitch.setChecked(featureEditor.featureData.m_isPermission)
						}

						function updateModel() {
							if (!featureEditor.featureData)
								return
							// A blank name is left as it was rather than saved - the field
							// itself keeps showing the rejected text and its red "Required"
							// hint, so nothing here needs to say it twice.
							if (featureNameInput.text !== "")
								featureEditor.featureData.m_featureName = featureNameInput.text
							featureEditor.featureData.m_featureId = featureIdInput.text
							featureEditor.featureData.m_description = descriptionInput.text
							featureEditor.featureData.m_optional = optionalSwitch.checked
							featureEditor.featureData.m_isPermission = permissionSwitch.checked
							featureEditor.rebuildFeatureTree(featureEditor.selectedFeature)
						}
					}
				}
			}
		}
	}

	Component {
		id: subfeaturesPageComp

		Item {
			id: subfeaturesPage
			anchors.fill: parent

			// What the hovered Sub-features chip is about to preview. The popup
			// lives here rather than in the row because a row is clipped by the
			// table it scrolls in.
			property var previewNode: null
			property real previewX: 0
			property real previewY: 0

			function showPreview(node, item) {
				if (!node || !node.children || node.children.length === 0) {
					// Closes whatever the chip before it had opened, instead of
					// leaving that popup hanging over an unrelated row.
					subfeaturesPage.previewNode = null
					return
				}
				let point = item.mapToItem(subfeaturesPage, item.width / 2, item.height + Style.marginXS)
				subfeaturesPage.previewX = point.x
				subfeaturesPage.previewY = point.y
				subfeaturesPage.previewNode = node
			}

			function hidePreview() {
				subfeaturesPage.previewNode = null
			}

			// At most this many names; the rest are summarised on the last line.
			function previewNames(node) {
				let names = []
				let children = node && node.children ? node.children : []
				for (let i = 0; i < children.length && i < 8; ++i)
					names.push(featureEditor.featureName(children[i]))
				return names
			}

			function previewOverflow(node) {
				let total = node && node.children ? node.children.length : 0
				return total > 8 ? total - 8 : 0
			}

			function updateGui() {
				featureEditor.rebuildFeatureTree(featureEditor.selectedFeature)
				featureEditor.updateActiveFeature()
			}

			function updateModel() {
			}

			// Forwarders: featureEditor's root-level functions and commandsDelegateComp
			// cannot reference "treeExplorer" directly (it belongs to this Component's own
			// id scope), so they reach it via multiPageView.getPageById("Subfeatures").
			property alias selectedNode: treeExplorer.selectedNode
			property alias navigationStack: treeExplorer.navigationStack
			function selectNode(node) { treeExplorer.selectNode(node) }
			function currentParentNode() { return treeExplorer.currentParentNode() }
				function commandTargets() { return treeExplorer.commandTargets() }

			// Column geometry, shared by the header and the rows so the two can
			// never drift. A column whose breakpoint is above the current table
			// width folds away and its share is handed to the columns that stay.
			property var columnFractions: [0.20, 0.13, 0.16, 0.07, 0.08, 0.08, 0.16, 0.12]
			property var columnBreakpoints: [0, 340, 720, 0, 540, 440, 620, 0]

			function columnVisible(index, width) {
				return width >= subfeaturesPage.columnBreakpoints[index]
			}

			function columnWidth(index, width, spacing) {
				if (!subfeaturesPage.columnVisible(index, width))
					return 0
				let sum = 0
				let count = 0
				for (let i = 0; i < subfeaturesPage.columnFractions.length; ++i) {
					if (!subfeaturesPage.columnVisible(i, width))
						continue
					sum += subfeaturesPage.columnFractions[i]
					++count
				}
				return (width - (count - 1) * spacing) * subfeaturesPage.columnFractions[index] / sum
			}

			property Component subfeaturesHeaderComp: Component {
				Row {
					id: headerRow
					anchors.fill: parent
					spacing: Style.spacingM

					Item {
						width: subfeaturesPage.columnWidth(0, headerRow.width, headerRow.spacing)
						height: headerRow.height
						visible: width > 0
						BaseText {
							anchors.left: parent.left
							anchors.leftMargin: Style.marginXS
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							text: qsTr("Name")
							font.family: Style.fontFamilyBold
							font.pixelSize: Style.fontSizeS
							color: Style.subtitleColor
						}
					}
					Item {
						width: subfeaturesPage.columnWidth(1, headerRow.width, headerRow.spacing)
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
						width: subfeaturesPage.columnWidth(2, headerRow.width, headerRow.spacing)
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
						width: subfeaturesPage.columnWidth(3, headerRow.width, headerRow.spacing)
						height: headerRow.height
						visible: width > 0
						BaseText {
							anchors.centerIn: parent
							width: parent.width
							horizontalAlignment: Text.AlignHCenter
							text: qsTr("Optional")
							font.family: Style.fontFamilyBold
							font.pixelSize: Style.fontSizeS
							color: Style.subtitleColor
						}
					}
					Item {
						width: subfeaturesPage.columnWidth(4, headerRow.width, headerRow.spacing)
						height: headerRow.height
						visible: width > 0
						BaseText {
							anchors.centerIn: parent
							width: parent.width
							horizontalAlignment: Text.AlignHCenter
							text: qsTr("Permission")
							font.family: Style.fontFamilyBold
							font.pixelSize: Style.fontSizeS
							color: Style.subtitleColor
						}
					}
					Item {
						width: subfeaturesPage.columnWidth(5, headerRow.width, headerRow.spacing)
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
						width: subfeaturesPage.columnWidth(6, headerRow.width, headerRow.spacing)
						height: headerRow.height
						visible: width > 0
						BaseText {
							anchors.left: parent.left
							anchors.leftMargin: Style.marginXS
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							text: qsTr("Requirements")
							font.family: Style.fontFamilyBold
							font.pixelSize: Style.fontSizeS
							color: Style.subtitleColor
						}
					}
					Item {
						width: subfeaturesPage.columnWidth(7, headerRow.width, headerRow.spacing)
						height: headerRow.height
						visible: width > 0
						BaseText {
							anchors.left: parent.left
							anchors.leftMargin: Style.marginXS
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							text: qsTr("Path")
							font.family: Style.fontFamilyBold
							font.pixelSize: Style.fontSizeS
							color: Style.subtitleColor
						}
					}
				}
			}

			// The row is plain text until treeExplorer puts it into edit mode, so a
			// click always reaches the row and only ever moves the selection. In edit
			// mode the three text cells become fields and Tab walks them; Tab off
			// either end carries editing into the neighbouring row.
			//
			// The two count cells stay live in both modes: the subfeature chip is how
			// you descend a level, the requirements preview selects the row so the panel
			// on the right shows the full list.
			property Component subfeatureRowComp: Component {
				Row {
					id: rowContent
					anchors.fill: parent
					spacing: Style.spacingM

					property var node: parent ? parent.node : null
					property bool editing: parent ? parent.editing : false
					property int editColumn: parent ? parent.editColumn : 0
					property int commitRequest: parent ? parent.commitRequest : 0
					property var sourceItem: rowContent.node ? featureEditor.sourceItem(rowContent.node) : null
					property bool isLeaf: featureEditor.isLeafNode(rowContent.node)

					// The row drives itself off the state above rather than being
					// called into: the view has no reliable handle on a delegate.
					onCommitRequestChanged: {
						if (rowContent.editing)
							rowContent.commitEditing()
					}

					function focusColumn(index) {
						if (index <= 0)
							nameCell.focusEditor()
						else if (index === 1)
							idCell.focusEditor()
						else
							descriptionCell.focusEditor()
					}

					// The text field of a cell only exists once the row is open, so
					// the caret is placed a turn later.
					Timer {
						id: focusTimer
						interval: 0
						onTriggered: rowContent.focusColumn(rowContent.editColumn)
					}


					// Drafts of the two switches. Like the text cells, they stay off
					// the model until the row is committed.
					property bool draftOptional: false
					property bool draftPermission: false
					// The identifier is derived from the name only for a row that
					// opened without one; an ID that already existed is the user's,
					// and generatedId records what was last derived so that typing
					// over it stops the derivation too.
					property bool deriveId: false
					property string generatedId: ""

					// Also on completion, because a row created by New sub-feature is
					// already in edit mode by the time its delegate is built.
					Component.onCompleted: {
						rowContent.enterEditing()
					}

					onEditingChanged: rowContent.enterEditing()

					function enterEditing() {
						rowContent.loadDrafts()
						if (rowContent.editing)
							focusTimer.restart()
					}

					onSourceItemChanged: rowContent.loadDrafts()

					function loadDrafts() {
						if (rowContent.editing && rowContent.sourceItem) {
							rowContent.draftOptional = rowContent.sourceItem.m_optional === true
							rowContent.draftPermission = rowContent.sourceItem.m_isPermission === true
							rowContent.generatedId = rowContent.sourceItem.m_featureId
							rowContent.deriveId = rowContent.sourceItem.m_featureId === ""
						}
						rowContent.syncSwitches()
					}

					function syncSwitches() {
						let item = rowContent.sourceItem
						let optional = rowContent.editing ? rowContent.draftOptional
							: item !== null && item.m_optional === true
						let permission = rowContent.editing ? rowContent.draftPermission
							: item !== null && item.m_isPermission === true
						optionalSwitch.setChecked(optional, false)
						permissionSwitch.setChecked(permission, false)
					}

					// Everything the row changed lands in the model between one
					// beginChanges/endChanges pair, so it is a single undo step.
					function commitEditing() {
						let item = rowContent.sourceItem
						if (!item || !featureEditor.canEdit)
							return
						// Grouped when the data element supports it; without the
						// guard a model that has no transactions would throw here
						// and the row would silently write nothing at all.
						let grouped = typeof item.beginChanges === "function"
						if (grouped)
							item.beginChanges()
						// A blank name is left as it was rather than saved, same as a
						// duplicate id below.
						let newFeatureName = nameCell.pendingValue()
						if (newFeatureName !== "")
							item.m_featureName = newFeatureName
						// A sibling using this id already is left as-is rather than
						// applied: the id would collide on save (same parent path).
						let newFeatureId = idCell.pendingValue()
						if (!featureEditor.siblingIdConflict(treeExplorer.currentParentNode(), newFeatureId, item))
							item.m_featureId = newFeatureId
						item.m_description = descriptionCell.pendingValue()
						if (rowContent.isLeaf) {
							item.m_optional = rowContent.draftOptional
							item.m_isPermission = rowContent.draftPermission
						}
						if (grouped)
							item.endChanges()
					}

					// Escape belongs to the whole row: the drafts are simply dropped.
					function cancelEditing() {
						treeExplorer.cancelEditRow()
					}

					EditableTableCell {
						id: nameCell
						width: subfeaturesPage.columnWidth(0, rowContent.width, rowContent.spacing)
						height: Style.controlHeightM
						anchors.verticalCenter: parent.verticalCenter
						visible: width > 0
						bold: true
						required: true
						editing: rowContent.editing
						readOnly: !featureEditor.canEdit
						placeHolderText: qsTr("Feature name")
						value: rowContent.sourceItem ? rowContent.sourceItem.m_featureName : ""
						nextEditor: idCell.editorItem
						previousEditor: previousRowProxy
						onEditingEndRequested: treeExplorer.commitEditRow()
						onEditingCancelRequested: rowContent.cancelEditing()
						// The identifier follows the name until the user types one of
						// their own, and then stops interfering.
						onDraftChanged: {
							if (!rowContent.editing || !rowContent.deriveId)
								return
							if (idCell.draft !== "" && idCell.draft !== rowContent.generatedId) {
								rowContent.deriveId = false
								return
							}
							rowContent.generatedId = featureEditor.sanitizeId(nameCell.draft)
							idCell.setDraft(rowContent.generatedId)
						}
					}

					EditableTableCell {
						id: idCell
						width: subfeaturesPage.columnWidth(1, rowContent.width, rowContent.spacing)
						height: Style.controlHeightM
						anchors.verticalCenter: parent.verticalCenter
						visible: width > 0
						required: true
						editing: rowContent.editing
						readOnly: !featureEditor.canEdit
						placeHolderText: qsTr("Identifier")
						value: rowContent.sourceItem ? rowContent.sourceItem.m_featureId : ""
						// Flags a sibling with the same id while typing; commitEditing()
						// is what actually keeps the duplicate out of the model.
						invalid: rowContent.editing && featureEditor.siblingIdConflict(
								treeExplorer.currentParentNode(), idCell.draft, rowContent.sourceItem)
						nextEditor: descriptionCell.editorItem
						previousEditor: nameCell.editorItem
						onEditingEndRequested: treeExplorer.commitEditRow()
						onEditingCancelRequested: rowContent.cancelEditing()
					}

					EditableTableCell {
						id: descriptionCell
						width: subfeaturesPage.columnWidth(2, rowContent.width, rowContent.spacing)
						height: Style.controlHeightM
						anchors.verticalCenter: parent.verticalCenter
						visible: width > 0
						editing: rowContent.editing
						readOnly: !featureEditor.canEdit
						placeHolderText: qsTr("Description")
						value: rowContent.sourceItem ? rowContent.sourceItem.m_description : ""
						emptyText: qsTr("No description")
						nextEditor: nextRowProxy
						previousEditor: idCell.editorItem
						onEditingEndRequested: treeExplorer.commitEditRow()
						onEditingCancelRequested: rowContent.cancelEditing()
					}

					// SwitchCustom drops any binding on "checked" the moment it is
					// toggled (its animation assigns the property), so both switches
					// are driven by hand from syncSwitches() instead.
					Item {
						id: optionalCell
						width: subfeaturesPage.columnWidth(3, rowContent.width, rowContent.spacing)
						height: Style.controlHeightM
						anchors.verticalCenter: parent.verticalCenter
						visible: width > 0

						SwitchCustom {
							id: optionalSwitch
							anchors.centerIn: parent
							height: Style.controlHeightS
							visible: rowContent.isLeaf
							readOnly: !featureEditor.canEdit || !rowContent.editing
							onCheckedChanged: {
								if (rowContent.editing)
									rowContent.draftOptional = optionalSwitch.checked
							}
						}

						BaseText {
							anchors.centerIn: parent
							visible: !rowContent.isLeaf
							text: "-"
							color: Style.inactiveTextColor
						}
					}

					Item {
						id: permissionCell
						width: subfeaturesPage.columnWidth(4, rowContent.width, rowContent.spacing)
						height: Style.controlHeightM
						anchors.verticalCenter: parent.verticalCenter
						visible: width > 0

						SwitchCustom {
							id: permissionSwitch
							anchors.centerIn: parent
							height: Style.controlHeightS
							visible: rowContent.isLeaf
							readOnly: !featureEditor.canEdit || !rowContent.editing
							onCheckedChanged: {
								if (rowContent.editing)
									rowContent.draftPermission = permissionSwitch.checked
							}
						}

						BaseText {
							anchors.centerIn: parent
							visible: !rowContent.isLeaf
							text: "-"
							color: Style.inactiveTextColor
						}
					}

					// Count chip: how many subfeatures this one has, and the way down
					// into them - which is why there is no separate open button. It
					// turns into a warning when something below is still unfinished,
					// and goes dead when the feature itself has no name or ID, since
					// an unnamed feature cannot own anything.
					Item {
						id: subfeaturesCell
						width: subfeaturesPage.columnWidth(5, rowContent.width, rowContent.spacing)
						height: Style.controlHeightM
						anchors.verticalCenter: parent.verticalCenter
						visible: width > 0

						property int count: featureEditor.subFeatureCount(rowContent.node)
						property bool complete: featureEditor.nodeIsComplete(rowContent.node)
						property bool hasIssues: featureEditor.subtreeHasIssues(rowContent.node)
						property bool openable: subfeaturesCell.complete && !rowContent.editing

						Rectangle {
							anchors.centerIn: parent
							width: Math.max(Style.controlHeightS,
								countText.width + warningIcon.width + 2 * Style.marginS)
							height: Style.controlHeightS
							radius: height / 2
							color: subfeaturesMouse.containsMouse ? Style.titleColor
								: subfeaturesCell.count > 0 ? Style.backgroundColor2 : "transparent"
							border.color: subfeaturesCell.hasIssues ? Style.errorTextColor
								: subfeaturesMouse.containsMouse || subfeaturesCell.count > 0
									? Style.borderColor : "transparent"
							border.width: 1

							Row {
								anchors.centerIn: parent
								spacing: Style.spacingXS

								Image {
									id: warningIcon
									anchors.verticalCenter: parent.verticalCenter
									visible: subfeaturesCell.hasIssues
									width: visible ? Style.iconSizeXS : 0
									height: Style.iconSizeXS
									source: "qrc:/" + Style.getIconPath("Icons/Warning", Icon.State.On, Icon.Mode.Normal)
								}

								BaseText {
									id: countText
									anchors.verticalCenter: parent.verticalCenter
									text: subfeaturesCell.count > 0 ? subfeaturesCell.count : "-"
									font.pixelSize: Style.fontSizeS
									font.family: Style.fontFamilyBold
									color: subfeaturesMouse.containsMouse ? Style.baseColor
										: subfeaturesCell.hasIssues ? Style.errorTextColor
										: subfeaturesCell.count > 0 ? Style.titleColor : Style.inactiveTextColor
								}
							}
						}

						MouseArea {
							id: subfeaturesMouse
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: subfeaturesCell.openable ? Qt.PointingHandCursor : Qt.ArrowCursor
							onEntered: subfeaturesPage.showPreview(rowContent.node, subfeaturesCell)
							onExited: subfeaturesPage.hidePreview()
							onClicked: {
								if (subfeaturesCell.openable)
									treeExplorer.navigateInto(rowContent.node)
							}
						}
					}

					Item {
						id: requirementsCell
						width: subfeaturesPage.columnWidth(6, rowContent.width, rowContent.spacing)
						height: Style.controlHeightM
						anchors.verticalCenter: parent.verticalCenter
						visible: width > 0

						property string summary: featureEditor.requirementSummary(rowContent.sourceItem)

						BaseText {
							anchors.left: parent.left
							anchors.leftMargin: Style.marginXS
							anchors.right: parent.right
							anchors.rightMargin: Style.marginXS
							anchors.verticalCenter: parent.verticalCenter
							text: !rowContent.isLeaf ? qsTr("Set on the sub-features")
								: requirementsCell.summary !== "" ? requirementsCell.summary : "-"
							font.italic: !rowContent.isLeaf
							color: requirementsCell.summary !== "" && rowContent.isLeaf
								? Style.textColor : Style.inactiveTextColor
							elide: Text.ElideRight
						}

					}

					// The path is how this feature is written down elsewhere - in a
					// requirement, in a permission list, in a configuration file - so it
					// is offered where the row is rather than only in the panel above.
					Item {
						id: pathCell
						width: subfeaturesPage.columnWidth(7, rowContent.width, rowContent.spacing)
						height: Style.controlHeightM
						anchors.verticalCenter: parent.verticalCenter
						visible: width > 0
						// Neither child ever paints past this box, however tight the column gets.
						clip: true

						property string path: featureEditor.featurePath(rowContent.sourceItem)
						property bool copyable: pathCell.path !== "" && !rowContent.editing
						// Below this the cell only has room for the path itself.
						property bool buttonFits: pathCell.width >= Style.buttonWidthS + 3 * Style.marginXS

						ToolButton {
							id: copyPathButton
							anchors.right: parent.right
							anchors.rightMargin: Style.marginXS
							anchors.verticalCenter: parent.verticalCenter
							width: Style.buttonWidthS
							height: width
							visible: pathCell.buttonFits
							enabled: pathCell.copyable
							iconSource: featureEditor.copyIconSource(pathCell.path)
							tooltipText: pathCell.copyable ? qsTr("Copy %1").arg(pathCell.path) : ""
							onClicked: featureEditor.copyFeaturePath(rowContent.sourceItem)
						}

						BaseText {
							anchors.left: parent.left
							anchors.leftMargin: Style.marginXS
							anchors.verticalCenter: parent.verticalCenter
							// Computed instead of anchored to the button's edge, so the two
							// can never share a pixel even while the button is toggling away.
							width: Math.max(0, pathCell.width - Style.marginXS - (pathCell.buttonFits
								? Style.buttonWidthS + 2 * Style.marginXS : Style.marginXS))
							clip: true
							// The tail tells the features apart, so drop the shared prefix first.
							elide: Text.ElideLeft
							text: pathCell.path !== "" ? pathCell.path : "-"
							font.pixelSize: Style.fontSizeXS
							color: pathCell.path !== "" ? Style.subtitleColor : Style.inactiveTextColor
						}
					}

					// Zero-size focus relays: Tab off the last cell or Shift+Tab off the
					// first lands here and hands editing to the adjacent row.
					Item {
						id: previousRowProxy
						width: 0
						height: 0
						onActiveFocusChanged: {
							if (activeFocus)
								treeExplorer.moveEditRow(rowContent.node, -1, 2)
						}
					}
					Item {
						id: nextRowProxy
						width: 0
						height: 0
						onActiveFocusChanged: {
							if (activeFocus)
								treeExplorer.moveEditRow(rowContent.node, 1, 0)
						}
					}
				}
			}

			// Right-hand table of the page: a flat list of every feature that can be
			// required, each with its full path. Entries the selected subfeature
			// must not require (itself, an ancestor, or anything that would close a
			// cycle) stay disabled and carry a chip saying why.
			property Component requirementsPanelComp: Component {
				CheckableListPanel {
					title: qsTr("Requirements")
					subtitle: featureEditor.activeFeature ? featureEditor.activeFeature.m_featureName : ""
					model: featureEditor.requirementEntries
					// Requirements belong to exactly one feature, and only to one
					// that has no sub-features of its own: a grouping grants what
					// its children grant, so its own requirements mean nothing.
					contentActive: featureEditor.activeFeature !== null
						&& treeExplorer.checkedNodes.length < 2
						&& featureEditor.isLeafNode(treeExplorer.selectedNode)
					revealKey: featureEditor.activeFeaturePath()
					placeholderText: treeExplorer.checkedNodes.length > 1
						? qsTr("More than one row is ticked")
						: featureEditor.activeFeature !== null
							? qsTr("This feature has sub-features") : qsTr("No sub-feature selected")
					placeholderDescription: treeExplorer.checkedNodes.length > 1
						? qsTr("Requirements belong to a single feature - untick the rest to edit them")
						: featureEditor.activeFeature !== null
							? qsTr("A feature that groups others grants whatever they grant, so it carries no requirements of its own. Open it and set them on the sub-features inside.")
							: qsTr("Pick a row on the left to see and edit what it requires")
					nameColumnTitle: qsTr("Feature")
					iconSource: "Icons/Dependencies"
					searchPlaceholder: qsTr("Search features")
					emptyText: qsTr("No features to require")
					emptyDescription: qsTr("Requirements are picked from the shared feature collection")
					actionText: qsTr("Clear")
					actionEnabled: featureEditor.canEdit && featureEditor.directRequirements().length > 0
					entryTitleProvider: function(entry) { return entry.featureName }
					entrySubtitleProvider: function(entry) { return entry.featurePath }
					entryBadgeProvider: function(entry) { return featureEditor.requirementBadge(entry.featurePath) }
					entrySearchableTextProvider: function(entry) { return entry.fullPath + " " + entry.featurePath }
					entryCheckStateProvider: function(entry) { return featureEditor.isDirectRequirement(entry.featurePath) ? Qt.Checked : Qt.Unchecked }
					entryCheckEnabledProvider: function(entry) { return featureEditor.requirementIsEnabled(entry.featurePath) }
					onToggleRequested: featureEditor.toggleRequirement(entry.featurePath)
					onActionRequested: featureEditor.clearRequirements()
				}
			}

			TreeExplorerView {
				id: treeExplorer
				anchors.fill: parent
				anchors.margins: Style.marginXL
				model: featureEditor.featureTree
				rootTitle: featureEditor.featureData && featureEditor.featureData.m_featureName
					? featureEditor.featureData.m_featureName : qsTr("Feature")
				createButtonText: qsTr("New sub-feature")
				emptyText: qsTr("No sub-features here")
				emptyDescription: featureEditor.levelAcceptsChildren(treeExplorer.navigationStack)
					? qsTr("Create a sub-feature to start this level")
					: qsTr("Give this feature a name and an ID before adding sub-features")
				editable: featureEditor.canEdit
				// One row at a time: a tick in front of every sub-feature reads as
				// a decision about the feature, which it is not.
				multiSelectEnabled: false
				// A blank feature owns nothing: no new children until it is filled in.
				createEnabled: featureEditor.levelAcceptsChildren(treeExplorer.navigationStack)
				levelStatusText: featureEditor.incompleteCount(treeExplorer.currentEntries) > 0
					? qsTr("%1 incomplete").arg(featureEditor.incompleteCount(treeExplorer.currentEntries)) : ""
				levelStatusDetails: featureEditor.incompleteDetails(treeExplorer.currentEntries)
				// Says what is in the way while it is, rather than only greying the
				// command out and leaving the reader to guess which page to visit.
				idleHintText: featureEditor.levelAcceptsChildren(treeExplorer.navigationStack)
					? qsTr("Select a sub-feature, or press New sub-feature to add one")
					: qsTr("Fill in Feature Name and Feature ID on the General page to add sub-features here")
				selectedHintText: featureEditor.isLeafNode(treeExplorer.selectedNode)
					? qsTr("Edit or F2 changes this row; the Sub-features chip opens the level below")
					: qsTr("This feature groups others - Optional, Permission and requirements are set on the sub-features inside it")
				renameVisible: false
				rowIconVisible: false
				headerContentComponent: subfeaturesPage.subfeaturesHeaderComp
				rowContentComponent: subfeaturesPage.subfeatureRowComp
				sidePanelComponent: subfeaturesPage.requirementsPanelComp
				// Read through to the live model item rather than the cached node, so
				// inline renames show up in the breadcrumb and in search right away.
				textProvider: function(node) { return featureEditor.featureName(node) }
				descriptionProvider: function(node) { return featureEditor.featureDescription(node) }
				secondaryTextProvider: function(node) { return featureEditor.featureIdOf(node) }
				// Only a feature that has a name and an ID can be descended into.
				containerProvider: function(node) { return featureEditor.nodeIsComplete(node) }
				onSelectedNodeChanged: featureEditor.selectFeatureNode(selectedNode)
				onNavigationStackChanged: featureEditor.updateActiveFeature()
				onCreateRequested: featureEditor.createSubFeature(parentNode)
				onRemoveNodesRequested: featureEditor.removeFeatureNodes(nodes, parentNode)
				onMoveRequested: featureEditor.moveFeatureNode(node, oldParentNode, newParentNode)
			}

			// Preview of what is one level down, without going there. Declared
			// after the explorer so it draws over the table.
			//
			// The shadow is a sibling of the panel, not a child of it: a
			// ShaderEffectSource that renders its own parent is a cycle, which
			// is what made the popup come up broken.
			DropShadow {
				anchors.fill: previewPopup
				z: previewPopup.z - 1
				visible: previewPopup.visible
				horizontalOffset: 3
				verticalOffset: 3
				radius: Style.radiusL
				samples: 17
				spread: 0
				color: Style.shadowColor
				source: previewPopup
			}

			Rectangle {
				id: previewPopup
				z: 200
				x: Math.max(Style.marginL, Math.min(subfeaturesPage.previewX - width / 2,
					subfeaturesPage.width - width - Style.marginL))
				// Flips above the chip when the row is near the bottom of the page.
				y: subfeaturesPage.previewY + height + Style.marginL < subfeaturesPage.height
					? subfeaturesPage.previewY
					: Math.max(Style.marginL, subfeaturesPage.previewY - height - Style.controlHeightM - Style.marginM)
				width: Style.sizeHintXXS
				height: previewColumn.height + 2 * Style.marginM
				visible: subfeaturesPage.previewNode !== null
				radius: Style.radiusM
				color: Style.baseColor
				border.color: Style.borderColor
				border.width: 1

				Column {
					id: previewColumn
					anchors.top: parent.top
					anchors.topMargin: Style.marginM
					anchors.left: parent.left
					anchors.leftMargin: Style.marginM
					anchors.right: parent.right
					anchors.rightMargin: Style.marginM
					spacing: Style.spacingXS

					BaseText {
						width: previewColumn.width
						text: qsTr("Sub-features")
						font.family: Style.fontFamilyBold
						font.pixelSize: Style.fontSizeS
						color: Style.subtitleColor
					}

					Repeater {
						model: subfeaturesPage.previewNames(subfeaturesPage.previewNode)

						delegate: BaseText {
							width: previewColumn.width
							text: modelData
							color: Style.textColor
							elide: Text.ElideRight
						}
					}

					BaseText {
						width: previewColumn.width
						visible: subfeaturesPage.previewOverflow(subfeaturesPage.previewNode) > 0
						text: qsTr("and %1 more").arg(subfeaturesPage.previewOverflow(subfeaturesPage.previewNode))
						font.pixelSize: Style.fontSizeS
						color: Style.subtitleColor
					}
				}
			}
		}
	}

	Connections {
		target: CachedFeatureCollection
		function onModelUpdated() {
			featureEditor.featureCollectionModel.copy(CachedFeatureCollection.collectionModel)
			featureEditor.rebuildRequirementEntries()
		}
	}
}

