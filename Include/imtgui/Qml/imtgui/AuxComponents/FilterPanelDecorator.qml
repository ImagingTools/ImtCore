import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Item {
    id: filterPanelDecorator;

    width: 325;

    property Item rootLoader: null;

    property alias textInputWidth: tfc.width;

    function calcWidth(){
        console.log("calcWidth");

        if (filterPanelDecorator.rootLoader){
            console.log("parent.width", filterPanelDecorator.rootLoader.parent.width);
            let width = filterPanelDecorator.rootLoader.parent.width - 30;
            if (width <= tfc.width){
                tfc.width = width;
            }
            else{
                tfc.width = 270;
            }

            console.log("tfc.width", tfc.width);
        }
    }

    onVisibleChanged: {
        console.log("onVisibleChanged", tfc.width);

        if (filterPanelDecorator.visible){
            animRect.to = tfc.width;
            animRect.start();
        }
    }

    onFocusChanged: {
        if (filterPanelDecorator.focus){
            tfc.forceActiveFocus();
        }
    }

    NumberAnimation {
        id: animRect;
        target: tfc;
        property: "width";
        from: 0;
        duration: 200;
    }

    CustomTextField {
        id: tfc;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: closeButton.left;
        anchors.rightMargin: Style.margin;

        width: 270;
        height: filterPanelDecorator.height;

        placeHolderText: qsTr("Enter some text to filter the item list");

        onTextChanged: {
            filterPanelDecorator.rootLoader.textChanged(model.index, tfc.text);
        }

        AuxButton {
            id: iconClear;

            anchors.right: parent.right;
            anchors.rightMargin: Style.margin;
            anchors.verticalCenter: parent.verticalCenter;

            width: Style.buttonWidthSmall;
            height: width;

            visible: tfc.text != "";

            iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);
            iconWidth: Style.iconSizeExtraSmall;
            iconHeight: iconWidth;

            onClicked: {
                tfc.text = "";
            }
        }
    }

    AuxButton {
        id: closeButton;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;

        width: Style.buttonWidthMedium;
        height: width;

        iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);

        iconWidth: Style.iconSizeSmall;
        iconHeight: iconWidth;

        onClicked: {
            tfc.text = "";
            filterPanelDecorator.rootLoader.onClosed();
        }
    }
}
