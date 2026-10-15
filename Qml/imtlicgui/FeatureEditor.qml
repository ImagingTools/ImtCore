import QtQuick 2.12
import Qt5Compat.GraphicalEffects
import QtGraphicalEffects 1.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtauthgui 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import imtlicFeaturesSdl 1.0

ViewBase {
	id: featureEditor
	contentColor: Style.baseColor

	property TreeItemModel featureCollectionModel: CachedFeatureCollection.collectionModel
	property string featureId: ""
	property FeatureData featureData: model
	property var featureTree: []
	property var requirementEntries: []
	property var rootRequirementEntries: []
	property var subfeatureRequirementEntries: []
	property var activeRequirementEntries: subfeatureRequirementEntries
	property var requirementGraph: []
	property var selectedFeature: null
	property var activeFeature: selectedFeature
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
		rebuildRequirementEntries()
		CachedFeatureCollection.updateModel()
		CachedFeatureCollection.modelUpdated.connect(featureEditor.onFeatureCollectionUpdated)
		__updateCommandStates()
		multiPageView.updatePages()
		// Both pages are loaded up-front (rather than lazily on first visit) because
		// the root-level functions below (rebuildFeatureTree, updateActiveFeature, etc.)
		// reach the Subfeatures page via multiPageView.getPageById("Subfeatures"), which
		// only returns a live instance once the page has been loaded.
		multiPageView.ensurePageLoaded(0)
		multiPageView.ensurePageLoaded(1)
	}

	Component.onDestruction: {
		CachedFeatureCollection.modelUpdated.disconnect(featureEditor.onFeatureCollectionUpdated)
	}

	function onFeatureCollectionUpdated() {
		featureEditor.rebuildRequirementEntries()
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
		rebuildRequirementEntries()
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
			&& isValidId(item.m_featureId)
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

	function updateActiveFeature() {
		activeFeature = selectedFeature
		rebuildAllRequirementEntries()
	}

	function activeFeaturePath() {
		return featurePath(activeFeature)
	}

	function findItemByPath(targetPath) {
		return findItemInTree(featureTree, rootFeaturePath, targetPath)
	}

	function findItemInTree(nodes, currentPrefix, targetPath) {
		for (let i = 0; i < nodes.length; ++i) {
			let item = sourceItem(nodes[i])
			if (!item)
				continue
			let p = currentPrefix + "/" + (item.m_featureId || "")
			if (p === targetPath)
				return item
			let childFound = findItemInTree(nodes[i].children || [], p, targetPath)
			if (childFound)
				return childFound
		}
		return null
	}

	function getDirectRequirementsForPath(path) {
		if (!path)
			return []
		if (path === rootFeaturePath && featureData)
			return directRequirementsOf(featureData)
		if (rootFeaturePath !== "" && path.indexOf(rootFeaturePath + "/") === 0) {
			let localItem = findItemByPath(path)
			if (localItem)
				return directRequirementsOf(localItem)
		}
		let graphEntry = requirementEntry(path)
		if (graphEntry && graphEntry.requirements) {
			let paths = graphEntry.requirements.split(';')
			let clean = []
			for (let i = 0; i < paths.length; ++i) {
				if (paths[i] !== "")
					clean.push(paths[i])
			}
			return clean
		}
		return []
	}

	function resolveRecursiveRequirements(directPaths) {
		if (!directPaths || directPaths.length === 0)
			return []
		let queue = []
		let visited = ({})
		let directList = []
		let transitiveList = []

		for (let i = 0; i < directPaths.length; ++i) {
			let p = directPaths[i]
			if (!p || visited[p])
				continue
			let name = requirementName(p)
			let fullPath = requirementFullPath(p)
			let item = {
				"featurePath": p,
				"featureName": name,
				"fullPath": fullPath,
				"isDirect": true,
				"canRemove": true,
				"sources": [],
				"sourceName": ""
			}
			visited[p] = item
			directList.push(item)
			queue.push(p)
		}

		while (queue.length > 0) {
			let currentPath = queue.shift()
			let currentItem = visited[currentPath]
			let childPaths = getDirectRequirementsForPath(currentPath)
			for (let j = 0; j < childPaths.length; ++j) {
				let childPath = childPaths[j]
				if (!childPath || childPath === currentPath)
					continue
				if (!visited[childPath]) {
					let childName = requirementName(childPath)
					let childFullPath = requirementFullPath(childPath)
					let childItem = {
						"featurePath": childPath,
						"featureName": childName,
						"fullPath": childFullPath,
						"isDirect": false,
						"canRemove": false,
						"sources": [currentItem.featureName],
						"sourceName": currentItem.featureName
					}
					visited[childPath] = childItem
					transitiveList.push(childItem)
					queue.push(childPath)
				} else {
					let existingItem = visited[childPath]
					if (!existingItem.isDirect) {
						if (existingItem.sources.indexOf(currentItem.featureName) < 0) {
							existingItem.sources.push(currentItem.featureName)
							existingItem.sourceName = existingItem.sources.join(", ")
						}
					}
				}
			}
		}

		return directList.concat(transitiveList)
	}

	function rebuildAllRequirementEntries() {
		if (featureData)
			rootRequirementEntries = resolveRecursiveRequirements(directRequirementsOf(featureData))
		else
			rootRequirementEntries = []

		if (selectedFeature)
			subfeatureRequirementEntries = resolveRecursiveRequirements(directRequirementsOf(selectedFeature))
		else
			subfeatureRequirementEntries = []

		activeRequirementEntries = subfeatureRequirementEntries
	}

	function rebuildActiveRequirementEntries() {
		rebuildAllRequirementEntries()
	}

	function rebuildRequirementEntries() {
		let tree = []
		if (featureCollectionModel) {
			tree = TreeModelBuilder.fromTreeItemModelByFields(featureCollectionModel, {
				key: FeatureItemTypeMetaInfo.s_featureId,
				children: FeatureItemTypeMetaInfo.s_subFeatures,
				columns: {
					featureName: FeatureItemTypeMetaInfo.s_featureName,
					featureId: FeatureItemTypeMetaInfo.s_featureId,
					optional: FeatureItemTypeMetaInfo.s_optional,
					requirements: FeatureItemTypeMetaInfo.s_requirements
				}
			})
		}

		if (featureData && featureData.m_featureId) {
			let currentId = featureData.m_featureId
			let foundIdx = -1
			for (let i = 0; i < tree.length; ++i) {
				let d = tree[i].data || {}
				if ((d[FeatureItemTypeMetaInfo.s_featureId] || d.featureId) === currentId) {
					foundIdx = i
					break
				}
			}
			let localRootNode = {
				key: currentId,
				text: featureData.m_featureName || currentId,
				data: {
					featureId: currentId,
					featureName: featureData.m_featureName || currentId,
					optional: featureData.m_optional === true,
					requirements: featureData.m_requirements || ""
				},
				children: featureTree || []
			}
			if (foundIdx >= 0)
				tree[foundIdx] = localRootNode
			else
				tree.push(localRootNode)
		}

		let graph = []
		appendRequirementGraphNodes(tree, [], "", graph)
		requirementGraph = graph

		let entries = []
		appendRequirementEntries(tree, [], "", "", "", entries)
		requirementEntries = entries
		rebuildMandatoryLeafCounts()
		rebuildAllRequirementEntries()
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

	function appendRequirementGraphNodes(nodes, pathNames, parentPath, target) {
		for (let i = 0; i < nodes.length; ++i) {
			let node = nodes[i]
			let data = node.data || {}
			let id = data[FeatureItemTypeMetaInfo.s_featureId] || data.featureId || data.m_featureId || ""
			if (id === "")
				continue
			let name = data[FeatureItemTypeMetaInfo.s_featureName] || data.featureName || data.m_featureName || id
			let currentNames = pathNames.concat([name])
			let path = parentPath + "/" + id
			target.push({
				"featurePath": path,
				"featureName": name,
				"fullPath": currentNames.join(" / "),
				"requirements": data[FeatureItemTypeMetaInfo.s_requirements] || data.requirements || data.m_requirements || ""
			})
			if (node.children && node.children.length > 0)
				appendRequirementGraphNodes(node.children, currentNames, path, target)
		}
	}

	// Flattens the feature collections into one row per feature, carrying the
	// full path it sits at. Both root features and their sub-features are included.
	function appendRequirementEntries(nodes, pathNames, parentPath, rootPath, rootName, target) {
		for (let i = 0; i < nodes.length; ++i) {
			let node = nodes[i]
			let data = node.data || {}
			let id = data[FeatureItemTypeMetaInfo.s_featureId] || data.featureId || data.m_featureId || ""
			if (id === "")
				continue
			let name = data[FeatureItemTypeMetaInfo.s_featureName] || data.featureName || data.m_featureName || id
			let currentNames = pathNames.concat([name])
			let path = parentPath + "/" + id
			// The feature a part belongs to, which is what a product takes.
			let currentRootPath = rootPath === "" ? path : rootPath
			let currentRootName = rootName === "" ? name : rootName

			target.push({
				"featureName": name,
				"fullPath": currentNames.join(" / "),
				"featurePath": path,
				"optional": (data[FeatureItemTypeMetaInfo.s_optional] === true || data.optional === true || data.m_optional === true),
				"rootPath": currentRootPath,
				"rootName": currentRootName
			})

			if (node.children && node.children.length > 0) {
				appendRequirementEntries(node.children, currentNames, path, currentRootPath, currentRootName, target)
			}
		}
	}

	function requirementName(featurePathValue) {
		if (!featurePathValue)
			return ""
		if (featurePathValue === rootFeaturePath && featureData)
			return featureData.m_featureName || featureData.m_featureId || featurePathValue
		let local = findItemByPath(featurePathValue)
		if (local)
			return local.m_featureName || local.m_featureId || featurePathValue
		let g = requirementEntry(featurePathValue)
		if (g && g.featureName)
			return g.featureName
		let entry = requirementEntryOf(featurePathValue)
		if (entry && entry.featureName)
			return entry.featureName
		let parts = featurePathValue.split('/')
		return parts.length > 0 ? parts[parts.length - 1] : featurePathValue
	}

	function requirementFullPath(featurePathValue) {
		if (!featurePathValue)
			return ""
		if (featurePathValue === rootFeaturePath && featureData)
			return featureData.m_featureName || featureData.m_featureId || featurePathValue
		let local = findItemByPath(featurePathValue)
		if (local)
			return featurePathValue
		let g = requirementEntry(featurePathValue)
		if (g && g.fullPath)
			return g.fullPath
		let entry = requirementEntryOf(featurePathValue)
		if (entry && entry.fullPath)
			return entry.fullPath
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

	function isValidId(text) {
		return text !== "" && /^[A-Za-z0-9_]+$/.test(text)
	}

	function rootIdConflict(candidateId) {
		if (!candidateId || candidateId === "" || !featureCollectionModel)
			return false
		for (let i = 0; i < featureCollectionModel.getItemsCount(); ++i) {
			let id = featureCollectionModel.getData(FeatureItemTypeMetaInfo.s_featureId, i)
			if (id === candidateId && candidateId !== featureEditor.featureId)
				return true
		}
		return false
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
			else if (item.m_featureId === "")
				details.push(qsTr("%1 - Feature ID is missing").arg(item.m_featureName))
			else if (!isValidId(item.m_featureId))
				details.push(qsTr("%1 - Feature ID contains invalid characters").arg(item.m_featureName))
			else if (siblingIdConflict(treeExplorer.currentParentNode(), item.m_featureId, item))
				details.push(qsTr("%1 - Feature ID is duplicated").arg(item.m_featureName))
		}
		return details
	}

	// Optional and Permission apply only to leaves. A feature with sub-features
	// may still carry requirements which are inherited by its descendants.
	function isLeafNode(node) {
		return node !== null && node !== undefined
			&& (!node.children || node.children.length === 0)
	}

	function subFeatureCount(node) {
		return node && node.children ? node.children.length : 0
	}

	function directRequirementsOf(item) {
		if (!item || !item.m_requirements)
			return []
		let paths = item.m_requirements.split(';')
		let clean = []
		for (let i = 0; i < paths.length; ++i) {
			if (paths[i] !== "")
				clean.push(paths[i])
		}
		return clean
	}

	function directRequirements() {
		return directRequirementsOf(selectedFeature ? selectedFeature : featureData)
	}

	function isDirectRequirement(featurePathValue) {
		return directRequirements().indexOf(featurePathValue) >= 0
	}

	function allRequirementPathsOf(item) {
		if (!item)
			return []
		let resolved = resolveRecursiveRequirements(directRequirementsOf(item))
		let paths = []
		for (let i = 0; i < resolved.length; ++i)
			paths.push(resolved[i].featurePath)
		return paths
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

	function isDescendantPath(featurePathValue) {
		let path = activeFeaturePath()
		return path !== "" && featurePathValue !== "" && featurePathValue.indexOf(path + "/") === 0
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
		if (isDescendantPath(featurePathValue))
			return false
		return !requirementReaches(featurePathValue, path, {})
	}

	function addRequirementsFor(targetItem, pathsToAdd) {
		if (!canEdit || !targetItem || !pathsToAdd || pathsToAdd.length === 0)
			return
		let targetPath = featurePath(targetItem)
		let paths = directRequirementsOf(targetItem)
		for (let i = 0; i < pathsToAdd.length; ++i) {
			let p = pathsToAdd[i]
			if (p === "" || paths.indexOf(p) >= 0)
				continue
			if (targetPath !== "" && p.indexOf(targetPath + "/") === 0)
				continue
			paths.push(p)
		}
		targetItem.m_requirements = paths.join(';')
		if (targetItem === featureData)
			featureEditor.doUpdateModel()
		else if (featureData && featureData.modelChanged)
			featureData.modelChanged([])
		rebuildAllRequirementEntries()
		rebuildFeatureTree(selectedFeature)
	}

	function removeRequirementFor(targetItem, featurePathValue) {
		if (!canEdit || !targetItem || !featurePathValue)
			return
		let paths = directRequirementsOf(targetItem)
		let index = paths.indexOf(featurePathValue)
		if (index >= 0) {
			paths.splice(index, 1)
			targetItem.m_requirements = paths.join(';')
			if (targetItem === featureData)
				featureEditor.doUpdateModel()
			else if (featureData && featureData.modelChanged)
				featureData.modelChanged([])
			rebuildAllRequirementEntries()
			rebuildFeatureTree(selectedFeature)
		}
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
		let target = selectedFeature ? selectedFeature : featureData
		target.m_requirements = paths.join(';')
		if (target === featureData)
			featureEditor.doUpdateModel()
		else if (featureData && featureData.modelChanged)
			featureData.modelChanged([])
		rebuildAllRequirementEntries()
		rebuildFeatureTree(selectedFeature)
	}

	function clearRequirementsFor(targetItem) {
		if (!canEdit || !targetItem)
			return
		targetItem.m_requirements = ""
		if (targetItem === featureData)
			featureEditor.doUpdateModel()
		else if (featureData && featureData.modelChanged)
			featureData.modelChanged([])
		rebuildAllRequirementEntries()
		rebuildFeatureTree(selectedFeature)
	}

	function addRequirements(pathsToAdd) {
		addRequirementsFor(selectedFeature ? selectedFeature : featureData, pathsToAdd)
	}

	function removeRequirement(featurePathValue) {
		removeRequirementFor(selectedFeature ? selectedFeature : featureData, featurePathValue)
	}

	function clearRequirements() {
		clearRequirementsFor(selectedFeature ? selectedFeature : featureData)
	}

	property var requirementsDialogTarget: null

	function openRequirementsDialogFor(targetItem) {
		if (!canEdit || !targetItem)
			return
		requirementsDialogTarget = targetItem
		ModalDialogManager.openDialog(requirementsDialogComp, {})
	}

	function openRequirementsDialog() {
		openRequirementsDialogFor(selectedFeature ? selectedFeature : featureData)
	}

	property Component requirementsDialogComp: Component {
		RequirementsDialog {
			currentRootPath: featureEditor.rootFeaturePath
			currentRootName: featureEditor.featureData ? (featureEditor.featureData.m_featureName || featureEditor.featureData.m_featureId || "") : ""
			targetFeaturePath: featureEditor.featurePath(featureEditor.requirementsDialogTarget)
			targetFeatureName: featureEditor.requirementsDialogTarget ? (featureEditor.requirementsDialogTarget.m_featureName || "") : ""
			alreadyRequiredPaths: featureEditor.allRequirementPathsOf(featureEditor.requirementsDialogTarget)
			requirementEntries: featureEditor.requirementEntries
			requirementGraph: featureEditor.requirementGraph
			mandatoryLeafCounts: featureEditor.mandatoryLeafCounts

			onFinished: {
				if (buttonId !== Enums.ok || checkedPaths.length === 0)
					return
				featureEditor.addRequirementsFor(featureEditor.requirementsDialogTarget, checkedPaths)
			}
		}
	}

	function updateGui() {
		if (!featureData)
			return
		var rootFeaturePageInstance = multiPageView.getPageByIndex(0)
		if (rootFeaturePageInstance)
			rootFeaturePageInstance.updateGui()
		rebuildFeatureTree(selectedFeature)
		updateActiveFeature()
	}

	function updateModel() {
		if (!featureData)
			return
		var rootFeaturePageInstance = multiPageView.getPageByIndex(0)
		if (rootFeaturePageInstance)
			rootFeaturePageInstance.updateModel()
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
			multiPageView.addPage("RootFeature", qsTr("Root Feature"), rootFeaturePageComp, "Icons/FeatureList")
			multiPageView.addPage("Subfeatures", qsTr("Sub-features"), subfeaturesPageComp, "Icons/FeaturePackage")
			multiPageView.currentIndex = 0
		}
	}

	Component {
		id: rootFeaturePageComp

		Item {
			id: rootFeaturePage
			anchors.fill: parent

			function updateGui() {
				rootFeatureGroup.updateGui()
			}

			function updateModel() {
				rootFeatureGroup.updateModel()
			}

			TableHeaders {
				id: rootReqHeaders
				Component.onCompleted: {
					addHeader("featureName", qsTr("Feature"), false, false)
					addHeader("fullPath", qsTr("Full path"), false, false)
					addHeader("source", qsTr("Source"), false, false)
					addHeader("actions", "", false, false)
				}
			}

			Component {
				id: rootReqNameColComp
				TableCellDelegateBase {
					id: nameCellBase

					Item {
						anchors.fill: parent

						Image {
							id: reqDepIcon
							anchors.verticalCenter: parent.verticalCenter
							anchors.left: parent.left
							anchors.leftMargin: Style.marginM
							width: Style.iconSizeS
							height: width
							sourceSize.width: width
							sourceSize.height: height
							source: "qrc:/" + Style.getIconPath("Icons/Dependencies", Icon.State.On, Icon.Mode.Normal)
						}

						BaseText {
							id: nameText
							anchors.verticalCenter: parent.verticalCenter
							anchors.left: reqDepIcon.right
							anchors.leftMargin: Style.marginS
							anchors.right: parent.right
							anchors.rightMargin: Style.marginS
							elide: Text.ElideRight
							font.family: Style.fontFamilyBold
							font.pixelSize: Style.fontSizeM
							color: Style.textColor
						}
					}

					onReused: {
						let item = (nameCellBase.table && nameCellBase.table.elements && nameCellBase.rowIndex >= 0 && nameCellBase.rowIndex < nameCellBase.table.elements.length)
							? nameCellBase.table.elements[nameCellBase.rowIndex]
							: (nameCellBase.rowDelegate ? nameCellBase.rowDelegate.dataModel : null)
						nameText.text = item ? (item.featureName || item.featurePath) : ""
					}
				}
			}

			Component {
				id: rootReqPathColComp
				TableCellDelegateBase {
					id: pathCellBase

					BaseText {
						id: pathText
						anchors.verticalCenter: parent.verticalCenter
						anchors.left: parent.left
						anchors.leftMargin: Style.marginM
						anchors.right: parent.right
						anchors.rightMargin: Style.marginS
						elide: Text.ElideRight
						font.pixelSize: Style.fontSizeS
						color: Style.subtitleColor
					}

					onReused: {
						let item = (pathCellBase.table && pathCellBase.table.elements && pathCellBase.rowIndex >= 0 && pathCellBase.rowIndex < pathCellBase.table.elements.length)
							? pathCellBase.table.elements[pathCellBase.rowIndex]
							: (pathCellBase.rowDelegate ? pathCellBase.rowDelegate.dataModel : null)
						pathText.text = item ? (item.fullPath !== "" ? item.fullPath : item.featurePath) : ""
					}
				}
			}

			Component {
				id: rootReqSourceColComp
				TableCellDelegateBase {
					id: sourceCellBase

					BaseText {
						id: sourceText
						anchors.verticalCenter: parent.verticalCenter
						anchors.left: parent.left
						anchors.leftMargin: Style.marginM
						anchors.right: parent.right
						anchors.rightMargin: Style.marginS
						elide: Text.ElideRight
						font.pixelSize: Style.fontSizeS
					}

					onReused: {
						let item = (sourceCellBase.table && sourceCellBase.table.elements && sourceCellBase.rowIndex >= 0 && sourceCellBase.rowIndex < sourceCellBase.table.elements.length)
							? sourceCellBase.table.elements[sourceCellBase.rowIndex]
							: (sourceCellBase.rowDelegate ? sourceCellBase.rowDelegate.dataModel : null)
						if (!item) {
							sourceText.text = ""
							return
						}
						if (item.isDirect) {
							sourceText.text = qsTr("Direct")
							sourceText.color = Style.subtitleColor
							sourceText.font.family = Style.fontFamily
						} else {
							sourceText.text = qsTr("via %1").arg(item.sourceName)
							sourceText.color = Style.firstColor
							sourceText.font.family = Style.fontFamilyBold
						}
					}
				}
			}

			Component {
				id: rootReqActionsColComp
				TableCellDelegateBase {
					id: actionCellBase

					Connections {
						target: featureEditor
						function onCanEditChanged() {
							actionCellBase.reused()
						}
					}

					Item {
						anchors.centerIn: parent
						width: Style.controlHeightS
						height: Style.controlHeightS

						ToolButton {
							id: removeBtn
							anchors.fill: parent
							iconSource: "qrc:/" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
							tooltipText: qsTr("Remove requirement")
							onClicked: {
								let item = (actionCellBase.table && actionCellBase.table.elements && actionCellBase.rowIndex >= 0 && actionCellBase.rowIndex < actionCellBase.table.elements.length)
									? actionCellBase.table.elements[actionCellBase.rowIndex]
									: (actionCellBase.rowDelegate ? actionCellBase.rowDelegate.dataModel : null)
								if (item)
									featureEditor.removeRequirementFor(featureEditor.featureData, item.featurePath)
							}
						}

						Item {
							id: lockIndicator
							anchors.fill: parent

							Image {
								anchors.centerIn: parent
								width: Style.iconSizeXS
								height: width
								sourceSize.width: width
								sourceSize.height: height
								source: "qrc:/" + Style.getIconPath("Icons/Lock", Icon.State.On, Icon.Mode.Disabled)
							}

							MouseArea {
								id: lockMouse
								anchors.fill: parent
								hoverEnabled: true
							}

							CustomTooltip {
								id: lockTooltip
								parent: lockIndicator
								visible: lockMouse.containsMouse
							}
						}
					}

					onReused: {
						let item = (actionCellBase.table && actionCellBase.table.elements && actionCellBase.rowIndex >= 0 && actionCellBase.rowIndex < actionCellBase.table.elements.length)
							? actionCellBase.table.elements[actionCellBase.rowIndex]
							: (actionCellBase.rowDelegate ? actionCellBase.rowDelegate.dataModel : null)
						let canRemove = item ? (item.canRemove && featureEditor.canEdit) : false
						removeBtn.visible = canRemove
						lockIndicator.visible = item ? !item.canRemove : false
						if (item && !item.canRemove) {
							lockTooltip.text = qsTr("Required via %1 (cannot be removed directly)").arg(item.sourceName)
						}
					}
				}
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
						title: qsTr("Root Feature")
						groupView: rootFeatureGroup
					}

					GroupElementView {
						id: rootFeatureGroup
						width: parent.width

						property bool idCustomized: false
						property string idValidationMessage: {
							if (featureIdInput.text === "")
								return qsTr("Required. Filled in from the name if you leave it empty.")
							if (!featureEditor.isValidId(featureIdInput.text))
								return qsTr("Invalid identifier. Use only Latin letters, digits and underscores.")
							if (featureEditor.rootIdConflict(featureIdInput.text))
								return qsTr("Identifier '%1' is already in use by another feature.").arg(featureIdInput.text)
							return ""
						}

						TextInputElementView {
							id: featureNameInput
							objectName: "FeatureNameInput"
							// Both of these must be filled in before the feature can be
							// given sub-features, so the page says so where they are
							// rather than leaving the reader to infer it from a command
							// that never lights up.
							name: qsTr("Feature Name") + " *"
							description: featureNameInput.text === "" ? qsTr("Required. A feature needs a name before it can have sub-features.") : ""
							descriptionColor: Style.errorTextColor
							placeHolderText: qsTr("Enter the feature name")
							readOnly: !featureEditor.canEdit
							onTextEdited: {
								if (!rootFeatureGroup.idCustomized)
									featureIdInput.text = featureEditor.sanitizeId(text)
								featureEditor.doUpdateModel()
							}
							onEditingFinished: {
								if (featureIdInput.text === "")
									featureIdInput.text = featureEditor.sanitizeId(text)
								featureEditor.doUpdateModel()
							}
							KeyNavigation.tab: featureIdInput
						}

						TextInputElementView {
							id: featureIdInput
							objectName: "FeatureIdInput"
							name: qsTr("Feature ID") + " *"
							description: rootFeatureGroup.idValidationMessage
							descriptionColor: rootFeatureGroup.idValidationMessage !== "" ? Style.errorTextColor : Style.descriptionColor
							borderColor: rootFeatureGroup.idValidationMessage !== "" ? Style.errorTextColor : ""
							placeHolderText: qsTr("Enter the feature ID")
							readOnly: !featureEditor.canEdit
							onTextEdited: {
								rootFeatureGroup.idCustomized = (text !== featureEditor.sanitizeId(featureNameInput.text))
								featureEditor.doUpdateModel()
							}
							onEditingFinished: featureEditor.doUpdateModel()
							KeyNavigation.tab: descriptionInput
							KeyNavigation.backtab: featureNameInput
						}

						TextInputElementView {
							id: descriptionInput
							objectName: "DescriptionInput"
							name: qsTr("Description")
							placeHolderText: qsTr("Enter the description")
							readOnly: !featureEditor.canEdit
							onEditingFinished: featureEditor.doUpdateModel()
							KeyNavigation.backtab: featureIdInput
						}

						SwitchElementView {
							id: optionalSwitch
							objectName: "OptionalSwitch"
							name: qsTr("Optional")
							readOnly: !featureEditor.canEdit
							onCheckedChanged: featureEditor.doUpdateModel()
						}

						SwitchElementView {
							id: permissionSwitch
							objectName: "PermissionSwitch"
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
								CustomTextField {
									id: fullPathField
									objectName: "FullPathField"
									width: fullPathElement.controlWidth
									height: Style.controlHeightM
									readOnly: true
									hasActionMenu: false
									textFieldRightMargin: copyFullPathButton.width + 2 * Style.marginXS
									text: featureEditor.rootFeaturePath

									ToolButton {
										id: copyFullPathButton
										z: fullPathField.z + 2
										anchors.right: parent.right
										anchors.rightMargin: Style.marginXS
										anchors.verticalCenter: parent.verticalCenter
										width: Style.buttonWidthM
										height: width
										enabled: featureEditor.rootFeaturePath !== ""
										iconSource: featureEditor.copyIconSource(featureEditor.rootFeaturePath)
										tooltipText: qsTr("Copy the full path")
										onClicked: featureEditor.copyFeaturePath(featureEditor.featureData)
									}
								}
							}
						}

						TableElementView {
							id: rootRequirementsTableElement
							width: parent.width
							name: qsTr("Requirements")

							controlComp: Component {
								Row {
									width: implicitWidth
									height: Style.controlHeightM
									spacing: Style.spacingS

									ToolbarButton {
										id: clearRootReqButton
										anchors.verticalCenter: parent.verticalCenter
										visible: featureEditor.canEdit && featureEditor.rootRequirementEntries.length > 0
										active: true
										text: qsTr("Clear all")
										onClicked: featureEditor.clearRequirementsFor(featureEditor.featureData)
									}

									Button {
										id: addRootReqButton
										anchors.verticalCenter: parent.verticalCenter
										height: Style.controlHeightM
										text: qsTr("Add requirement")
										iconSource: "qrc:/" + Style.getIconPath("Icons/Add", Icon.State.On, Icon.Mode.Normal)
										enabled: featureEditor.canEdit && featureEditor.featureData !== null
										onClicked: featureEditor.openRequirementsDialogFor(featureEditor.featureData)
									}
								}
							}

							bottomComp: Component {
								Item {
									id: bottomContainer
									width: rootRequirementsTableElement.contentWidth
									height: featureEditor.rootRequirementEntries.length === 0
										? Style.controlHeightM
										: (reqTable.contentHeight + reqTable.headerHeight)

									Table {
										id: reqTable
										anchors.top: parent.top
										anchors.left: parent.left
										anchors.right: parent.right
										height: contentHeight + headerHeight
										canMoveColumns: false
										selectable: false
										visible: featureEditor.rootRequirementEntries.length > 0

										Component.onCompleted: {
											rootRequirementsTableElement.table = reqTable
											reqTable.headers = rootReqHeaders
											reqTable.setColumnContentById("featureName", rootReqNameColComp)
											reqTable.setColumnContentById("fullPath", rootReqPathColComp)
											reqTable.setColumnContentById("source", rootReqSourceColComp)
											reqTable.setColumnContentById("actions", rootReqActionsColComp)
											let actionIdx = rootReqHeaders.getHeaderIndex("actions")
											if (actionIdx >= 0 && reqTable.widthDecorator.getItemsCount() > actionIdx) {
												reqTable.widthDecorator.setData("Width", Style.buttonWidthM + Style.marginM, actionIdx)
												reqTable.widthDecorator.setData("WidthPercent", -1, actionIdx)
												reqTable.setWidth()
											}
											reqTable.elements = featureEditor.rootRequirementEntries
										}

										Connections {
											target: featureEditor
											function onRootRequirementEntriesChanged() {
												reqTable.elements = featureEditor.rootRequirementEntries
												rootFeatureGroup.scheduleRecalc()
											}
										}
									}

									Item {
										anchors.fill: parent
										visible: featureEditor.rootRequirementEntries.length === 0

										Row {
											anchors.centerIn: parent
											spacing: Style.spacingS

											Image {
												anchors.verticalCenter: parent.verticalCenter
												width: Style.iconSizeXS
												height: width
												source: "qrc:/" + Style.getIconPath("Icons/Dependencies", Icon.State.On, Icon.Mode.Disabled)
												sourceSize.width: width
												sourceSize.height: height
											}

											BaseText {
												anchors.verticalCenter: parent.verticalCenter
												text: qsTr("No requirements")
												font.pixelSize: Style.fontSizeS
												color: Style.inactiveTextColor
											}
										}
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
							rootFeatureGroup.idCustomized = featureIdInput.text !== ""
								&& featureIdInput.text !== featureEditor.sanitizeId(featureNameInput.text)
						}

						function updateModel() {
							if (!featureEditor.featureData)
								return
							// A blank name is left as it was rather than saved - the field
							// itself keeps showing the rejected text and its red "Required"
							// hint, so nothing here needs to say it twice.
							if (featureNameInput.text !== "")
								featureEditor.featureData.m_featureName = featureNameInput.text
							if (featureIdInput.text !== "" && featureEditor.isValidId(featureIdInput.text) && !featureEditor.rootIdConflict(featureIdInput.text))
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

			// Column geometry, shared by the header, the rows and the drag handles
			// the explorer draws, so none of the three can drift from the others.
			// A column whose breakpoint is above the current table width folds
			// away and its share is handed to the columns that stay.
			TableColumnLayout {
				id: subfeatureColumns
				fractions: [0.32, 0.22, 0.30, 0.08, 0.08]
				breakpoints: [0, 340, 640, 0, 500]
			}

			function columnWidth(index, width, spacing) {
				return subfeatureColumns.widthOf(index, width, spacing)
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
						// A sibling using this id already or invalid id is left as-is rather than
						// applied: the id would collide or be corrupt on save.
						let newFeatureId = idCell.pendingValue()
						if (newFeatureId !== "" && featureEditor.isValidId(newFeatureId) && !featureEditor.siblingIdConflict(treeExplorer.currentParentNode(), newFeatureId, item))
							item.m_featureId = newFeatureId
						item.m_description = descriptionCell.pendingValue()
						item.m_optional = rowContent.draftOptional
						item.m_isPermission = rowContent.draftPermission
						if (grouped)
							item.endChanges()
					}

					// Escape belongs to the whole row: the drafts are simply dropped.
					function cancelEditing() {
						treeExplorer.cancelEditRow()
						treeExplorer.forceActiveFocus()
						Qt.callLater(function() {
							treeExplorer.forceActiveFocus()
						})
					}

					EditableTableCell {
						id: nameCell
						objectName: "SubfeatureNameCell"
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
						objectName: "SubfeatureIdCell"
						width: subfeaturesPage.columnWidth(1, rowContent.width, rowContent.spacing)
						height: Style.controlHeightM
						anchors.verticalCenter: parent.verticalCenter
						visible: width > 0
						required: true
						editing: rowContent.editing
						readOnly: !featureEditor.canEdit
						placeHolderText: qsTr("Identifier")
						value: rowContent.sourceItem ? rowContent.sourceItem.m_featureId : ""
						// Flags a sibling with the same id or invalid format while typing; commitEditing()
						// is what actually keeps the duplicate or invalid id out of the model.
						invalid: rowContent.editing && (featureEditor.siblingIdConflict(
								treeExplorer.currentParentNode(), idCell.draft, rowContent.sourceItem)
								|| (idCell.draft !== "" && !featureEditor.isValidId(idCell.draft)))
						nextEditor: descriptionCell.editorItem
						previousEditor: nameCell.editorItem
						onEditingEndRequested: treeExplorer.commitEditRow()
						onEditingCancelRequested: rowContent.cancelEditing()
					}

					EditableTableCell {
						id: descriptionCell
						objectName: "SubfeatureDescriptionCell"
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
						objectName: "SubfeatureOptionalCell"
						width: subfeaturesPage.columnWidth(3, rowContent.width, rowContent.spacing)
						height: Style.controlHeightM
						anchors.verticalCenter: parent.verticalCenter
						visible: width > 0

						SwitchCustom {
							id: optionalSwitch
							anchors.centerIn: parent
							height: Style.controlHeightS
							readOnly: !featureEditor.canEdit || !rowContent.editing
							backgroundColorChecked: readOnly ? Style.inactiveTextColor : Style.lightBlueColor
							controlColorChecked: Style.firstColor
							onCheckedChanged: {
								if (rowContent.editing)
									rowContent.draftOptional = optionalSwitch.checked
							}
						}
					}

					Item {
						id: permissionCell
						objectName: "SubfeaturePermissionCell"
						width: subfeaturesPage.columnWidth(4, rowContent.width, rowContent.spacing)
						height: Style.controlHeightM
						anchors.verticalCenter: parent.verticalCenter
						visible: width > 0

						SwitchCustom {
							id: permissionSwitch
							anchors.centerIn: parent
							height: Style.controlHeightS
							readOnly: !featureEditor.canEdit || !rowContent.editing
							backgroundColorChecked: readOnly ? Style.inactiveTextColor : Style.lightBlueColor
							controlColorChecked: Style.firstColor
							onCheckedChanged: {
								if (rowContent.editing)
									rowContent.draftPermission = permissionSwitch.checked
							}
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

			// Sub-features indicator & drill-down action positioned next to the Edit button
			property Component subfeatureRowActionComp: Component {
				Item {
					id: subfeatureActionItem
					width: 36
					height: Style.controlHeightM

					property var node: parent ? parent.node : null
					property bool editing: parent ? parent.editing : false
					property bool rowSelected: parent ? parent.selected : false

					property int count: featureEditor.subFeatureCount(subfeatureActionItem.node)
					property bool complete: featureEditor.nodeIsComplete(subfeatureActionItem.node)
					property bool hasIssues: featureEditor.subtreeHasIssues(subfeatureActionItem.node)
					property bool openable: subfeatureActionItem.complete && !subfeatureActionItem.editing

					visible: !subfeatureActionItem.editing && (subfeatureActionItem.count > 0 || subfeatureActionItem.rowSelected || actionMouse.containsMouse)

					Rectangle {
						id: countPill
						anchors.centerIn: parent
						width: Math.max(Style.controlHeightS, pillRow.width + 2 * Style.marginS)
						height: Style.controlHeightS
						radius: height / 2
						color: actionMouse.containsMouse ? Style.titleColor
							: subfeatureActionItem.count > 0 ? Style.backgroundColor2 : "transparent"
						border.color: subfeatureActionItem.hasIssues ? Style.errorTextColor
							: actionMouse.containsMouse || subfeatureActionItem.count > 0
								? Style.borderColor : "transparent"
						border.width: 1

						Row {
							id: pillRow
							anchors.centerIn: parent
							spacing: Style.spacingXS

							Image {
								id: actionWarningIcon
								anchors.verticalCenter: parent.verticalCenter
								visible: subfeatureActionItem.hasIssues
								width: visible ? Style.iconSizeXS : 0
								height: Style.iconSizeXS
								sourceSize.width: width
								sourceSize.height: height
								source: "qrc:/" + Style.getIconPath("Icons/Warning", Icon.State.On, Icon.Mode.Normal)
							}

							BaseText {
								id: actionCountText
								anchors.verticalCenter: parent.verticalCenter
								text: subfeatureActionItem.count > 0 ? subfeatureActionItem.count : ">"
								font.pixelSize: subfeatureActionItem.count > 0 ? Style.fontSizeS : Style.fontSizeM
								font.family: Style.fontFamilyBold
								color: actionMouse.containsMouse ? Style.baseColor
									: subfeatureActionItem.hasIssues ? Style.errorTextColor
									: subfeatureActionItem.count > 0 ? Style.titleColor : Style.subtitleColor
							}
						}
					}

					MouseArea {
						id: actionMouse
						anchors.fill: parent
						hoverEnabled: true
						cursorShape: subfeatureActionItem.openable ? Qt.PointingHandCursor : Qt.ArrowCursor
						onEntered: subfeaturesPage.showPreview(subfeatureActionItem.node, subfeatureActionItem)
						onExited: subfeaturesPage.hidePreview()
						onClicked: {
							if (subfeatureActionItem.openable)
								treeExplorer.navigateInto(subfeatureActionItem.node)
						}
					}

					CustomTooltip {
						id: actionTooltip
						parent: countPill
						text: subfeatureActionItem.count > 0
							? qsTr("%1 sub-features (click to open)").arg(subfeatureActionItem.count)
							: qsTr("Open sub-features")
						visible: actionMouse.containsMouse
					}
				}
			}

			// Whatever the narrow cells cut short, in full: the name, the path that
			// requirements and permissions are written in, and the description.
			property Component subfeatureDetailsComp: Component {
				TableRowDetails {
					id: subfeatureDetails
					objectName: "SubfeatureDetails"
					title: featureEditor.selectedFeature ? featureEditor.featureName(treeExplorer.selectedNode) : ""
					keyText: featureEditor.selectedFeature ? featureEditor.featurePath(featureEditor.selectedFeature) : ""
					description: featureEditor.selectedFeature ? featureEditor.featureDescription(treeExplorer.selectedNode) : ""
					placeholderText: qsTr("Select a sub-feature to see its full name, path and description")
					copyVisible: true
					copyIconSource: featureEditor.copyIconSource(subfeatureDetails.keyText)
					copyTooltipText: qsTr("Copy the full path")
					onCopyRequested: featureEditor.copyFeaturePath(featureEditor.selectedFeature)
				}
			}

			// Right-hand table of the page: shows only the requirements that have
			// been added to the active feature. Adding new requirements is done via
			// the "Add requirement" button which opens RequirementsDialog.
			property Component requirementsPanelComp: Component {
				FocusScope {
					id: requirementsPanel
					objectName: "DependenciesPanel"
					anchors.fill: parent

					property var selectedEntries: featureEditor.subfeatureRequirementEntries
					property bool hasSelectedSubfeature: featureEditor.selectedFeature !== null && treeExplorer.checkedNodes.length < 2
					property string subtitleText: {
						if (treeExplorer.checkedNodes.length > 1)
							return ""
						if (featureEditor.selectedFeature)
							return featureEditor.selectedFeature.m_featureName || ""
						return ""
					}

					Item {
						id: panelHeader
						anchors.top: parent.top
						anchors.left: parent.left
						anchors.right: parent.right
						height: Style.controlHeightL

						BaseText {
							id: titleText
							objectName: "PanelTitle"
							anchors.left: parent.left
							anchors.verticalCenter: parent.verticalCenter
							text: qsTr("Requirements")
							font.family: Style.fontFamilyBold
							font.pixelSize: Style.fontSizeL
							color: Style.titleColor
						}

						BaseText {
							id: subtitleLabel
							objectName: "PanelSubtitle"
							anchors.left: titleText.right
							anchors.leftMargin: Style.marginS
							anchors.right: clearButton.visible ? clearButton.left : parent.right
							anchors.rightMargin: Style.marginS
							anchors.verticalCenter: parent.verticalCenter
							visible: requirementsPanel.subtitleText !== ""
							text: requirementsPanel.subtitleText
							font.pixelSize: Style.fontSizeS
							color: Style.subtitleColor
							elide: Text.ElideRight
						}

						ToolbarButton {
							id: clearButton
							objectName: "ClearRequirementsButton"
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							visible: featureEditor.canEdit && requirementsPanel.hasSelectedSubfeature && requirementsPanel.selectedEntries.length > 0
							active: true
							text: qsTr("Clear all")
							onClicked: featureEditor.clearRequirementsFor(featureEditor.selectedFeature)
						}
					}

					Item {
						id: actionsBar
						anchors.top: panelHeader.bottom
						anchors.topMargin: Style.marginM
						anchors.left: parent.left
						anchors.right: parent.right
						height: Style.controlHeightM

						Button {
							id: addReqButton
							objectName: "AddRequirementButton"
							anchors.left: parent.left
							anchors.right: parent.right
							height: Style.controlHeightM
							text: qsTr("Add requirement")
							iconSource: "qrc:/" + Style.getIconPath("Icons/Add", Icon.State.On, Icon.Mode.Normal)
							enabled: featureEditor.canEdit && requirementsPanel.hasSelectedSubfeature
							onClicked: featureEditor.openRequirementsDialogFor(featureEditor.selectedFeature)
						}
					}

					Item {
						id: columnHeader
						anchors.top: actionsBar.bottom
						anchors.topMargin: Style.marginL
						anchors.left: parent.left
						anchors.right: parent.right
						height: Style.controlHeightS

						BaseText {
							anchors.left: parent.left
							anchors.leftMargin: Style.marginL
							anchors.verticalCenter: parent.verticalCenter
							text: qsTr("Required features (%1)").arg(requirementsPanel.selectedEntries.length)
							font.family: Style.fontFamilyBold
							font.pixelSize: Style.fontSizeS
							color: Style.subtitleColor
						}
					}

					Rectangle {
						id: panelFrame
						objectName: "PanelTable"
						anchors.top: columnHeader.bottom
						anchors.topMargin: Style.marginXS
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.bottom: parent.bottom
						radius: Style.radiusM
						color: Style.baseColor
						border.color: Style.borderColor
						border.width: 1
						clip: true

						ListView {
							id: entriesView
							anchors.fill: parent
							anchors.margins: 1
							anchors.rightMargin: 1 + Style.marginM
							visible: requirementsPanel.hasSelectedSubfeature && requirementsPanel.selectedEntries.length > 0
							model: requirementsPanel.hasSelectedSubfeature ? requirementsPanel.selectedEntries : []
							clip: true
							boundsBehavior: Flickable.StopAtBounds

							delegate: Rectangle {
								id: entryRow
								width: entriesView.width
								height: Style.controlHeightL + Style.marginS
								color: entryMouse.containsMouse ? Style.hover : "transparent"

								MouseArea {
									id: entryMouse
									anchors.fill: parent
									hoverEnabled: true
								}

								// Clean feature icon
								Image {
									id: reqIcon
									anchors.left: parent.left
									anchors.leftMargin: Style.marginM
									anchors.verticalCenter: parent.verticalCenter
									width: Style.iconSizeS
									height: width
									sourceSize.width: width
									sourceSize.height: height
									source: "qrc:/" + Style.getIconPath("Icons/Dependencies", Icon.State.On, Icon.Mode.Normal)
								}

								Column {
									anchors.left: reqIcon.right
									anchors.leftMargin: Style.marginS
									anchors.right: subActionArea.left
									anchors.rightMargin: Style.marginS
									anchors.verticalCenter: parent.verticalCenter
									spacing: Style.spacingXXS

									// Title line: Feature name has full width, never squeezed by badges
									BaseText {
										width: parent.width
										text: modelData.featureName || modelData.featurePath
										font.family: Style.fontFamilyBold
										font.pixelSize: Style.fontSizeM
										color: Style.textColor
										elide: Text.ElideRight
									}

									Row {
										width: parent.width
										spacing: Style.spacingS

										BaseText {
											text: modelData.fullPath !== "" ? modelData.fullPath : modelData.featurePath
											font.pixelSize: Style.fontSizeS
											color: Style.subtitleColor
											elide: Text.ElideRight
											width: Math.min(implicitWidth, parent.width - (subViaText.visible ? subViaText.implicitWidth + Style.spacingS : 0))
										}

										BaseText {
											id: subViaText
											visible: !modelData.isDirect && modelData.sourceName !== ""
											text: qsTr("via %1").arg(modelData.sourceName)
											font.pixelSize: Style.fontSizeS
											font.family: Style.fontFamilyBold
											color: Style.firstColor
											elide: Text.ElideRight
										}
									}
								}

								Item {
									id: subActionArea
									anchors.right: parent.right
									anchors.rightMargin: Style.marginM
									anchors.verticalCenter: parent.verticalCenter
									width: Style.controlHeightS
									height: Style.controlHeightS

									ToolButton {
										id: removeBtn
										objectName: "RemoveRequirementButton_" + index
										anchors.fill: parent
										visible: featureEditor.canEdit && modelData.canRemove
										iconSource: "qrc:/" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
										tooltipText: qsTr("Remove requirement")
										onClicked: featureEditor.removeRequirementFor(featureEditor.selectedFeature, modelData.featurePath)
									}

									Item {
										id: subLockIndicator
										anchors.fill: parent
										visible: !modelData.canRemove

										Image {
											anchors.centerIn: parent
											width: Style.iconSizeXS
											height: width
											sourceSize.width: width
											sourceSize.height: height
											source: "qrc:/" + Style.getIconPath("Icons/Lock", Icon.State.On, Icon.Mode.Disabled)
										}

										MouseArea {
											id: subLockMouse
											anchors.fill: parent
											hoverEnabled: true
										}

										CustomTooltip {
											parent: subLockIndicator
											text: qsTr("Required via %1 (cannot be removed directly)").arg(modelData.sourceName)
											visible: subLockMouse.containsMouse
										}
									}
								}

								// Clean divider between items
								Rectangle {
									anchors.left: parent.left
									anchors.leftMargin: Style.marginM
									anchors.right: parent.right
									anchors.rightMargin: Style.marginM
									anchors.bottom: parent.bottom
									height: 1
									color: Style.borderColor
									opacity: 0.3
									visible: index < entriesView.count - 1
								}
							}
						}

						CustomScrollbar {
							z: entriesView.z + 1
							anchors.right: parent.right
							anchors.rightMargin: Style.marginXS
							anchors.top: entriesView.top
							anchors.topMargin: Style.marginXS
							anchors.bottom: entriesView.bottom
							anchors.bottomMargin: Style.marginXS
							secondSize: Style.marginS
							targetItem: entriesView
							radius: Style.radiusS
						}

						Column {
							objectName: "PanelPlaceholder"
							anchors.centerIn: parent
							width: Math.min(parent.width - 2 * Style.marginXL, Style.sizeHintM)
							spacing: Style.spacingL
							visible: !requirementsPanel.hasSelectedSubfeature || requirementsPanel.selectedEntries.length === 0

							Image {
								anchors.horizontalCenter: parent.horizontalCenter
								width: Style.iconSizeL
								height: width
								source: "qrc:/" + Style.getIconPath("Icons/Dependencies", Icon.State.On, Icon.Mode.Disabled)
								sourceSize.width: width
								sourceSize.height: height
							}

							BaseText {
								width: parent.width
								horizontalAlignment: Text.AlignHCenter
								text: treeExplorer.checkedNodes.length > 1
									? qsTr("More than one row is ticked")
									: !requirementsPanel.hasSelectedSubfeature
										? qsTr("No sub-feature selected")
										: qsTr("No requirements")
								font.pixelSize: Style.fontSizeL
								color: Style.textColor
								wrapMode: Text.WordWrap
							}

							BaseText {
								width: parent.width
								horizontalAlignment: Text.AlignHCenter
								text: treeExplorer.checkedNodes.length > 1
									? qsTr("Requirements belong to a single sub-feature - untick the rest to edit them")
									: !requirementsPanel.hasSelectedSubfeature
										? qsTr("Select a sub-feature to view and edit its requirements")
										: qsTr("Click 'Add requirement' to select features that this feature depends on")
								font.pixelSize: Style.fontSizeS
								color: Style.subtitleColor
								wrapMode: Text.WordWrap
							}
						}
					}
				}
			}

			TreeExplorerView {
				id: treeExplorer
				objectName: "SubfeaturesExplorer"
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
					: qsTr("Fill in Feature Name and Feature ID on the Root Feature page to add sub-features here")
				selectedHintText: qsTr("Press Edit or F2 to change this row")
				renameVisible: false
				moveVisible: false
				rowIconVisible: false
				rowActionExtensionComponent: subfeaturesPage.subfeatureRowActionComp
				rowActionExtensionWidth: 44
				headerContentComponent: subfeaturesPage.subfeaturesHeaderComp
				rowContentComponent: subfeaturesPage.subfeatureRowComp
				columnLayout: subfeatureColumns
				detailsComponent: subfeaturesPage.subfeatureDetailsComp
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
				objectName: "SubfeaturePreviewPopup"
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
}
