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
		Stores the remaining (unprocessed) path when navigation
		is interrupted at this Navigable. This allows child
		Navigables to later pick up the pending path once they
		are created and ready to handle it.

		\internal
	*/
	property var pendingPath: null

	/*!
		Emitted when this Navigable is activated.

		\param params      Arbitrary parameters passed with navigation
		\param restPath    Remaining path segments after this Navigable
		\param matchedPath The path segment that matched this Navigable
	*/
	signal activated(var params, var restPath, string matchedPath)

	Component.onCompleted: {
		NavigationController.navigatePath.connect(processPath)
		NavigationController.registerNavigableItem(root)
		checkParentSegmentPending()
	}

	/*!
		Checks whether the parent Navigable has a pending path that
		this Navigable can handle. If so, the pending path is consumed
		and navigation is resumed from this Navigable.

		Called automatically when this Navigable is created, and also
		when \c parentSegment or \c paths change.
	*/
	function checkParentSegmentPending(){
		let parentNavigableItem = NavigationController.getNavigableItem(parentSegment)
		if (parentNavigableItem !== null){
			let pendingPath = parentNavigableItem.pendingPath
			if (pendingPath !== null){
				if (paths.includes(pendingPath.rest[0])){
					NavigationController.navigatePath(pendingPath.rest, pendingPath.params, pendingPath.activeSegments, function(){})
					parentNavigableItem.pendingPath.rest = parentNavigableItem.pendingPath.rest.slice(1)
					root.pendingPath = {
						rest: parentNavigableItem.pendingPath.rest,
						params: parentNavigableItem.pendingPath.params,
						activeSegments: parentNavigableItem.pendingPath.activeSegments
					}
					parentNavigableItem.pendingPath = null
				}
			}
		}
	}

	onParentSegmentChanged: {
		checkParentSegmentPending()
	}

	onPathsChanged: {
		checkParentSegmentPending()
	}

	Component.onDestruction: {
		NavigationController.navigatePath.disconnect(processPath)
	}

	/*!
		Processes a navigation request for the given \a path.

		If the first segment of the path matches one of this Navigable's
		\c paths, the \c activated signal is emitted. The remaining
		segments may be forwarded further if \c forwardRest is true.

		Any leftover path is stored in \c pendingPath, allowing child
		Navigables to continue navigation later.
	*/
	function processPath(path, params, activeSegments, markHandled) {
		if (path.length === 0)
			return

		if (parentSegment === "" && activeSegments.length > 0){
			console.log("processPath", path, activeSegments)
			console.log("parentSegment", parentSegment)
			console.log("paths", paths)
			return
		}

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

				root.pendingPath = {
					rest: rest,
					params: params,
					activeSegments: newActiveSegments
				}

				markHandled()
				break
			}
		}
	}
}
