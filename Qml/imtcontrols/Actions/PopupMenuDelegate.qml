import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

ItemDelegate {
    id: popupMenuDelegate;

    decorator: Style.popupItemDelegateDecorator;

    property bool selected: false;
    property bool isSeparator: String(model.Id) === "";

    // Reference to the control in which the delegate is declared
    property Item containerItem: null;
    property Item rootItem: null;

    onSelectedChanged: {
        console.log("ItemDelegate onSelectedChanged", selected);
    }

    onHeightChanged: {
        internal.checkSeparator();
    }

    onIsSeparatorChanged: {
        internal.checkSeparator();
    }

    QtObject {
        id: internal;

        function checkSeparator(){
            if (popupMenuDelegate.isSeparator){
                mouseArea.visible = false;
                popupMenuDelegate.height = 5;
            }
        }
    }
}
