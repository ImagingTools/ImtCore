// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15

/**
	AssistantRepresentationRegistry

	Registry of the QML components that render structured assistant results.
	A result is shown only by a component registered for its type and version;
	content delivered by the model is never loaded as QML, JavaScript or HTML.

	A registered component must provide:
	- function setRepresentationData(data): validates and shows the data, returns false if invalid;
	- signal selectionHandedOver(string contextKind, string contextLabel, var contextData) (optional).

	The type and version must match a representation registered on the server
	(imtassist::IUiRepresentationRegistry).
*/
QtObject {
	id: root

	property var components: ({})

	function createKey(type, version){
		return type + "@" + version
	}

	function registerComponent(type, version, component){
		if (type === "" || version <= 0 || component === null){
			return false
		}

		let components = Object.assign({}, root.components)
		components[root.createKey(type, version)] = component
		root.components = components

		return true
	}

	function findComponent(type, version){
		let component = root.components[root.createKey(type, version)]

		return component === undefined ? null : component
	}
}
