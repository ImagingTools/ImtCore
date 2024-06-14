import QtQuick 2.12

import Acf 1.0
import imtcontrols 1.0
import QtQuick.Window 2.2
import imtgui 1.0
import imtdocgui 1.0

Rectangle {
    id: testPage;

    anchors.fill: parent;
    clip: true;

    property var defaultModel;

    TreeItemModel {
        id: testModel;

        Component.onCompleted: {
            testModel.insertNewItem();
            testModel.setData("CustomerId", "Test");
            testModel.addTreeModel("ChildModel")

            undoRedoManager.registerModel(testModel);
        }

        onDataChanged: {
            console.log("testModel onDataChanged", testModel.toJson());
        }
    }

    function updateGui(){
        customerIdInput.text = testModel.getData("CustomerId");
        listView.model = testModel.getData("ChildModel");
    }

    function updateModel(){
        testModel.setData("CustomerId", customerIdInput.text);
    }

    ListView {
        id: listView;

        width: 500;
        height: 200;

        delegate: Component {
            Rectangle {
                width: listView.width;
                height: 10;

                color: "red";
            }
        }
    }

    Row {
        id: buttons;

        anchors.top: listView.bottom;
        anchors.topMargin: 10;

        height: 40;

        Button {
            text: "+"

            onClicked: {
                listView.model.insertNewItemWithParameters(0, {"Id":"test"});
            }
        }

        Button {
            text: "-"

            onClicked: {
                if (listView.model.getItemsCount() > 0){
                    listView.model.removeItem(0)
                }
            }
        }
    }

    TextInputElementView {
        id: customerIdInput;

        anchors.top: buttons.bottom;

        width: 700;
        height: 100;

        name: qsTr("Customer-ID");
        placeHolderText: qsTr("Enter the customer-ID");

        onEditingFinished: {
            testPage.updateModel()
        }
    }

    UndoRedoManager {
        id: undoRedoManager;

        onModelChanged: {
            console.log("UndoManager onModelChanged");

            let undoSteps = getAvailableUndoSteps();
            let redoSteps = getAvailableRedoSteps();

            undoButton.enabled = undoSteps > 0;
            redoButton.enabled = redoSteps > 0;

            testPage.updateGui();
        }
    }

    Row {
        anchors.top: customerIdInput.bottom;
        anchors.topMargin: 10;

        height: 40;

        Button {
            id: undoButton;

            text: "Undo"
            enabled: false;

            onClicked: {
                undoRedoManager.doUndo();
            }
        }

        Button {
            id: redoButton;
            text: "Redo"

            enabled: false;

            onClicked: {
                undoRedoManager.doRedo();
            }
        }
    }
}
