import QtQuick

/*!
	\qmltype StateManager
	\inqmlmodule imtcontrols.Base
	\inherits QtObject
	\brief A lightweight state management component for QML applications.

	\details StateManager provides a flexible mechanism for managing UI states by allowing
	declarative property changes on target objects. Unlike Qt's built-in State system,
	StateManager offers programmatic control with automatic default value preservation
	and restoration capabilities.

	\section1 Key Features
	\list
	\li Declarative state definitions with property change sets
	\li Automatic preservation of original property values
	\li Easy restoration to default state
	\li Support for multiple target objects per state
	\li Deep cloning of object values to prevent reference issues
	\endlist

	\section1 Usage Example

	\qml
	import QtQuick
	import imtcontrols.Base

	Rectangle {
		id: root
		Rectangle {id: box; width: 100; height: 100; color: "blue"; opacity: 1.0}
		Text {id: label; text: "Normal"; color: "black"}
		StateManager {
			id: stateManager
			states: ({
				"highlighted": [
					{
						target: box,
						properties: {color: "red", opacity: 0.8, width: 150}
					},
					{
						target: label,
						properties: {text: "Highlighted!", color: "red"}
					}
				],
				"disabled": [
					{
						target: box,
						properties: {opacity: 0.3, color: "gray"}
					},
					{
						target: label,
						properties: {text: "Disabled", color: "gray"}
					}
				]
			})
		}

		Row {
			anchors.bottom: parent.bottom
			spacing: 10

			Button { text: "Highlight"; onClicked: {stateManager.state = "highlighted"}}
			Button { text: "Disable"; onClicked: {stateManager.state = "disabled"}}
			Button { text: "Reset"; onClicked: {stateManager.restoreDefaults()}}
		}
	}
	\endqml

	\section1 State Definition Format

	States are defined as a JavaScript object where keys are state names and values
	are arrays of change objects:

	\code
	states: ({
		"stateName": [
			{
				target: targetObject,      // Reference to QML object
				properties: {
					propertyName: value,   // Property changes to apply
					...
				}
			},
			...
		]
	})
	\endcode

	\sa {Qt Quick States}, QtObject
 */
QtObject {
	id: manager

	/*!
		\qmlproperty string StateManager::state
		\brief The name of the currently active state.

		\details Setting this property triggers a state transition. When set to an empty
		string or an undefined state name, all targets are restored to their
		default values.

		\default ""
	 */
	property string state: ''

	/*!
		\qmlproperty var StateManager::states
		\brief A dictionary containing all state definitions.

		This property holds a JavaScript object where each key is a state name
		and each value is an array of change objects. Each change object MUST
		contain:
		\list
		\li \c target - Reference to the QML object to modify
		\li \c properties - Object containing property-value pairs to apply
		\endlist

		\default {}
	 */
	property var states: ({})

	/*!
		\internal
		\qmlproperty var StateManager::_defaultValues
		\brief Internal storage for original property values.

		\details This property automatically stores the original values of properties
		before they are modified by state changes. Used internally for
		restoration purposes.

		\default {}
	 */
	property var _defaultValues: ({})

	/*!
		\internal
		\brief Signal handler triggered when the state property changes. Transitions to the specified state.

		\details This method performs the following operations:
		\list 1
		\li Resets all previously modified targets to their default values
		\li If the current \l state is valid, applies all property changes defined for that state
		\li Updates the \l state property to reflect the new state
		\endlist

		If the current \l state is \c null, empty, or not found in \l states, all targets
		are restored to defaults and \l state is set to an empty string.

		\sa restoreDefaults(), state	 
	 */
	onStateChanged: {
		_resetTargets()

		if (!state || !states[state]) {
			return
		}

		let stateData = states[state]
		for (let i = 0; i < stateData.length; i++) {
			let change = stateData[i]
			let target = change.target
			if (target === null || target === undefined) {
				console.warn('StateManager: Target is null or undefined for state "' + state + '"')
				continue
			}

			for (let prop in change.properties) {
				_saveDefault(target, prop)
				if (prop in target) {
					target[prop] = change.properties[prop]
				}
			}
		}
	}

	/*!
		\qmlmethod void StateManager::restoreDefaults()
		\brief Restores all modified properties to their original values.

		\details This method resets all target objects to their default property values
		that were captured before any state changes were applied. After calling
		this method, the \l state property will be set to an empty string.
	 */
	function restoreDefaults() {
		_resetTargets()
	}

	/*!
		\internal
		\qmlmethod void StateManager::_saveDefault(QtObject target, string prop)
		\brief Saves the current value of a property before modification.

		\details This method stores the original value of a property if it hasn't been
		saved already. The value is stored in \l _defaultValues using a composite
		key derived from the target object and property name.

		For stored values, the behavior depends on the value type:
		\list
		\li Primitive JavaScript types (number, string, boolean, null): stored via JSON round-trip and restored as-is.
		\li Plain JSON-serializable objects and arrays: stored as deep-cloned structures via JSON serialization.
		\li Other object values (including QML object references or types that only provide \c toString()): stored using their string representation and restored as a string value, not as the original object.
		\endlist

		\note Non-JSON-serializable objects and QML object references cannot be restored as their original objects; only their string representation is preserved.
		\param {QtObject} target - The QML object whose property value should be saved.
		\param {string} prop - The name of the property to save.

		\note This method is idempotent - subsequent calls for the same
			target/property pair will not overwrite the stored value.

		\sa _resetTargets(), _defaultValues
	 */
	function _saveDefault(target, prop) {
		let key = target.toString() + '<.>' + prop

		if (!(key in _defaultValues)) {
			let value = target[prop]
			let storedValue

			if (typeof value === 'object' && value !== null && value.toString) {
				storedValue = value.toString()
			}
			else {
				// Guard against undefined and other non-JSON-serializable values
				if (value === undefined) {
					storedValue = value
				} else {
					const json = JSON.stringify(value)
					if (json === undefined) {
						// Fallback: store value directly if it cannot be JSON-serialized
						storedValue = value
					} else {
						storedValue = JSON.parse(json)
					}
				}
			}

			_defaultValues[key] = {
				target: target,
				prop: prop,
				value: storedValue
			}
		}
	}

	/*!
		\internal
		\qmlmethod void StateManager::_resetTargets()
		\brief Restores all modified properties to their saved default values.

		\details Iterates through all entries in \l _defaultValues and restores each
		target object's property to its original value.

		\sa _saveDefault(), _defaultValues
	 */
	function _resetTargets() {
		for (let key in _defaultValues) {
			let data = _defaultValues[key]
			if (data && data.target) {
				data.target[data.prop] = data.value
			}
		}
		_defaultValues = ({})
	}
}
