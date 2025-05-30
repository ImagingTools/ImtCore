import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0


TableCellDelegateBase {
    id: delegateContainer;

    property alias icon: image_

    Image {
        id: image_;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.left: parent.left;
        anchors.leftMargin:  delegateContainer.rowDelegate ? delegateContainer.rowDelegate.textLeftMargin : 0

        width: visible ? Style.itemSizeS : 0;
        height: width;

        sourceSize.width: width;
        sourceSize.height: height;
    }

    TableCellText {
        anchors.left: image_.right;
        anchors.leftMargin:  delegateContainer.rowDelegate && image_.visible ? delegateContainer.rowDelegate.textLeftMargin : 0
        anchors.right: parent.right;
        anchors.rightMargin: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.textRightMargin : 0
        anchors.verticalCenter: parent.verticalCenter

        rowDelegate:  delegateContainer.rowDelegate
        text: delegateContainer.getValue()
    }
}

