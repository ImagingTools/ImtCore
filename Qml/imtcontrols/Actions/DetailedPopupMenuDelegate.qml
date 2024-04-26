import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

PopupMenuDelegate {
    id: popupMenuDelegate;

    decorator: Style.detailedPopupItemDelegateDecorator;

    property string description: "";
}
