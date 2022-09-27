import QtQuick 2.15
import Acf 1.0

Rectangle {
    id: treeViewContainer;

    color: Style.baseColor;

    property TreeItemModel modelItems;

    function resetSelectedItem(model){
        let count = model.GetItemsCount();
        for (let i = 0; i < count; i++){
            let selected = model.GetData("Selected", i);

            if (selected){
                model.SetData("Selected", false, i);
            }

            let childModel = model.GetData("ChildModel", i);
            if (childModel){
                resetSelectedItem(childModel);
            }
        }
    }

    MouseArea {
        anchors.fill: parent;

        onClicked: {
            resetSelectedItem(modelItems);
        }
    }

    Component {
        id: delegateComp;

        TreeItemDelegate {
            width: treeViewContainer.width;

            onDoubleClicked: {

            }
        }
    }

    ListView {
        id: mainTreeView;

        anchors.fill: parent;

        boundsBehavior: Flickable.StopAtBounds;
        model: treeViewContainer.modelItems;

        delegate: delegateComp;
    }
}
