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
	// Part of the product this view works in; scopes its permission checks.
	property string permissionPath: ""

	// The feature catalog, flattened: what each requirement path names, and the
	// nodes of every feature with the requirements they declare. Requirements
	// travel as paths ("/Reports/Advanced"), while a product holds feature
	// document ids - this is what maps the one onto the other.
	property var catalogNodesByPath: ({})
	property var catalogNodesByFeatureId: ({})
	// Who asked for what: a feature document id, or "<featureId>/<subFeatureId>"
	// for a part, against the names of the features requiring it. Nothing of this
	// is stored with the product - it follows from the requirements again, so it
	// cannot go stale when a feature is edited.
	property var requiredByIndex: ({})
	// The parts that were actually asked for, by the feature they belong to. A
	// product takes a feature whole, so what else came with it is only knowable
	// by comparing the two.
	property var requiredPartsIndex: ({})
	// Requirements of this product that the catalog has no feature for, and
	// features it requires but does not hold yet. Both are shown as a warning
	// rather than repaired behind the user's back.
	property var unresolvedRequirements: []
	property var missingRequiredNames: []
	// Features that nothing requires any more after the last change, and that
	// stayed in the product. They are named rather than dropped: a license may
	// already grant them, and a product that offers one feature too many is a
	// smaller problem than a license that grants what its product no longer has.
	property var freedFeatureNames: []

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

	// Every feature and part of the catalog, by the path requirements name it
	// with. Rebuilt whenever the catalog changes, which is what updateFeaturesGui
	// answers to.
	function rebuildCatalogIndex() {
		let byPath = ({})
		let byFeatureId = ({})
		let tree = TreeModelBuilder.fromTreeItemModelByFields(allFeaturesModel, {
			key: FeatureItemTypeMetaInfo.s_id,
			children: FeatureItemTypeMetaInfo.s_subFeatures,
			columns: {
				id: FeatureItemTypeMetaInfo.s_id,
				featureName: FeatureItemTypeMetaInfo.s_featureName,
				featureId: FeatureItemTypeMetaInfo.s_featureId,
				optional: FeatureItemTypeMetaInfo.s_optional,
				requirements: FeatureItemTypeMetaInfo.s_requirements
			}
		})
		for (let i = 0; i < tree.length; ++i) {
			let data = tree[i].data || {}
			collectCatalogNodes(tree[i], data.id || "", "", true, byPath, byFeatureId)
		}
		catalogNodesByPath = byPath
		catalogNodesByFeatureId = byFeatureId
	}

	function collectCatalogNodes(node, featureId, parentPath, isRoot, byPath, byFeatureId) {
		let data = node.data || {}
		let nodeId = data.featureId || ""
		if (featureId === "" || nodeId === "")
			return
		let nodePath = parentPath + "/" + nodeId
		let entry = {
			"featureId": featureId,
			"subFeatureId": isRoot ? "" : nodeId,
			"optional": data.optional === true,
			"featureName": data.featureName || nodeId,
			"requirements": data.requirements ? data.requirements.split(';') : []
		}
		byPath[nodePath] = entry
		if (!byFeatureId[featureId])
			byFeatureId[featureId] = []
		byFeatureId[featureId].push(entry)
		let children = node.children || []
		for (let i = 0; i < children.length; ++i)
			collectCatalogNodes(children[i], featureId, nodePath, false, byPath, byFeatureId)
	}

	function takenSubFeaturesOf(taken, featureId) {
		return taken[featureId] ? taken[featureId] : []
	}

	// Follows the requirements of the given features to the end and answers with
	// what the product has to hold: the features, the parts taken from them, and
	// who asked for each of them. A part that is not taken says nothing - what an
	// optional part requires counts once the product takes it.
	function resolveRequirements(featureIdList, optionalList) {
		let ids = featureIdList.slice()
		let taken = ({})
		for (let i = 0; i < optionalList.length; ++i) {
			if (ids.indexOf(optionalList[i].featureId) >= 0)
				taken[optionalList[i].featureId] = optionalList[i].subFeatureIds.slice()
		}
		let requiredBy = ({})
		// Which parts of a feature were actually asked for. A product takes a
		// feature whole, so the rest of it comes along - that is worth saying.
		let requiredParts = ({})
		let unresolved = []
		let pending = ids.slice()
		while (pending.length > 0) {
			let featureId = pending.shift()
			let nodes = catalogNodesByFeatureId[featureId] ? catalogNodesByFeatureId[featureId] : []
			for (let n = 0; n < nodes.length; ++n) {
				let node = nodes[n]
				if (node.subFeatureId !== "" && node.optional
						&& takenSubFeaturesOf(taken, featureId).indexOf(node.subFeatureId) < 0)
					continue
				for (let r = 0; r < node.requirements.length; ++r) {
					let path = node.requirements[r]
					if (path === "")
						continue
					let target = catalogNodesByPath[path]
					if (!target) {
						if (unresolved.indexOf(path) < 0)
							unresolved.push(path)
						continue
					}
					noteRequiredBy(requiredBy, target.featureId, node.featureName)
					if (ids.indexOf(target.featureId) < 0) {
						ids.push(target.featureId)
						pending.push(target.featureId)
					}
					if (target.subFeatureId === "")
						continue
					// A part is required, so the feature it belongs to stays as
					// well; an optional part on top of that has to be taken.
					noteRequiredBy(requiredBy, target.featureId + "/" + target.subFeatureId, node.featureName)
					noteRequiredBy(requiredParts, target.featureId, target.featureName)
					if (target.optional
							&& takenSubFeaturesOf(taken, target.featureId).indexOf(target.subFeatureId) < 0) {
						if (!taken[target.featureId])
							taken[target.featureId] = []
						taken[target.featureId].push(target.subFeatureId)
						pending.push(target.featureId)
					}
				}
			}
		}
		return {
			"featureIds": ids,
			"optionalFeatures": takenToOptionalFeatures(taken, ids),
			"requiredBy": requiredBy,
			"requiredParts": requiredParts,
			"unresolved": unresolved
		}
	}

	function noteRequiredBy(requiredBy, key, featureName) {
		if (!requiredBy[key])
			requiredBy[key] = []
		if (requiredBy[key].indexOf(featureName) < 0)
			requiredBy[key].push(featureName)
	}

	function takenToOptionalFeatures(taken, ids) {
		let result = []
		for (let i = 0; i < ids.length; ++i) {
			let subFeatureIds = takenSubFeaturesOf(taken, ids[i])
			if (subFeatureIds.length > 0)
				result.push({ "featureId": ids[i], "subFeatureIds": subFeatureIds.slice() })
		}
		return result
	}

	function sameIds(first, second) {
		if (first.length !== second.length)
			return false
		for (let i = 0; i < first.length; ++i) {
			if (second.indexOf(first[i]) < 0)
				return false
		}
		return true
	}

	function sameOptionalFeatures(first, second) {
		if (first.length !== second.length)
			return false
		for (let i = 0; i < first.length; ++i) {
			let ids = takenSubFeatureIdsIn(second, first[i].featureId)
			if (!sameIds(first[i].subFeatureIds, ids))
				return false
		}
		return true
	}

	function takenSubFeatureIdsIn(entries, featureId) {
		for (let i = 0; i < entries.length; ++i) {
			if (entries[i].featureId === featureId)
				return entries[i].subFeatureIds
		}
		return []
	}

	// Brings the product in line with what its features require. With applyChanges
	// the missing ones are added - that is what every edit of the feature set
	// does; without it the product is only marked, so that opening one never
	// writes to it behind the user's back. Running it twice changes nothing the
	// first run did not.
	// Answers whether the list of features itself changed, which is what decides
	// if the table has to be built anew.
	function applyRequirements(applyChanges) {
		let closure = resolveRequirements(selectedFeatureIds(), optionalFeatures)
		requiredByIndex = closure.requiredBy
		requiredPartsIndex = closure.requiredParts
		unresolvedRequirements = closure.unresolved
		if (!applyChanges) {
			missingRequiredNames = missingFeatureNames(closure.featureIds)
			return false
		}
		missingRequiredNames = []
		let featuresChanged = !sameIds(closure.featureIds, selectedFeatureIds())
		if (featuresChanged)
			applyFeatureIds(closure.featureIds)
		if (!sameOptionalFeatures(closure.optionalFeatures, optionalFeatures))
			applyOptionalFeatures(closure.optionalFeatures)
		return featuresChanged
	}

	function missingFeatureNames(closureIds) {
		let names = []
		let ids = selectedFeatureIds()
		for (let i = 0; i < closureIds.length; ++i) {
			if (ids.indexOf(closureIds[i]) >= 0)
				continue
			let nodes = catalogNodesByFeatureId[closureIds[i]]
			names.push(nodes && nodes.length > 0 ? nodes[0].featureName : closureIds[i])
		}
		return names
	}

	// The features that asked for this one; while there is any, the row stays.
	function requiredByNames(featureId) {
		return requiredByIndex[featureId] ? requiredByIndex[featureId] : []
	}

	function subFeatureRequiredByNames(featureId, subFeatureId) {
		let key = featureId + "/" + subFeatureId
		return requiredByIndex[key] ? requiredByIndex[key] : []
	}

	function featureIsRequired(featureId) {
		return requiredByNames(featureId).length > 0
	}

	// Requirement paths by the name of what they point at. An unknown one is
	// shown as the path it was written as, so a stale requirement is visible
	// instead of silently missing.
	function requirementNamesFor(paths) {
		let names = []
		for (let i = 0; i < paths.length; ++i) {
			let path = paths[i]
			if (path === "")
				continue
			let target = catalogNodesByPath[path]
			let name = target ? target.featureName : path
			if (names.indexOf(name) < 0)
				names.push(name)
		}
		return names
	}

	// What one part of a feature asks for. The feature itself is the part with
	// an empty sub-feature id.
	function requirementNamesOf(featureId, subFeatureId) {
		let nodes = catalogNodesByFeatureId[featureId] ? catalogNodesByFeatureId[featureId] : []
		for (let i = 0; i < nodes.length; ++i) {
			if (nodes[i].subFeatureId === subFeatureId)
				return requirementNamesFor(nodes[i].requirements)
		}
		return []
	}

	// What this feature asks for, by name: its own requirements and those of the
	// parts the product takes from it - the ones the product really pulls in.
	function featureRequirementNames(featureId) {
		let names = []
		let nodes = catalogNodesByFeatureId[featureId] ? catalogNodesByFeatureId[featureId] : []
		for (let n = 0; n < nodes.length; ++n) {
			let node = nodes[n]
			if (node.subFeatureId !== "" && node.optional
					&& takenSubFeatureIds(featureId).indexOf(node.subFeatureId) < 0)
				continue
			let nodeNames = requirementNamesFor(node.requirements)
			for (let i = 0; i < nodeNames.length; ++i) {
				if (names.indexOf(nodeNames[i]) < 0)
					names.push(nodeNames[i])
			}
		}
		return names
	}

	function requiredPartNames(featureId) {
		return requiredPartsIndex[featureId] ? requiredPartsIndex[featureId] : []
	}

	// The line under the panel: what holds the selected feature in this product,
	// or what it drags along, and only failing both the general note.
	function selectedFeatureFooterText() {
		let featureId = selectedRootFeatureId()
		let requiredBy = requiredByNames(featureId)
		if (requiredBy.length > 0) {
			// Naming the part that was asked for is what tells a feature that
			// belongs here as a whole from one whose single part was wanted -
			// the case where the parts below are worth making optional.
			let parts = requiredPartNames(featureId)
			if (parts.length > 0) {
				return qsTr("%1 needs %2; the rest of this feature comes with it, as its parts are not optional")
					.arg(namesSummary(requiredBy)).arg(namesSummary(parts))
			}
			return qsTr("%1 needs this feature, so it stays in the product").arg(namesSummary(requiredBy))
		}
		let requires = featureRequirementNames(featureId)
		if (requires.length > 0)
			return qsTr("This feature needs %1, which the product takes along").arg(namesSummary(requires))
		return qsTr("Which optional parts a license actually unlocks is decided on the Features page of that license")
	}

	function requirementWarningText() {
		if (missingRequiredNames.length > 0)
			return qsTr("%1 required missing").arg(missingRequiredNames.length)
		if (unresolvedRequirements.length > 0)
			return qsTr("%1 requirements unknown").arg(unresolvedRequirements.length)
		if (freedFeatureNames.length > 0)
			return qsTr("%1 no longer required").arg(freedFeatureNames.length)
		return ""
	}

	function requirementWarningDetails() {
		if (missingRequiredNames.length > 0)
			return missingRequiredNames
		if (unresolvedRequirements.length > 0)
			return unresolvedRequirements
		return freedFeatureNames
	}

	function updateFeaturesGui() {
		productFeaturesViewModel.clear()
		freedFeatureNames = []
		rebuildCatalogIndex()
		refreshFeatureIds()
		// A product is completed as it is opened, not only as it is edited: one
		// saved before a requirement was declared would otherwise show a feature
		// list its own features contradict, and would only right itself once the
		// user happened to touch something. A reader who may not change the
		// product gets the warning instead.
		applyRequirements(canEdit)
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

	// Drops the features and everything picked underneath them in one pass. A
	// feature another one requires is not dropped: it would come straight back,
	// so the row keeps its "Required by" mark instead of appearing to resist.
	// Removing the feature that asked for it first is what frees it.
	function removeRootFeatures(nodes) {
		if (!canEdit || !nodes || nodes.length === 0)
			return
		let doomed = []
		for (let i = 0; i < nodes.length; ++i) {
			let data = nodes[i] && nodes[i].data ? nodes[i].data : null
			if (!data || !data.id)
				continue
			if (featureIsRequiredByOthers(data.id, nodes))
				continue
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
		applyRequirements(true)
		updateFeaturesGui()
	}

	// Whether some feature that stays behind requires this one. Features removed
	// in the same pass do not count - dropping a feature together with what it
	// alone required is one decision, not a contradiction.
	function featureIsRequiredByOthers(featureId, removedNodes) {
		let remainingIds = []
		let ids = selectedFeatureIds()
		for (let i = 0; i < ids.length; ++i) {
			if (!nodesContainFeatureId(removedNodes, ids[i]))
				remainingIds.push(ids[i])
		}
		let closure = resolveRequirements(remainingIds, optionalFeatures)
		return closure.featureIds.indexOf(featureId) >= 0
	}

	function nodesContainFeatureId(nodes, featureId) {
		for (let i = 0; i < nodes.length; ++i) {
			let data = nodes[i] && nodes[i].data ? nodes[i].data : null
			if (data && data.id === featureId)
				return true
		}
		return false
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

	// The chip in the Sub-features column: plain count for a feature the product
	// took, "1 / 3" for one it holds because a single part of it was required -
	// the two numbers say at a glance how much of it nobody asked for.
	function subFeatureSummary(node) {
		let count = subFeatureCount(node)
		if (count === 0)
			return ""
		let featureId = node && node.data ? node.data.id || "" : ""
		let needed = requiredPartNames(featureId).length
		if (needed === 0 || needed >= count)
			return "" + count
		return needed + " / " + count
	}

	function subFeaturePartlyNeeded(node) {
		return subFeatureSummary(node).indexOf("/") >= 0
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

	function entryIsChangeable(entry) {
		if (!canEdit || !entry || !entry.optional)
			return false
		// A part another feature requires stays taken as long as that feature does.
		return subFeatureRequiredByNames(entry.rootId, entry.featureId).length === 0
	}

	function toggleSubFeature(entry) {
		if (!entryIsChangeable(entry))
			return
		let previousRequiredBy = requiredByIndex
		setSubFeatureTaken(entry.rootId, entry.featureId, !entryIsIncluded(entry))
		// Taking a part brings in what it requires. The table is only built anew
		// when that added a feature to the product - otherwise the row the user
		// is working in would lose its selection under their hands.
		let featuresChanged = applyRequirements(true)
		freedFeatureNames = collectFreedFeatures(previousRequiredBy)
		if (featuresChanged)
			updateFeaturesGui()
	}

	// What this change let go of: features the product still holds, that were
	// there for a requirement which no longer speaks. Whether they are wanted on
	// their own is a question only the user can answer, so they are listed.
	function collectFreedFeatures(previousRequiredBy) {
		let names = []
		let ids = selectedFeatureIds()
		for (let i = 0; i < ids.length; ++i) {
			let featureId = ids[i]
			if (!previousRequiredBy[featureId] || previousRequiredBy[featureId].length === 0)
				continue
			if (requiredByNames(featureId).length > 0)
				continue
			let nodes = catalogNodesByFeatureId[featureId]
			names.push(nodes && nodes.length > 0 ? nodes[0].featureName : featureId)
		}
		return names
	}

	// What the table says about a part: what holds it, or what it is otherwise.
	// What it requires is not spelled out here - a part may name a long list of
	// them, and the badge has one line.
	function entryStateText(entry) {
		if (!entry)
			return ""
		let requiredBy = subFeatureRequiredByNames(entry.rootId, entry.featureId)
		if (requiredBy.length > 0)
			return qsTr("Required by %1").arg(namesSummary(requiredBy))
		// A feature that is here for one of its parts brings the rest of them
		// with it, because they are not optional. Those are the parts nobody
		// asked for, and they say so instead of passing as a choice.
		if (entryCameAlong(entry))
			return qsTr("Came along")
		return entry.optional ? qsTr("Optional") : qsTr("Always included")
	}

	// A mandatory part of a feature that is in this product only because another
	// feature needs a different part of it.
	function entryCameAlong(entry) {
		if (!entry || entry.optional)
			return false
		if (subFeatureRequiredByNames(entry.rootId, entry.featureId).length > 0)
			return false
		return requiredPartNames(entry.rootId).length > 0
	}

	// Names for a place with one line to spare: the first few, and how many more.
	function namesSummary(names) {
		if (names.length <= 3)
			return names.join(", ")
		return qsTr("%1 and %2 more").arg(names.slice(0, 3).join(", ")).arg(names.length - 3)
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
		for (let i = 0; i < entries.length; ++i) {
			// Clearing leaves the parts another feature requires where they are.
			if (takeAll || !entryIsChangeable(entries[i]))
				ids.push(entries[i].featureId)
		}
		let result = []
		for (let j = 0; j < optionalFeatures.length; ++j) {
			if (optionalFeatures[j].featureId !== featureId)
				result.push(optionalFeatures[j])
		}
		if (ids.length > 0)
			result.push({ "featureId": featureId, "subFeatureIds": ids })
		let previousRequiredBy = requiredByIndex
		applyOptionalFeatures(result)
		let featuresChanged = applyRequirements(true)
		freedFeatureNames = collectFreedFeatures(previousRequiredBy)
		if (featuresChanged)
			updateFeaturesGui()
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
				// A feature is worth nothing without what it requires, so those
				// come along - transitively, and with the optional parts a
				// requirement points at.
				productViewContainer.applyRequirements(true)
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
			property var columnFractions: [0.22, 0.14, 0.20, 0.10, 0.14, 0.20]
			property var columnBreakpoints: [0, 460, 720, 300, 560, 380]

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
					Item {
						width: featuresPage.columnWidth(5, headerRow.width, headerRow.spacing)
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

					// The name, with a lock in front of it while another feature
					// holds this one in the product. The mark sits here rather than
					// only in the Requirements column, which folds away on a narrow
					// table.
					Item {
						id: featureNameCell
						width: featuresPage.columnWidth(0, rowContent.width, rowContent.spacing)
						height: rowContent.height
						visible: width > 0

						property bool locked: rowContent.nodeData
							? productViewContainer.featureIsRequired(rowContent.nodeData.id || "") : false

						Image {
							id: lockIcon
							anchors.left: parent.left
							anchors.leftMargin: Style.marginXS
							anchors.verticalCenter: parent.verticalCenter
							width: Style.iconSizeS
							height: width
							visible: featureNameCell.locked
							source: "qrc:/" + Style.getIconPath("Icons/Lock", Icon.State.On, Icon.Mode.Normal)
							fillMode: Image.PreserveAspectFit
						}

						BaseText {
							anchors.left: lockIcon.visible ? lockIcon.right : parent.left
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
						// "needed / total" while only part of this feature was asked
						// for, so the parts that came along are visible from the row.
						property bool partlyNeeded: productViewContainer.subFeaturePartlyNeeded(rowContent.node)

						Rectangle {
							anchors.centerIn: parent
							width: Math.max(Style.controlHeightS, countText.width + 2 * Style.marginS)
							height: Style.controlHeightS
							radius: height / 2
							visible: subfeaturesCell.count > 0
							color: Style.backgroundColor2
							border.color: subfeaturesCell.partlyNeeded ? Style.highlightColor : Style.borderColor
							border.width: 1

							BaseText {
								id: countText
								anchors.centerIn: parent
								text: productViewContainer.subFeatureSummary(rowContent.node)
								font.pixelSize: Style.fontSizeS
								font.family: Style.fontFamilyBold
								color: subfeaturesCell.partlyNeeded ? Style.highlightColor : Style.titleColor
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

					// What ties this feature to the others: the features that
					// brought it into the product - such a row stays while they do -
					// and, failing that, what it asks for itself.
					Item {
						id: requirementsCell
						width: featuresPage.columnWidth(5, rowContent.width, rowContent.spacing)
						height: rowContent.height
						visible: width > 0

						property string featureId: rowContent.nodeData ? rowContent.nodeData.id || "" : ""
						property var requiredBy: productViewContainer.requiredByNames(requirementsCell.featureId)
						property var requires: productViewContainer.featureRequirementNames(requirementsCell.featureId)

						Rectangle {
							id: requiredByChip
							anchors.left: parent.left
							anchors.leftMargin: Style.marginXS
							anchors.right: parent.right
							anchors.rightMargin: Style.marginXS
							anchors.verticalCenter: parent.verticalCenter
							height: Style.controlHeightS
							radius: height / 2
							visible: requirementsCell.requiredBy.length > 0
							color: Style.backgroundColor2
							border.color: Style.borderColor
							border.width: 1

							Row {
								anchors.left: parent.left
								anchors.leftMargin: Style.marginS
								anchors.right: parent.right
								anchors.rightMargin: Style.marginS
								anchors.verticalCenter: parent.verticalCenter
								spacing: Style.spacingXS

								Image {
									anchors.verticalCenter: parent.verticalCenter
									width: Style.iconSizeS
									height: width
									source: "qrc:/" + Style.getIconPath("Icons/Lock", Icon.State.On, Icon.Mode.Normal)
									fillMode: Image.PreserveAspectFit
								}

								BaseText {
									anchors.verticalCenter: parent.verticalCenter
									width: requiredByChip.width - Style.iconSizeS - 3 * Style.marginS
									text: qsTr("Required by %1").arg(productViewContainer.namesSummary(requirementsCell.requiredBy))
									font.pixelSize: Style.fontSizeS
									color: Style.titleColor
									elide: Text.ElideRight
								}
							}
						}

						BaseText {
							anchors.left: parent.left
							anchors.leftMargin: Style.marginXS
							anchors.right: parent.right
							anchors.rightMargin: Style.marginXS
							anchors.verticalCenter: parent.verticalCenter
							visible: !requiredByChip.visible
							text: requirementsCell.requires.length > 0
								? qsTr("Requires %1").arg(productViewContainer.namesSummary(requirementsCell.requires)) : "-"
							color: requirementsCell.requires.length > 0 ? Style.subtitleColor : Style.inactiveTextColor
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
					footerText: productViewContainer.selectedFeatureFooterText()
					subtitle: productViewContainer.selectedFeatureName()
					actionText: productViewContainer.optionalEntries().length === 0 ? ""
						: productViewContainer.allOptionalIncluded() ? qsTr("Clear") : qsTr("Take all")
					actionEnabled: productViewContainer.canEdit
					entryTitleProvider: function(entry) { return entry.featureName }
					entrySubtitleProvider: function(entry) { return entry.fullPath }
					entryBadgeProvider: function(entry) { return productViewContainer.entryStateText(entry) }
					entrySearchableTextProvider: function(entry) { return entry.fullPath + " " + entry.featureId }
					entryCheckStateProvider: function(entry) { return productViewContainer.entryIsIncluded(entry) ? Qt.Checked : Qt.Unchecked }
					entryCheckEnabledProvider: function(entry) { return productViewContainer.entryIsChangeable(entry) }
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
				selectedHintText: productViewContainer.featureIsRequired(productViewContainer.selectedRootFeatureId())
					? qsTr("%1 needs this feature; remove it first to drop this one")
						.arg(productViewContainer.requiredByNames(productViewContainer.selectedRootFeatureId()).join(", "))
					: qsTr("Tick the optional parts on the right; the rest always comes with the feature")
				// What this product still owes: features its own ones require but
				// that are not in it (added as soon as the feature set is touched,
				// and by the server on save), and requirements naming a feature
				// the catalog no longer has.
				levelStatusText: productViewContainer.requirementWarningText()
				levelStatusDetails: productViewContainer.requirementWarningDetails()
				editable: productViewContainer.canEdit
				navigationEnabled: false
				// One row at a time: a tick in front of every feature reads as a
				// choice about the product, which it is not - the choices are the
				// optional parts on the right.
				multiSelectEnabled: false
				// A feature another one requires cannot be dropped, so the command
				// says so before it is pressed.
				removeEnabled: !productViewContainer.featureIsRequired(productViewContainer.selectedRootFeatureId())
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
