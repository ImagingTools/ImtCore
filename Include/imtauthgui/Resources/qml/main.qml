import QtQuick 2.0
//import ACF 1.0
//import "../qml"


Item {
//    anchors.fill: parent
    width: 300
    height: 500
    //    color: "red"
    //    property TreeItemModel contactInfoModel

    //    required contactInfoModel

    ListModel {
        id: contactInfoModel1
        function getData(key, row){
            if(row === undefined)
                row = 0
            if(row === null)
                row = 0
            var modelObject = get(row)
            var  retVal =  modelObject[key]
            return retVal
            //    return "test"
        }
        ListElement { firstName: "Ivan2"; secondName: "Ivanov"; nicName: "nicIvanov" }

    }
    
    TreeItemModel {
        id:testModel
//        source: "../Models/__EXAMPLE_2__"
        source: "../../Models/__ContactInfo__"
//        source: "qrc:/qml/jsonData.txt"
    }
    
    ContactInfoEditor {
        width: parent.width
        height: 300
        id: contactEditor
//        contactInfoModel: contactInfoModel1
            contactInfoModel: testModel
    }
    Rectangle {
        //anchors.top: contactEditor.bottom
        id: rec1
        y: 300
        width: 100
        height: 100
        color: "gray"
        Text {
            id: nameTxt
            text: "name"
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                nameTxt.text = testModel.getData("firstName");
                //        contactEditor.contactInfoModel = testModel
                rec2.color = "green"
            }
        }
    }
    Rectangle {
        id: rec2
        //anchors.top: contactEditor.bottom
        y: 300
        x: 150
        width: 100
        height: 100
        color: "blue"
        Text {
            id: nameTxt2
            text: "name2 rec2"
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                //	    rec2.color = "black"
                rec1.color = "red"
            }
        }
    }

}


