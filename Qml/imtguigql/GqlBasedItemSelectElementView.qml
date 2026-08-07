// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcolgui 1.0
import imtguigql 1.0

/**
 * GqlBasedItemSelectElementView
 *
 * Extends ItemSelectElementView with GQL-based name resolution and data provider.
 * Uses FilterableSelectGqlDataProvider to:
 *   1. Provide the selectable items list in the popup
 *   2. Resolve item names from IDs when items are set externally
 *
 * Usage:
 *   GqlBasedItemSelectElementView {
 *       label: "Roles"
 *       collectionId: "Roles"
 *       items: [{id: "role-id-1", name: ""}, {id: "role-id-2", name: ""}]
 *   }
 *
 * The component automatically resolves names from the server for items
 * whose name is empty or equals their ID.
 */
ItemSelectElementView {
	id: gqlItemSelectView

	// CollectionId for the GQL data provider
	property string collectionId: ""

	// Ids the popup must never offer, beyond the already selected ones - e.g. the edited role
	// itself, which may not become its own parent.
	property var excludeIds: []

	// Internal GQL data provider for the select popup
	dataProvider: __gqlDataProvider

	// --- Name resolution via FilterableSelectGqlDataProvider ---
	property bool __resolvingNames: false

	FilterableSelectGqlDataProvider {
		id: nameResolver
		collectionId: gqlItemSelectView.collectionId
		multiSelect: true
		pageSize: 100

		onDataChanged: {
			gqlItemSelectView.__resolveItemNames()
		}
	}

	FilterableSelectGqlDataProvider {
		id: __gqlDataProvider
		collectionId: gqlItemSelectView.collectionId
		extraExcludeIds: gqlItemSelectView.excludeIds
		multiSelect: true
	}

	onItemsChanged: {
		if (!gqlItemSelectView.__resolvingNames)
			gqlItemSelectView.__triggerResolveIfNeeded()
	}

	function __triggerResolveIfNeeded() {
		if (!gqlItemSelectView.items || gqlItemSelectView.items.length === 0) return
		var hasUnresolved = false
		for (var i = 0; i < gqlItemSelectView.items.length; i++) {
			var item = gqlItemSelectView.items[i]
			if (!item.name || item.name === item.id) {
				hasUnresolved = true
				break
			}
		}
		if (hasUnresolved) {
			nameResolver.fetch("")
		}
	}

	function __resolveItemNames() {
		var resolverItems = nameResolver.items
		if (!resolverItems || resolverItems.length === 0) return

		var nameMap = ({})
		for (var i = 0; i < resolverItems.length; i++) {
			var ri = resolverItems[i]
			if (ri.id && ri.title && ri.title !== "")
				nameMap[ri.id] = ri.title
		}

		var updated = false
		var newItems = []
		for (var j = 0; j < gqlItemSelectView.items.length; j++) {
			var cur = gqlItemSelectView.items[j]
			var resolved = nameMap[cur.id]
			if (resolved && cur.name !== resolved) {
				newItems.push({ id: cur.id, name: resolved })
				updated = true
			} else {
				newItems.push(cur)
			}
		}
		if (updated) {
			gqlItemSelectView.__resolvingNames = true
			gqlItemSelectView.items = newItems
			gqlItemSelectView.__resolvingNames = false
		}
	}
}
