pragma Singleton

import QtQuick 2.12

/*!
	\qmltype NavigationController
	\inqmlmodule Custom
	\brief Centralized navigation manager with history stack.

	NavigationController is a singleton that manages application navigation
	through hierarchical path-based routing. It stores a navigation stack,
	allows forward/backward navigation, and emits navigation signals to
	registered \c NavigableItem items.

	It ensures only valid paths are stored in the stack, trimming history
	when navigating backward and pushing new routes.

	Example usage:
	\qml
	NavigationController.navigate("Orders", { filter: "active" })
	NavigationController.navigate("Orders/Order/12345")
	\endqml

	\sa NavigableItem
*/
QtObject {
	id: root

	/*!
		Maximum number of entries in the navigation stack.
		Older entries are discarded when this size is exceeded.
	*/
	property int maxSize: 10

	/*!
		Emitted when navigation is requested.

		\param path            Array of path segments
		\param params          Arbitrary parameters
		\param activeSegments  Already matched path segments
		\param resultCallback  Function that must be called when path is handled
	*/
	signal navigatePath(var path, var params, var activeSegments, var resultCallback)

	/*!
		List of currently registered NavigableItem objects.
		Each NavigableItem must register itself on creation and can
		be later queried using \c getNavigableItem().
	*/
	property var navigableItems: []

	/*!
		Emitted when the current navigation index changes.
		\param index New stack index
	*/
	signal currentIndexChanged(int index)

	/*!
		Registers a new NavigableItem in the controller.

		\param navigableItem NavigableItem instance to register
		\return true if registration was successful, false if already registered
	*/
	function registerNavigableItem(navigableItem){
		if (navigableItems.includes(navigableItem)){
			return false
		}

		navigableItems.push(navigableItem)

		// Проверяем, есть ли pending для этого сегмента
		let key = navigableItem.parentSegment
		if (key && pending[key]) {
			let p = pending[key]
			if (navigableItem.paths.includes(p.rest[0])) {
				console.log("Delivering pending path", p, "to", navigableItem.paths)
				navigableItem.processSegment(p.rest[0], p.params, p.rest.slice(1))
				delete pending[key]
			}
		}

		return true
	}

	property var pending: ({})
	function setPending(parentSegment, rest, params) {
		pending[parentSegment] = {
			rest: rest,
			params: params
		}
	}

	function unregisterNavigableItem(navigableItem){
		let index = navigableItems.indexOf(navigableItem)
		if (index > -1) {
			navigableItems.splice(index, 1)
			return true
		}

		return false
	}

	function navigate(path, params) {
		console.log("navigate", path)
		let segments = path.split("/")
		if (segments.length === 0)
			return
		
		let first = segments[0]
		let rest = segments.slice(1)
		
		for (let i = 0; i < navigableItems.length; ++i) {
			let item = navigableItems[i]
			if (item.segmentIsSupported(first)) {
				item.processSegment(first, params || {}, rest)
				__internal.push(path, params || {})
				return
			}
		}
		
		console.error("No NavigableItem found for:", first)
	}

	/*!
		Pushes a new entry into the stack without verifying it.
		Typically used internally after \c navigate().
	*/
	function push(path, params) {
		__internal.push(path, params)
	}

	/*!
		Moves forward in the navigation stack.
	*/
	function next() {
		if (hasNext()) {
			__internal.currentIndex++
			__internal.navigateInternal()
		}
	}

	/*!
		Returns true if there is a forward entry in the stack.
	*/
	function hasNext() {
		return __internal.currentIndex < __internal.stack.length - 1
	}

	/*!
		Moves backward in the navigation stack.
	*/
	function prev() {
		if (hasPrev()) {
			__internal.currentIndex--
			__internal.navigateInternal()
		}
	}

	/*!
		Returns true if there is a previous entry in the stack.
	*/
	function hasPrev() {
		return __internal.currentIndex > 0
	}

	/*!
		Clears the navigation stack completely.
	*/
	function clear() {
		__internal.clear()
	}

	/*!
		\internal
		Private object that maintains the navigation stack and index.
	*/
	property QtObject __internal: QtObject {
			property var stack: []          // [{ path, params }, ...]
			property int currentIndex: -1
	
			// forward currentIndex changes
			onCurrentIndexChanged: {
				root.currentIndexChanged(currentIndex)
			}
	
			function clear() {
				stack = []
				currentIndex = -1
			}
	
			/*
				Push new entry. If we are not at the end of the stack,
				discard forward entries. Trim by maxSize.
			*/
			function push(path, params) {
				// drop forward entries
				if (currentIndex < stack.length - 1) {
					stack.splice(currentIndex + 1, stack.length - (currentIndex + 1))
				}
	
				stack.push({ path: path, params: params || {} })
	
				// trim by maxSize
				while (stack.length > root.maxSize) {
					stack.shift()
					if (currentIndex > 0) currentIndex = currentIndex - 1
				}
	
				// set to last
				currentIndex = stack.length - 1
			}
	
			/*
				Replay current stack entry WITHOUT pushing it to history.
				This is used by prev()/next() to replay saved routes.
			*/
			function navigateInternal() {
				if (currentIndex < 0 || currentIndex >= stack.length) {
					console.log("Unable to navigate internal. Current index is invalid")
					return
				}
	
				let entry = stack[currentIndex]
				if (!entry || !entry.path) {
					console.log("navigateInternal: empty entry")
					return
				}
	
				let segments = entry.path.split("/")
				if (segments.length === 0) return
	
				let first = segments[0]
				let rest = segments.slice(1)
	
				for (let i = 0; i < root.navigableItems.length; ++i) {
					let item = root.navigableItems[i]
					if (item && item.segmentIsSupported && item.segmentIsSupported(first)) {
						try {
							// We call processSegment but DO NOT push again
							item.processSegment(first, entry.params || {}, rest)
						} catch (e) {
							console.error("navigateInternal: processing error:", e)
						}
						return
					}
				}
	
				console.warn("navigateInternal: no item found for root segment:", first)
			}
		}
}
