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
		if (!gqlItemSelectView.__resolvingNames && gqlItemSelectView.hasUnresolvedItems())
			nameResolver.fetch("")
	}

	function __resolveItemNames() {
		gqlItemSelectView.resolveItemNames(nameResolver.items)
	}
}
