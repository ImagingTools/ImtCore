import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item {
    id: container;

    width: row.width

    property Component firstSegmentDecorator: Style.firstSegmentDecorator
    property Component middleSegmentDecorator: Style.middleSegmentDecorator
    property Component lastSegmentdecorator: Style.lastSegmentDecorator

    property bool isExclusive: false
    property bool checkable: true

    property bool compl: false;
    property var helpers_: []

    property bool ready: container.compl  && firstSegmentDecorator && middleSegmentDecorator && lastSegmentdecorator
    property bool isSet: false;
    property int spacing: 0;
    property int selectedIndex: -1;
    property alias rowAlias: row;

    onReadyChanged: {
        console.log("SegmentedButton children.length:: ", children.length)
        let count  = children.length
        let list = []
        for (let index = 1; index < count; index++){
            list.push(children[index])
        }
        children = [row]
        row.children = list
        row.setup()
    }

    Component.onCompleted: {
        compl = true
    }

    function setChecked(itemIndex){
        if (!checkable){
            return;
        }

        for (let index = 0; index < row.children.length; index++){
            if (index != itemIndex){
                row.children[index].checked = false
            }
        }
        row.children[itemIndex].checked = true
        container.selectedIndex = itemIndex
    }

    Component {
        id: objectHelper
        QtObject {
            property int itemIndex: -1

            function onClicked(){
                console.log("sender", itemIndex, row.children[itemIndex])
                container.setChecked(itemIndex)
            }
        }
    }

    function createObjectHelper(index){
        var helper = objectHelper.createObject(container);
        helper.itemIndex = index

        return helper
    }

    function setDecorator(index){
        if (index == 0){
            row.children[index].decorator = container.firstSegmentDecorator
        }
        else if (index == row.children.length - 1){
            row.children[index].decorator = container.lastSegmentdecorator
        }
        else{
            row.children[index].decorator = container.middleSegmentDecorator
        }

    }

    Row {
        id: row

        height: container.height
        spacing: container.spacing

        function setup() {
            console.log("SegmentedButton row children.length:: ", children.length)
            for (let index = 0; index < children.length; index++){

                container.setDecorator(index)

                if (container.checkable){
                    children[index].checkable = true
                }
                if (container.isExclusive){
                    var helper = container.createObjectHelper(index)
                    children[index].clicked.connect(helper.onClicked)
                }
            }

            container.isSet = true;
        }
    }
}

