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

		return true
	}

	/*!
		Returns the first registered NavigableItem that supports
		the given \a segment, or null if none match.

		\param segment Path segment string
		\return NavigableItem or null
	*/
	function getNavigableItem(segment){
		for (let i = 0; i < navigableItems.length; ++i){
			let navigableItem = navigableItems[i]
			if (navigableItem.paths.includes(segment)){
				return navigableItem
			}
		}

		return null
	}

	/*!
		Navigates to the given path.
		If the path is handled, it is pushed to the navigation stack.

		\param path   Full navigation path (e.g. "Orders/Order/123")
		\param params Arbitrary parameters
	*/
	function navigate(path, params) {
		let segments = path.split("/")
		let handled = false

		let markHandledFunc = function() { handled = true }

		root.navigatePath(segments, params || {}, [], markHandledFunc)

		if (handled) {
			__internal.push(path, params)
		} else {
			console.error("Path not handled:", path)
		}
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
		/*!
			Stack of navigation entries: [{ path, params }, ...]
		*/
		property var stack: []

		/*!
			Current index in the stack.
		*/
		property int currentIndex: -1

		onCurrentIndexChanged: {
			root.currentIndexChanged(currentIndex)
		}

		/*!
			Clears the navigation stack and resets index.
		*/
		function clear() {
			stack = []
			currentIndex = -1
		}

		/*!
			Pushes a new navigation entry into the stack.
			If there are forward entries, they are discarded.
			Ensures the stack size does not exceed \c root.maxSize.
		*/
		function push(path, params) {
			if (currentIndex < stack.length - 1) {
				stack.splice(currentIndex + 1, stack.length - (currentIndex + 1))
			}

			stack.push({ path: path, params: params || {} })

			while (stack.length > root.maxSize) {
				stack.shift()
				if (currentIndex > 0) currentIndex--
			}

			currentIndex = stack.length - 1
		}

		/*!
			Performs internal navigation to the current stack entry.
			Used when moving back/forward in the stack.
		*/
		function navigateInternal() {
			if (currentIndex < 0 || currentIndex >= stack.length) {
				console.log("Unable to navigate internal. Current index is invalid")
				return
			}

			let entry = stack[currentIndex]
			let segments = entry.path.split("/")
			let handled = false

			let markHandledFunc = function() { handled = true }

			root.navigatePath(segments, entry.params || {}, [], markHandledFunc)

			if (!handled)
				console.log("Internal navigation failed:", entry.path)
		}
	}
}
