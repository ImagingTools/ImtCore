import QtQuick

/*!
	\qmltype NavigableItem
	\inqmlmodule Custom
	\brief Provides hierarchical navigation handling for path-based routing.

	The Navigable item is used in combination with \c NavigationController to
	handle application navigation in a hierarchical, path-segmented manner.

	Each Navigable registers one or more \c paths that it can respond to.
	When a navigation request is emitted, Navigable instances check if they
	match the first segment of the path. If matched, the \c activated signal
	is emitted, and the remaining path can optionally be forwarded deeper
	into the navigation tree.

	Typical usage:
	\qml
	NavigableItem {
		paths: ["Orders"]
		onActivated: (params, restPath, matchedPath) => {
			console.log("Orders section activated")
		}
	}

	NavigableItem {
		paths: ["Order"]
		parentSegment: "Orders"
		onActivated: (params, restPath, matchedPath) => {
			console.log("Order activated with id:", restPath[0])
		}
	}
	\endqml

	\sa NavigationController
*/
Item {
	id: root

	/*!
		List of path segments this Navigable can handle.
	*/
	property var paths: []

	/*!
		Optional parent segment name. If set, this Navigable
		will only activate when the parent segment has already
		been activated in the current navigation path.
	*/
	property string parentSegment: ""

	/*!
		If true (default), the remaining path segments will be
		forwarded to other Navigable items for further handling.
		If false, navigation will stop at this level.
	*/
	property bool forwardRest: true

	/*!
		Emitted when this Navigable is activated.

		\param params      Arbitrary parameters passed with navigation
		\param restPath    Remaining path segments after this Navigable
		\param matchedPath The path segment that matched this Navigable
	*/
	signal activated(var params, var restPath, string matchedPath)

	Component.onCompleted: {
		NavigationController.navigatePath.connect(processPath)
	}

	Component.onDestruction: {
		NavigationController.navigatePath.disconnect(processPath)
	}

	function processPath(path, params, activeSegments, markHandled) {
		if (path.length === 0)
			return

		if (parentSegment !== "" && activeSegments.indexOf(parentSegment) === -1)
			return

		for (let i = 0; i < paths.length; i++) {
			let pattern = paths[i]
			if (pattern === path[0]) {
				let rest = path.slice(1)
				root.activated(params, rest, pattern)

				let newActiveSegments = activeSegments.slice()
				newActiveSegments.push(path[0])

				if (forwardRest && rest.length > 0) {
					NavigationController.navigatePath(rest, params, newActiveSegments, markHandled)
				}

				markHandled()
				break
			}
		}
	}
}
