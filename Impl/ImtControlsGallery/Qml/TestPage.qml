import QtQuick 2.12

import Acf 1.0
import imtcontrols 1.0
import QtQuick.Window 2.2
import imtgui 1.0

Rectangle {
    id: testPage;

    anchors.fill: parent;
    clip: true;

    property var defaultModel;

    TreeItemModel {
        id: testModel;

        Component.onCompleted: {
            console.log("Start createFromJson");
            testModel.createFromJson("{\"Test\": \"012345\"}");
            console.log("End createFromJson");
        }

        onDataChanged: {
            console.log("testModel onDataChanged", testModel.toJson());
        }
    }

    function updateGui(){
        customerIdInput.text = testModel.getData("String");
    }

    function updateModel(){
        testModel.setData("String", customerIdInput.text);
    }

    TextInputElementView {
        id: customerIdInput;

        width: 700;
        height: 100;

        name: qsTr("Customer-ID");
        placeHolderText: qsTr("Enter the customer-ID");

        onEditingFinished: {
            testPage.updateModel()
        }
    }
}
