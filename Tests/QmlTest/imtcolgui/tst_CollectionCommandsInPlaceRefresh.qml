import QtQuick 2.12
import QtTest 1.12
import imtcolgui 1.0

TestCase {
	id: testCase

	name: "CollectionCommandsInPlaceRefresh"
	when: windowShown

	// A collection view with the parts the delegate reads; a row value tells whether the row may be edited
	Component {
		id: collectionViewComp

		Item {
			property string collectionId: "Tenants"
			property var commandsController: QtObject {
				property var states: ({})

				function setCommandIsEnabled(commandId, isEnabled){
					states[commandId] = isEnabled
				}

				signal commandsChanged()
			}
			property var dataController: QtObject {
				signal elementsReceived(var elements)
			}
			property var documentCollectionFilter: null
			property Item table: Item {
				property var elements: QtObject {
					property string scope: "Invited"

					function getData(key, index){
						return scope
					}

					function containsKey(key, index){
						return false
					}
				}

				function getSelectedIndexes(){
					return [0]
				}
			}

			signal selectionChanged(var selectedIds, var selectedIndexes)
		}
	}

	Component {
		id: delegateComp

		CollectionViewCommandsDelegateBase {
			function updateStateCustomCommands(selection, commandsController, elementsModel){
				if (elementsModel.getData("scope", selection[0]) === "Invited"){
					commandsController.setCommandIsEnabled("Edit", false)
				}
			}
		}
	}

	// Accepting an invitation changes the row in place: Edit follows without selecting the row again
	function test_inPlaceRefreshUpdatesTheCommands(){
		let view = createTemporaryObject(collectionViewComp, testCase)
		let delegate = createTemporaryObject(delegateComp, testCase, {"collectionView": view})

		delegate.updateItemSelection([0])
		compare(view.commandsController.states["Edit"], false)

		view.table.elements.scope = "Member"
		view.dataController.elementsReceived(view.table.elements)

		tryCompare(view.commandsController.states, "Edit", true, 1000)
	}
}
