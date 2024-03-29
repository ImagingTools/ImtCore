import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item {
    id: rowDelegate;

    property var table: null;
    property int rowIndex: model.index;

    onTableChanged: {
        if (table){
            listView.model = table.columnCount
        }
    }

    ListView {
        id: listView;

        width: parent.width;
        height: parent.height;

        clip: true;

        orientation: ListView.Horizontal;

        boundsBehavior: Flickable.StopAtBounds;

        delegate: Component {
            Item {
                width: rowDelegate.table.width / rowDelegate.table.columnCount;
                height: parent.height;

                Text {
                    id: contentText;

                    width: parent.width;
                }

                Component.onCompleted: {
                    let headerId = rowDelegate.table.headers.GetData("Id", model.index);
                    let text = rowDelegate.table.elements.GetData(headerId, rowDelegate.rowIndex);
                    contentText.text = text;
                }
            }
        }
    }
}
