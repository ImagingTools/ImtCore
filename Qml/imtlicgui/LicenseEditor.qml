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
	// Part of the product this editor works in; scopes its permission checks.
	property string permissionPath: ""

	// Raised while the page is being filled in from the document - by an open,
	// by Undo, by Redo. Every control that writes back on change has to sit
	// still for that, because the value it is being handed is the document's
	// own. ViewBase's guard cannot be used here: doUpdateGui() raises
	// blockingUpdateModel, while guiIsBlocked() reports blockingUpdateGui, so
	// the combo box read "nothing is blocked", took the -1 it was being reset
	// with for a choice, and cleared the product, the features and the
	// inherited licenses on every Undo.
	property bool applyingGui: false

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
		licenseEditor.applyingGui = true
		var generalPageInstance = multiPageView.getPageByIndex(0)
		if (generalPageInstance)
			generalPageInstance.updateGui()
		licenseEditor.applyingGui = false
		reloadFromDocument()
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

	// Writing the same string back would still count as a change to the
	// document, which is how a plain redraw used to become an undo step.
	function applyFeatureIds(ids) {
		featureIds = ids
		let joined = ids.join(';')
		if (licenseData && (licenseData.m_features || "") !== joined)
			licenseData.m_features = joined
		updatePageBadges()
	}

	function applyParentLicenseIds(ids) {
		parentLicenseIds = ids
		let joined = ids.join(';')
		if (licenseData && (licenseData.m_parentLicenses || "") !== joined)
			licenseData.m_parentLicenses = joined
		rebuildInheritedEntries()
		rebuildLicenseContent()
		updatePageBadges()
	}

	function setProductId(newProductId) {
		if (newProductId === productId)
			return
		productId = newProductId
		if (licenseData)
			licenseData.m_productId = newProductId
		// Both picks belong to the old product, so neither survives the switch.
		explicitPaths = []
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

	// The features this product offers, as document ids. Read once per rebuild
	// and kept, because the requirement closure asks "is this one of ours?"
	// for every feature it walks through.
	property var productFeatureIdList: []
	// Which products offer a given feature, so a feature reached by a
	// requirement can be introduced by where it comes from rather than by a
	// bare identifier.
	property var productNamesByFeature: ({})

	function buildProductIndex() {
		let index = {}
		for (let i = 0; i < CachedProductCollection.collectionModel.getItemsCount(); ++i) {
			let name = CachedProductCollection.collectionModel.getData(ProductItemTypeMetaInfo.s_productName, i) || ""
			let features = CachedProductCollection.collectionModel.getData(ProductItemTypeMetaInfo.s_features, i) || ""
			let list = features === "" ? [] : features.split(';')
			for (let j = 0; j < list.length; ++j) {
				if (list[j] === "")
					continue
				if (!index[list[j]])
					index[list[j]] = []
				if (index[list[j]].indexOf(name) < 0)
					index[list[j]].push(name)
			}
		}
		productNamesByFeature = index
	}

	function productsOffering(rootUuid) {
		return productNamesByFeature[rootUuid] ? productNamesByFeature[rootUuid] : []
	}

	function readProductFeatureIds() {
		for (let i = 0; i < CachedProductCollection.collectionModel.getItemsCount(); ++i) {
			let id = CachedProductCollection.collectionModel.getData(ProductItemTypeMetaInfo.s_id, i)
			if (id !== licenseEditor.productId)
				continue
			let features = CachedProductCollection.collectionModel.getData(ProductItemTypeMetaInfo.s_features, i)
			return features ? features.split(';') : []
		}
		return []
	}

	function productFeatureIds() {
		return productFeatureIdList
	}

	// Re-reads the collections and works the page out again from the picks that
	// are already in hand. Everything a change of collection can affect is
	// covered - names, new sub-features, new requirements - and the picks
	// themselves are left alone, because a notification from the server is not
	// somebody changing their mind.
	function rebuildEntries() {
		buildFeatureIndex()
		buildProductIndex()
		productFeatureIdList = readProductFeatureIds()
		rebuildFeatureNodes()
		recomputeGrants()
		rebuildLicenseEntries()
		rebuildInheritedEntries()
		rebuildLicenseContent()
	}

	// The document arrived - an open, an Undo, a Redo - so the picks come from
	// it rather than from the page. Reading them back and running them through
	// the same rules is what makes a requirement added upstream take effect on
	// opening a license that was saved before it existed.
	//
	// A requirement-driven grant is remembered as a pick here, which is the one
	// place the two differ: within a session, dropping the feature that pulled
	// something in drops it again, whereas a license reopened from the server
	// keeps it and lets it be unticked by hand. Nothing is ever lost that way,
	// which is the trade that matters.
	function reloadFromDocument() {
		refreshIds()
		buildFeatureIndex()
		buildProductIndex()
		productFeatureIdList = readProductFeatureIds()
		deriveExplicitPaths()
		rebuildFeatureNodes()
		recomputeGrants()
		rebuildLicenseEntries()
		rebuildInheritedEntries()
		rebuildLicenseContent()
	}

	// --- the feature collection, indexed by path ---------------------------- //
	//
	// Requirements name a feature by the path of identifiers that leads to it,
	// e.g. "/Administration/EditUser/AddUser" - the same form FeatureEditor
	// writes. The index below is built over the WHOLE collection, not just over
	// this product's features, because a requirement is free to point at a
	// feature this product does not offer and the user has to be told so by
	// name rather than by path.
	//
	// A license stores its picks differently: a whole feature under its document
	// id, an optional part inside one under "<featureUuid>/<subFeatureId>".
	// Everything below works in paths and converts to that form once, on the way
	// into the document.
	property var featureIndex: ({})
	property var rootPathByUuid: ({})
	// leafPathsUnder() is asked the same question by every binding in every
	// visible row, and the answer only changes when the collection does.
	property var leafPathCache: ({})

	function buildFeatureIndex() {
		let index = {}
		let rootPaths = {}
		leafPathCache = {}
		let tree = TreeModelBuilder.fromTreeItemModelByFields(allFeaturesModel, {
			key: FeatureItemTypeMetaInfo.s_id,
			children: FeatureItemTypeMetaInfo.s_subFeatures,
			columns: {
				id: FeatureItemTypeMetaInfo.s_id,
				featureName: FeatureItemTypeMetaInfo.s_featureName,
				featureId: FeatureItemTypeMetaInfo.s_featureId,
				description: FeatureItemTypeMetaInfo.s_description,
				optional: FeatureItemTypeMetaInfo.s_optional,
				requirements: FeatureItemTypeMetaInfo.s_requirements
			}
		})
		for (let i = 0; i < tree.length; ++i) {
			let data = tree[i].data || {}
			let uuid = data.id || ""
			let featureId = data.featureId || ""
			if (uuid === "" || featureId === "")
				continue
			let rootPath = "/" + featureId
			rootPaths[uuid] = rootPath
			indexFeatureNode(tree[i], uuid, rootPath, true, index)
		}
		featureIndex = index
		rootPathByUuid = rootPaths
	}

	function indexFeatureNode(treeNode, rootUuid, path, isRoot, index) {
		let data = treeNode.data || {}
		let sourceChildren = treeNode.children || []
		let childPaths = []
		let requirements = []
		let rawRequirements = data.requirements ? data.requirements.split(';') : []
		for (let r = 0; r < rawRequirements.length; ++r) {
			if (rawRequirements[r] !== "")
				requirements.push(rawRequirements[r])
		}
		index[path] = {
			"path": path,
			"rootUuid": rootUuid,
			"isRoot": isRoot,
			"isLeaf": sourceChildren.length === 0,
			"featureId": data.featureId || "",
			"featureName": data.featureName || data.featureId || "",
			"description": data.description || "",
			"optional": data.optional === true,
			"requirements": requirements,
			"childPaths": childPaths
		}
		for (let i = 0; i < sourceChildren.length; ++i) {
			let childData = sourceChildren[i].data || {}
			let childPath = path + "/" + (childData.featureId || "")
			childPaths.push(childPath)
			indexFeatureNode(sourceChildren[i], rootUuid, childPath, false, index)
		}
	}

	function collectLeafPaths(path, target) {
		let descriptor = featureIndex[path]
		if (!descriptor)
			return
		if (descriptor.isLeaf) {
			if (target.indexOf(path) < 0)
				target.push(path)
			return
		}
		for (let i = 0; i < descriptor.childPaths.length; ++i)
			collectLeafPaths(descriptor.childPaths[i], target)
	}

	// The leaves of a branch are what it actually grants: a feature that owns
	// sub-features is a grouping, and a grouping grants whatever is inside it.
	function leafPathsUnder(path) {
		let cached = leafPathCache[path]
		if (cached !== undefined)
			return cached
		let result = []
		collectLeafPaths(path, result)
		leafPathCache[path] = result
		return result
	}

	// The leaves under a branch that are a decision of their own. A mandatory
	// part is not one: it comes with its feature and cannot be left out.
	function choosableLeafPathsUnder(path) {
		let leaves = leafPathsUnder(path)
		let result = []
		for (let i = 0; i < leaves.length; ++i) {
			let descriptor = featureIndex[leaves[i]]
			if (descriptor && (descriptor.isRoot || descriptor.optional))
				result.push(leaves[i])
		}
		return result
	}


	// "Administration / Users / Add user" - a requirement is written as a path
	// of identifiers, and identifiers are not what the reader picked rows by.
	function displayPath(path) {
		let segments = path.split('/')
		let names = []
		let current = ""
		for (let i = 1; i < segments.length; ++i) {
			current += "/" + segments[i]
			let descriptor = featureIndex[current]
			names.push(descriptor ? descriptor.featureName : segments[i])
		}
		return names.join(" / ")
	}

	function featureNameOfPath(path) {
		let descriptor = featureIndex[path]
		return descriptor ? descriptor.featureName : path
	}

	// The features the license's product offers, as rows for the table: the top
	// level is the product's features, and opening one shows what is inside it.
	// The row carries nothing but a path - what it means is worked out from the
	// index and from what the license currently grants.
	function rebuildFeatureNodes() {
		let nodes = []
		for (let i = 0; i < productFeatureIdList.length; ++i) {
			let rootPath = rootPathByUuid[productFeatureIdList[i]]
			if (rootPath === undefined)
				continue
			nodes.push(buildFeatureNode(rootPath))
		}
		featureNodes = nodes
	}

	function buildFeatureNode(path) {
		let descriptor = featureIndex[path]
		let children = []
		for (let i = 0; i < descriptor.childPaths.length; ++i)
			children.push(buildFeatureNode(descriptor.childPaths[i]))
		return {
			"key": path,
			"text": descriptor.featureName,
			"children": children,
			"data": {
				"entry": {
					"path": path,
					"rootUuid": descriptor.rootUuid,
					"isRoot": descriptor.isRoot,
					"isLeaf": descriptor.isLeaf,
					"optional": descriptor.optional,
					"featureName": descriptor.featureName,
					"featureId": descriptor.featureId,
					"description": descriptor.description,
					"subFeatureCount": children.length
				}
			}
		}
	}

	// --- what the license grants -------------------------------------------- //
	//
	// explicitPaths is what the user ticked. Everything else is worked out from
	// it: the parts that come with a feature, the features pulled in by
	// requirements, and the requirements of those in turn.
	property var explicitPaths: []
	// Leaf paths this license ends up granting, and why - grantedReasons maps a
	// leaf path to { requirers: [{name, rootUuid}], chain: [names] }. A leaf
	// with requirers cannot be unticked while they are in the license.
	property var grantedLeafPaths: []
	property var grantedLeafSet: ({})
	property var grantedReasons: ({})
	// Requirements whose path leads nowhere at all - the feature they name has
	// been deleted or renamed. A requirement pointing at another product is not
	// one of these: that is ordinary, and it is simply granted.
	property var unresolvedRequirements: []
	// Grants this license carries for features its product no longer offers.
	// Kept as they are and reported, because no row on this page owns them.
	property var orphanGrantIds: []

	// The document keeps grant ids, not picks. Turning them back into picks is
	// what lets a saved license be edited with the same rules as a fresh one.
	function deriveExplicitPaths() {
		let result = []
		for (let i = 0; i < featureIds.length; ++i) {
			let grantId = featureIds[i]
			if (grantId === "" || grantId.indexOf('/') >= 0)
				continue
			let rootPath = rootPathByUuid[grantId]
			if (rootPath === undefined)
				continue
			// Only what the Features page can show is treated as a pick. A
			// feature outside this product has no row, so calling it a pick
			// would make it impossible to ever get rid of; it is left to the
			// requirement that pulled it in, or carried over untouched.
			if (productFeatureIdList.indexOf(grantId) < 0)
				continue
			let choosable = choosableLeafPathsUnder(rootPath)
			let picked = []
			for (let j = 0; j < choosable.length; ++j) {
				let descriptor = featureIndex[choosable[j]]
				if (descriptor.isRoot || featureIds.indexOf(grantId + "/" + descriptor.featureId) >= 0)
					picked.push(choosable[j])
			}
			// A feature that is in the license without any of its optional parts
			// picked still grants its mandatory ones, so the feature itself is
			// the pick that has to be remembered.
			if (picked.length === 0)
				result.push(rootPath)
			else
				result = result.concat(picked)
		}
		explicitPaths = result
	}

	function setExplicitPaths(paths) {
		explicitPaths = paths
		recomputeGrants()
		rebuildLicenseContent()
	}

	// Grants a leaf and records why. requirerName is empty when the leaf is
	// there because it was ticked or because it comes with its feature.
	function closureAddLeaf(state, leafPath, requirer, chain) {
		let descriptor = featureIndex[leafPath]
		if (!descriptor)
			return
		if (!state.reasons[leafPath])
			state.reasons[leafPath] = { "requirers": [], "chain": chain }
		if (requirer) {
			let requirers = state.reasons[leafPath].requirers
			let known = false
			for (let i = 0; i < requirers.length; ++i) {
				if (requirers[i].path === requirer.path)
					known = true
			}
			if (!known)
				requirers.push(requirer)
		}
		if (state.granted[leafPath] === true)
			return
		state.granted[leafPath] = true
		state.queue.push(leafPath)
	}

	// A feature is granted whole or not at all: the moment anything inside it is
	// in the license, its mandatory parts are in too, and their own requirements
	// come with them.
	function closureAddRoot(state, rootUuid) {
		if (state.roots[rootUuid] === true)
			return
		state.roots[rootUuid] = true
		let rootPath = rootPathByUuid[rootUuid]
		if (rootPath === undefined)
			return
		let leaves = leafPathsUnder(rootPath)
		for (let i = 0; i < leaves.length; ++i) {
			let descriptor = featureIndex[leaves[i]]
			if (descriptor && !descriptor.isRoot && !descriptor.optional)
				closureAddLeaf(state, leaves[i], null, [])
		}
	}

	// A requirement may name a grouping, in which case everything inside it is
	// required. It may also name a feature of another product entirely, which is
	// an ordinary thing for a requirement to do: the license simply grants that
	// feature too, and says where it came from. The only requirement this page
	// cannot honour is one whose path leads nowhere at all.
	function closureRequire(state, path, requirer, chain) {
		let descriptor = featureIndex[path]
		if (!descriptor) {
			closureNoteUnresolved(state, path, requirer, chain)
			return
		}
		let leaves = leafPathsUnder(path)
		for (let i = 0; i < leaves.length; ++i)
			closureAddLeaf(state, leaves[i], requirer, chain)
	}

	function closureNoteUnresolved(state, path, requirer, chain) {
		if (!state.unresolved[path]) {
			state.unresolved[path] = {
				"path": path,
				"requirers": [],
				"chain": chain
			}
			state.unresolvedOrder.push(path)
		}
		if (requirer) {
			let requirers = state.unresolved[path].requirers
			let known = false
			for (let i = 0; i < requirers.length; ++i) {
				if (requirers[i].path === requirer.path)
					known = true
			}
			if (!known)
				requirers.push(requirer)
		}
	}

	// Features a requirement reached outside this product's own list, in the
	// order the walk found them. They have no row on the Features page - there
	// is nothing to decide about them - but they are part of the license.
	function grantedRootsOutsideProduct(state) {
		let roots = []
		let leaves = Object.keys(state.granted)
		for (let i = 0; i < leaves.length; ++i) {
			let descriptor = featureIndex[leaves[i]]
			if (!descriptor || productFeatureIdList.indexOf(descriptor.rootUuid) >= 0)
				continue
			if (roots.indexOf(descriptor.rootUuid) < 0)
				roots.push(descriptor.rootUuid)
		}
		return roots
	}

	// Walks out from what was ticked until nothing new is reached: A pulls in B,
	// B pulls in C and D, and every one of them arrives with the chain that
	// brought it, so the table can say what it is here for.
	function recomputeGrants() {
		let state = {
			"granted": {},
			"roots": {},
			"heldRoots": {},
			"reasons": {},
			"unresolved": {},
			"unresolvedOrder": [],
			"queue": []
		}
		for (let i = 0; i < explicitPaths.length; ++i) {
			let descriptor = featureIndex[explicitPaths[i]]
			if (!descriptor)
				continue
			// A feature picked with none of its optional parts is still in the
			// license, so its presence is held here rather than read off a leaf.
			state.heldRoots[descriptor.rootUuid] = true
			let leaves = leafPathsUnder(explicitPaths[i])
			for (let j = 0; j < leaves.length; ++j)
				closureAddLeaf(state, leaves[j], null, [])
		}

		let head = 0
		while (head < state.queue.length) {
			let leafPath = state.queue[head++]
			let descriptor = featureIndex[leafPath]
			if (!descriptor)
				continue
			closureAddRoot(state, descriptor.rootUuid)
			let reason = state.reasons[leafPath]
			let chain = (reason && reason.chain ? reason.chain : []).concat([descriptor.featureName])
			let requirer = {
				"path": leafPath,
				"name": descriptor.featureName,
				"rootUuid": descriptor.rootUuid
			}
			for (let r = 0; r < descriptor.requirements.length; ++r)
				closureRequire(state, descriptor.requirements[r], requirer, chain)
		}

		// Out into the form the document keeps: the product's own features first,
		// in table order, then whatever the requirements reached beyond them.
		let ids = []
		let grantedLeaves = []
		let emitted = {}
		let ordered = productFeatureIdList.concat(grantedRootsOutsideProduct(state))
		for (let p = 0; p < ordered.length; ++p) {
			let rootUuid = ordered[p]
			if (emitted[rootUuid] === true)
				continue
			emitted[rootUuid] = true
			let rootPath = rootPathByUuid[rootUuid]
			if (rootPath === undefined)
				continue
			let leaves = leafPathsUnder(rootPath)
			let optionalIds = []
			let anyGranted = false
			for (let i = 0; i < leaves.length; ++i) {
				if (state.granted[leaves[i]] !== true)
					continue
				anyGranted = true
				grantedLeaves.push(leaves[i])
				let descriptor = featureIndex[leaves[i]]
				if (descriptor && !descriptor.isRoot && descriptor.optional)
					optionalIds.push(rootUuid + "/" + descriptor.featureId)
			}
			if (!anyGranted && state.heldRoots[rootUuid] !== true)
				continue
			ids.push(rootUuid)
			ids = ids.concat(optionalIds)
		}

		// A grant that nothing on this page accounts for any more - the product
		// dropped the feature, or the requirement that once pulled it in is
		// gone - is kept exactly as it stands. There is no row to work it out
		// from, and dropping it would quietly take a capability away from a
		// license nobody asked to change.
		let orphans = []
		for (let o = 0; o < featureIds.length; ++o) {
			let grantId = featureIds[o]
			if (grantId === "" || ids.indexOf(grantId) >= 0)
				continue
			let separator = grantId.indexOf('/')
			let rootUuid = separator < 0 ? grantId : grantId.substring(0, separator)
			if (productFeatureIdList.indexOf(rootUuid) >= 0)
				continue
			orphans.push(grantId)
			ids.push(grantId)
		}
		orphanGrantIds = orphans

		grantedLeafPaths = grantedLeaves
		grantedLeafSet = state.granted
		grantedReasons = state.reasons
		let unresolved = []
		for (let m = 0; m < state.unresolvedOrder.length; ++m)
			unresolved.push(state.unresolved[state.unresolvedOrder[m]])
		unresolvedRequirements = unresolved
		applyFeatureIds(ids)
	}

	// --- what a row means ---------------------------------------------------- //

	function pathIsGranted(path) {
		return grantedLeafSet[path] === true
	}

	// Requirers of everything under a branch, minus the ones that live inside
	// the branch itself: a feature is not held in place by its own parts, so a
	// feature whose sub-features require each other can still be removed whole.
	function entryRequiredBy(entry) {
		if (!entry)
			return []
		let leaves = leafPathsUnder(entry.path)
		let names = []
		for (let i = 0; i < leaves.length; ++i) {
			if (!pathIsGranted(leaves[i]))
				continue
			let reason = grantedReasons[leaves[i]]
			if (!reason)
				continue
			for (let j = 0; j < reason.requirers.length; ++j) {
				let requirer = reason.requirers[j]
				if (requirer.path === entry.path || requirer.path.indexOf(entry.path + "/") === 0)
					continue
				if (names.indexOf(requirer.name) < 0)
					names.push(requirer.name)
			}
		}
		return names
	}

	// The chain that brought a row in: "A -> B -> C" reads back as "C is here
	// because A needs B and B needs C".
	function entryRequirementChain(entry) {
		if (!entry)
			return ""
		let leaves = leafPathsUnder(entry.path)
		for (let i = 0; i < leaves.length; ++i) {
			let reason = grantedReasons[leaves[i]]
			if (reason && reason.requirers.length > 0 && reason.chain && reason.chain.length > 0)
				return reason.chain.join(" → ")
		}
		return ""
	}

	function entryIsGranted(entry) {
		if (!entry)
			return false
		let leaves = leafPathsUnder(entry.path)
		for (let i = 0; i < leaves.length; ++i) {
			if (pathIsGranted(leaves[i]))
				return true
		}
		return false
	}

	function entryLeafCount(entry) {
		return entry ? leafPathsUnder(entry.path).length : 0
	}

	function entryGrantedLeafCount(entry) {
		if (!entry)
			return 0
		let leaves = leafPathsUnder(entry.path)
		let granted = 0
		for (let i = 0; i < leaves.length; ++i) {
			if (pathIsGranted(leaves[i]))
				++granted
		}
		return granted
	}

	// A grouping is ticked when everything inside it is, and half-ticked when
	// only part of it is: the tick on a feature is a reading of its leaves
	// rather than a decision of its own.
	function entryCheckState(entry) {
		if (!entry)
			return Qt.Unchecked
		let leaves = leafPathsUnder(entry.path)
		if (leaves.length === 0)
			return Qt.Unchecked
		let granted = 0
		for (let i = 0; i < leaves.length; ++i) {
			if (pathIsGranted(leaves[i]))
				++granted
		}
		if (granted === 0)
			return Qt.Unchecked
		return granted === leaves.length ? Qt.Checked : Qt.PartiallyChecked
	}

	// Only the leaves of the tree are a decision, plus the feature itself, which
	// is how a whole feature is taken or dropped in one go. A grouping that
	// holds nothing optional carries no decision at all - it is in exactly when
	// its feature is.
	function entryIsChangeable(entry) {
		if (!canEdit || !entry)
			return false
		if (entryRequiredBy(entry).length > 0)
			return false
		if (entry.isRoot)
			return true
		if (entry.isLeaf)
			return entry.optional
		return choosableLeafPathsUnder(entry.path).length > 0
	}

	function toggleEntry(entry) {
		if (!entryIsChangeable(entry))
			return
		let next = []
		if (entryCheckState(entry) === Qt.Checked) {
			// Dropping a branch drops everything picked inside it, otherwise a
			// pick would linger under something that is no longer granted.
			for (let i = 0; i < explicitPaths.length; ++i) {
				let path = explicitPaths[i]
				if (path !== entry.path && path.indexOf(entry.path + "/") !== 0)
					next.push(path)
			}
		}
		else {
			next = explicitPaths.slice()
			let targets = choosableLeafPathsUnder(entry.path)
			// Nothing inside is optional, so taking the branch is the pick.
			if (targets.length === 0)
				targets = [entry.path]
			for (let j = 0; j < targets.length; ++j) {
				if (next.indexOf(targets[j]) < 0)
					next.push(targets[j])
			}
		}
		setExplicitPaths(next)
	}

	// Why a row is the way it is, in the words the table shows next to it.
	function entryStateText(entry) {
		if (!entry)
			return ""
		let requiredBy = entryRequiredBy(entry)
		if (requiredBy.length > 0)
			return qsTr("Required by %1").arg(requiredBy.join(", "))
		if (entry.isLeaf && !entry.isRoot && !entry.optional)
			return qsTr("Always included")
		if (!entry.isLeaf && !entry.isRoot && choosableLeafPathsUnder(entry.path).length === 0)
			return qsTr("Always included")
		return ""
	}

	// Counts on the page panel: how much of the product this license grants, and
	// whether anything is still owed to it.
	function updatePageBadges() {
		let granted = 0
		for (let i = 0; i < featureIds.length; ++i) {
			if (featureIds[i] !== "" && featureIds[i].indexOf('/') < 0)
				++granted
		}
		multiPageView.setPageBadge("Features", unresolvedRequirements.length > 0
			? granted + " / " + featureNodes.length + " !"
			: featureNodes.length > 0 ? granted + " / " + featureNodes.length : "")
		multiPageView.setPageBadge("Inheritance", parentLicenseIds.length > 0
			? "" + parentLicenseIds.length : "")
		multiPageView.setPageBadge("General", licenseContentEntries.length > 0
			? "" + licenseContentEntries.length : "")
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

	function featureIsIncluded(grantId) {
		return featureIds.indexOf(grantId) >= 0
	}

	// A grant id read back as something a person can recognise: a feature by its
	// name, a part inside one by the path of names that leads to it.
	function grantDisplayName(grantId) {
		let separator = grantId.indexOf('/')
		let rootUuid = separator < 0 ? grantId : grantId.substring(0, separator)
		let rootPath = rootPathByUuid[rootUuid]
		if (rootPath === undefined)
			return grantId
		if (separator < 0)
			return featureIndex[rootPath].featureName
		let subFeatureId = grantId.substring(separator + 1)
		let leaves = leafPathsUnder(rootPath)
		for (let i = 0; i < leaves.length; ++i) {
			if (featureIndex[leaves[i]].featureId === subFeatureId)
				return displayPath(leaves[i])
		}
		return featureIndex[rootPath].featureName + " / " + subFeatureId
	}

	function grantFeatureCode(grantId) {
		let separator = grantId.indexOf('/')
		if (separator >= 0)
			return grantId.substring(separator + 1)
		let rootPath = rootPathByUuid[grantId]
		return rootPath === undefined ? "" : featureIndex[rootPath].featureId
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
			let grantId = order[i]
			entries.push({
				"id": grantId,
				"featureName": grantDisplayName(grantId),
				"featureId": grantFeatureCode(grantId),
				"licenseName": origins[grantId]
			})
		}
		inheritedEntries = entries
	}

	// --- everything this license will contain -------------------------------- //
	//
	// Ticks on the Features page are only one of the ways a feature ends up in a
	// license: a requirement can bring one in, and so can an inherited license.
	// One list answers "what does this license actually grant" for the General
	// page and for the panel beside the features table, so the two can never
	// disagree with each other.
	property var licenseContentEntries: []

	readonly property string sectionPicked: qsTr("Picked here")
	readonly property string sectionRequired: qsTr("Added by requirements")
	readonly property string sectionInherited: qsTr("Inherited")
	readonly property string sectionUnresolved: qsTr("Requirements that lead nowhere")
	readonly property string sectionOrphan: qsTr("Kept from an earlier state")

	// A pick can be a whole branch, so a leaf counts as picked when it or
	// anything it hangs below was ticked.
	function pathIsExplicit(path) {
		for (let i = 0; i < explicitPaths.length; ++i) {
			let explicit = explicitPaths[i]
			if (explicit === path || path.indexOf(explicit + "/") === 0)
				return true
		}
		return false
	}

	function requirerNames(requirers) {
		let names = []
		for (let i = 0; i < requirers.length; ++i) {
			if (names.indexOf(requirers[i].name) < 0)
				names.push(requirers[i].name)
		}
		return names
	}

	// A requirement whose path names no feature at all. The path is shown as it
	// stands, because there is nothing left to translate it into.
	function unresolvedRequirementDetail(entry) {
		return qsTr("Required by %1, but no feature has this path any more")
			.arg(requirerNames(entry.requirers).join(", "))
	}

	// Where a feature came from, for one that this product does not list itself.
	// Belonging to another product is an ordinary thing for a requirement to
	// reach into, so this is a note about origin and not a complaint.
	function originNote(rootUuid) {
		let products = productsOffering(rootUuid)
		if (products.length === 0)
			return qsTr("not offered by any product")
		return qsTr("from %1").arg(products.join(", "))
	}

	function rebuildLicenseContent() {
		let picked = []
		let required = []
		for (let i = 0; i < grantedLeafPaths.length; ++i) {
			let path = grantedLeafPaths[i]
			let descriptor = featureIndex[path]
			if (!descriptor)
				continue
			let reason = grantedReasons[path]
			let requirers = reason ? reason.requirers : []
			let isPicked = pathIsExplicit(path) || requirers.length === 0
			if (isPicked) {
				picked.push({
					"key": "picked:" + path,
					"section": licenseEditor.sectionPicked,
					"title": displayPath(path),
					"subtitle": descriptor.featureId,
					"badge": !descriptor.isRoot && !descriptor.optional ? qsTr("Always included") : "",
					"tone": "neutral"
				})
			}
			else {
				let chain = reason && reason.chain && reason.chain.length > 0 ? reason.chain.join(" → ")
					: requirerNames(requirers).join(", ")
				// A feature this product does not list is reached through a
				// requirement like any other; it just says where it came from,
				// because there is no row for it on the Features page.
				let fromElsewhere = productFeatureIdList.indexOf(descriptor.rootUuid) < 0
				required.push({
					"key": "required:" + path,
					"section": licenseEditor.sectionRequired,
					"title": displayPath(path),
					"subtitle": fromElsewhere
						? qsTr("Required by %1 · %2").arg(chain).arg(originNote(descriptor.rootUuid))
						: qsTr("Required by %1").arg(chain),
					"badge": qsTr("Requirement"),
					"tone": "accent"
				})
			}
		}

		let inherited = []
		for (let j = 0; j < inheritedEntries.length; ++j) {
			let entry = inheritedEntries[j]
			inherited.push({
				"key": "inherited:" + entry.id,
				"section": licenseEditor.sectionInherited,
				"title": entry.featureName,
				"subtitle": qsTr("from %1").arg(entry.licenseName),
				"badge": isAlsoGrantedHere(entry.id) ? qsTr("Also granted here") : "",
				"tone": "neutral"
			})
		}

		let unresolved = []
		for (let k = 0; k < unresolvedRequirements.length; ++k) {
			let entry = unresolvedRequirements[k]
			unresolved.push({
				"key": "unresolved:" + entry.path,
				"section": licenseEditor.sectionUnresolved,
				"title": entry.path,
				"subtitle": unresolvedRequirementDetail(entry),
				"badge": qsTr("Unknown feature"),
				"tone": "warning"
			})
		}

		let orphans = []
		for (let o = 0; o < orphanGrantIds.length; ++o) {
			let grantId = orphanGrantIds[o]
			orphans.push({
				"key": "orphan:" + grantId,
				"section": licenseEditor.sectionOrphan,
				"title": grantDisplayName(grantId),
				"subtitle": qsTr("Granted, but nothing on this page accounts for it any more - it stays until it is removed by hand"),
				"badge": qsTr("Kept as it was"),
				"tone": "warning"
			})
		}

		licenseContentEntries = picked.concat(required).concat(inherited).concat(unresolved).concat(orphans)
		updatePageBadges()
	}

	function unresolvedRequirementDetails() {
		let details = []
		for (let i = 0; i < unresolvedRequirements.length; ++i)
			details.push(unresolvedRequirements[i].path + " - " + unresolvedRequirementDetail(unresolvedRequirements[i]))
		return details
	}

	// Granted twice: once by this license and once by a license it inherits.
	function isAlsoGrantedHere(grantId) {
		return featureIds.indexOf(grantId) >= 0
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
							// updateGui() clears the box before it puts the stored
							// product back, and that clearing step is not a choice
							// anybody made - acting on it dropped the product, the
							// features and the inherited licenses on every Undo.
							onCurrentIndexChanged: {
								if (licenseEditor.applyingGui || licenseEditor.guiIsBlocked())
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

					// Everything this license grants, however it got there: the
					// ticks from the Features page, whatever their requirements
					// pulled in, and whatever the inherited licenses bring. The
					// decisions are made elsewhere - this is the answer to "so
					// what is in it", which is the question the General page is
					// otherwise silent about.
					GroupHeaderView {
						width: parent.width
						title: qsTr("In this license")
					}

					Rectangle {
						width: parent.width
						height: contentColumn.height + 2 * Style.marginL
						radius: Style.marginXS
						color: Style.baseColor
						border.width: 1
						border.color: Style.borderColor

						Column {
							id: contentColumn
							anchors.top: parent.top
							anchors.topMargin: Style.marginL
							anchors.left: parent.left
							anchors.leftMargin: Style.marginL
							anchors.right: parent.right
							anchors.rightMargin: Style.marginL
							spacing: Style.spacingS

							BaseText {
								width: parent.width
								visible: licenseEditor.licenseContentEntries.length === 0
								text: licenseEditor.productId === ""
									? qsTr("Pick a product above, then tick its features on the Features page")
									: qsTr("Nothing granted yet - tick features on the Features page")
								color: Style.inactiveTextColor
								wrapMode: Text.WordWrap
							}

							Repeater {
								model: licenseEditor.licenseContentEntries

								Column {
									id: contentRow
									width: contentColumn.width
									spacing: Style.spacingS

									property bool startsSection: index === 0
										|| licenseEditor.licenseContentEntries[index - 1].section !== modelData.section

									Item {
										width: parent.width
										height: visible ? Style.controlHeightM : 0
										visible: contentRow.startsSection

										BaseText {
											anchors.left: parent.left
											anchors.bottom: parent.bottom
											anchors.bottomMargin: Style.marginXS
											text: modelData.section
											font.family: Style.fontFamilyBold
											font.pixelSize: Style.fontSizeS
											color: modelData.tone === "warning" ? Style.secondColor : Style.subtitleColor
										}

										Rectangle {
											anchors.left: parent.left
											anchors.right: parent.right
											anchors.bottom: parent.bottom
											height: 1
											color: Style.borderColor
											opacity: 0.6
										}
									}

									Row {
										width: parent.width
										spacing: Style.spacingS

										Rectangle {
											anchors.verticalCenter: parent.verticalCenter
											width: Style.marginXS
											height: Style.marginXS
											radius: width / 2
											color: modelData.tone === "warning" ? Style.secondColor
												: modelData.tone === "accent" ? Style.titleColor : Style.borderColor
										}

										Column {
											width: parent.width - Style.marginXS - parent.spacing
											spacing: Style.spacingXXS

											Row {
												width: parent.width
												spacing: Style.spacingS

												BaseText {
													anchors.verticalCenter: parent.verticalCenter
													width: Math.max(0, parent.width - rowBadge.width - parent.spacing)
													text: modelData.title
													color: Style.textColor
													elide: Text.ElideRight
												}

												Rectangle {
													id: rowBadge
													anchors.verticalCenter: parent.verticalCenter
													width: modelData.badge === "" ? 0 : rowBadgeText.width + 2 * Style.marginS
													height: Style.controlHeightS
													radius: height / 2
													visible: modelData.badge !== ""
													color: modelData.tone === "warning" ? Style.popupWarningBackgroundColor
														: modelData.tone === "accent" ? Style.selectedColor : Style.backgroundColor2
													border.width: 1
													border.color: modelData.tone === "warning" ? Style.secondColor
														: modelData.tone === "accent" ? Style.titleColor : Style.borderColor

													BaseText {
														id: rowBadgeText
														anchors.centerIn: parent
														text: modelData.badge
														font.pixelSize: Style.fontSizeS
														color: modelData.tone === "warning" ? Style.secondColor
															: modelData.tone === "accent" ? Style.titleColor : Style.subtitleColor
													}
												}
											}

											BaseText {
												width: parent.width
												visible: text !== ""
												text: modelData.subtitle
												font.pixelSize: Style.fontSizeS
												color: Style.subtitleColor
												wrapMode: Text.WordWrap
											}
										}
									}
								}
							}
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
				licenseEditor.reloadFromDocument()
			}

			function updateModel() {
			}

			// Column geometry, shared by the header, the rows and the drag handles
			// the explorer draws, so none of the three can drift from the others.
			// A column whose breakpoint is above the current table width folds
			// away and its share is handed to the columns that stay.
			// The first column carries the tick and, beside it, the reason a tick
			// cannot be cleared - which is a sentence, not a glyph.
			TableColumnLayout {
				id: featureColumns
				fractions: [0.20, 0.26, 0.15, 0.25, 0.14]
				breakpoints: [0, 0, 380, 620, 300]
			}

			function columnVisible(index, width) {
				return featureColumns.isVisible(index, width)
			}

			function columnWidth(index, width, spacing) {
				return featureColumns.widthOf(index, width, spacing)
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

					// Every row carries a box, including the groupings: a feature
					// that owns sub-features is ticked when all of them are and
					// half-ticked when only some are, so the state of a whole
					// branch is readable without opening it, and one click takes
					// or drops the branch entire.
					Item {
						id: stateCell
						width: featuresPage.columnWidth(0, rowContent.width, rowContent.spacing)
						height: rowContent.height
						visible: width > 0

						property bool changeable: licenseEditor.entryIsChangeable(rowContent.entry)
						property string stateText: licenseEditor.entryStateText(rowContent.entry)

						CheckBox {
							id: stateCheck
							anchors.left: parent.left
							anchors.leftMargin: Style.marginXS
							anchors.verticalCenter: parent.verticalCenter
							visible: rowContent.entry !== null
							isActive: stateCell.changeable
							tristate: true
							checkState: licenseEditor.entryCheckState(rowContent.entry)
							mouseArea.enabled: false
						}

						// A box that cannot be cleared says why right beside
						// itself, rather than leaving the reader to guess which
						// other row is holding it down.
						BaseText {
							anchors.left: stateCheck.right
							anchors.leftMargin: Style.marginS
							anchors.right: parent.right
							anchors.rightMargin: Style.marginXS
							anchors.verticalCenter: parent.verticalCenter
							visible: stateCell.stateText !== ""
							text: stateCell.stateText
							font.pixelSize: Style.fontSizeS
							color: licenseEditor.entryRequiredBy(rowContent.entry).length > 0
								? Style.titleColor : Style.inactiveTextColor
							elide: Text.ElideRight
						}

						// Stays live even when the row cannot be ticked: hovering a
						// box that is held down is how the reader finds out what
						// is holding it.
						MouseArea {
							id: stateMouse
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: stateCell.changeable ? Qt.PointingHandCursor : Qt.ArrowCursor
							onClicked: licenseEditor.toggleEntry(rowContent.entry)
						}

						TooltipArea {
							anchors.fill: parent
							mouseArea: stateMouse
							text: licenseEditor.entryRequirementChain(rowContent.entry) !== ""
								? qsTr("Pulled in by %1").arg(licenseEditor.entryRequirementChain(rowContent.entry))
								: stateCell.stateText
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
						// Opening a feature is how the parts inside it are reached,
						// and they are what is actually picked - so a feature opens
						// whether or not anything in it is granted yet.
						property bool openable: subfeaturesCell.count > 0
						// "2 / 5" rather than "5": a feature is a grouping, so what
						// this column has to answer is how much of the grouping is
						// in the license, without the reader having to open it.
						property int leafTotal: licenseEditor.entryLeafCount(rowContent.entry)
						property int leafGranted: licenseEditor.entryGrantedLeafCount(rowContent.entry)

						Rectangle {
							anchors.centerIn: parent
							width: Math.max(Style.controlHeightS, countText.width + 2 * Style.marginS)
							height: Style.controlHeightS
							radius: height / 2
							visible: subfeaturesCell.count > 0
							color: subfeaturesMouse.containsMouse ? Style.titleColor
								: subfeaturesCell.leafGranted > 0 ? Style.selectedColor : Style.backgroundColor2
							border.color: subfeaturesCell.leafGranted > 0 ? Style.titleColor : Style.borderColor
							border.width: 1

							BaseText {
								id: countText
								anchors.centerIn: parent
								text: subfeaturesCell.leafGranted + " / " + subfeaturesCell.leafTotal
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

			// Right-hand table of the page: everything this license ends up
			// granting, whichever way it got there. Read-only - the ticks are on
			// the left, the inheritance on its own page - but it is where the
			// consequences of both are spelled out, including the requirements
			// that lead out of this product and cannot be met here at all.
			property Component licenseContentPanelComp: Component {
				CheckableListPanel {
					title: qsTr("In this license")
					subtitle: licenseEditor.unresolvedRequirements.length > 0
						? qsTr("%1 requirements lead nowhere").arg(licenseEditor.unresolvedRequirements.length)
						: licenseEditor.licenseContentEntries.length > 0
							? qsTr("%1 entries").arg(licenseEditor.licenseContentEntries.length) : ""
					model: licenseEditor.licenseContentEntries
					contentActive: licenseEditor.productId !== ""
					revealKey: licenseEditor.productId
					placeholderText: qsTr("No product chosen")
					placeholderDescription: qsTr("Pick a product on the General page first")
					nameColumnTitle: qsTr("Feature")
					iconSource: "Icons/FeatureList"
					searchPlaceholder: qsTr("Search license content")
					emptyText: qsTr("Nothing granted yet")
					emptyDescription: qsTr("Tick a feature on the left, or inherit a license, and what it brings appears here")
					footerText: qsTr("Ticked here, pulled in by a requirement, or inherited - all of it ends up in the license")
					entrySectionProvider: function(entry) { return entry.section }
					entryTitleProvider: function(entry) { return entry.title }
					entrySubtitleProvider: function(entry) { return entry.subtitle }
					entryBadgeProvider: function(entry) { return entry.badge }
					entryBadgeToneProvider: function(entry) { return entry.tone }
					entrySearchableTextProvider: function(entry) { return entry.title + " " + entry.subtitle + " " + entry.badge }
					// A requirement that cannot be met is the one row here that
					// is not a grant, so it is the one row without a tick.
					entryCheckStateProvider: function(entry) {
						return entry.section === licenseEditor.sectionUnresolved ? Qt.Unchecked : Qt.Checked
					}
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
				idleHintText: qsTr("Tick what this license grants; a feature is a grouping, so ticking one takes everything inside it")
				selectedHintText: qsTr("Open a feature to pick its parts one by one")
				// A requirement reaching into another product is ordinary and is
				// simply granted. One whose path names nothing at all is not,
				// and is reported the way an unfinished level is: a chip on the
				// path, details on hover.
				levelStatusText: licenseEditor.unresolvedRequirements.length > 0
					? qsTr("%1 requirements lead nowhere").arg(licenseEditor.unresolvedRequirements.length) : ""
				levelStatusDetails: licenseEditor.unresolvedRequirementDetails()
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
				columnLayout: featureColumns
				sidePanelComponent: featuresPage.licenseContentPanelComp
				// A feature opens whenever it holds anything: what is inside it
				// is what gets picked, so it has to be reachable before anything
				// in it is granted.
				containerProvider: function(node) {
					let entry = licenseEditor.nodeEntry(node)
					return entry !== null && entry.subFeatureCount > 0
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
				licenseEditor.reloadFromDocument()
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
