import QtQuick 2.15
import Acf 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtgui 1.0
import imtauthgui 1.0

DecoratorBase {
    id: mainItem;

    width: baseElement ? baseElement.width: 0;
    height: 40;

    property alias segmentedButton: segmentedButton_
    property alias filtermenu: filtermenu_

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", onLocalizationChanged);
        checkWidth();
        updateText()
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", onLocalizationChanged);
    }

    onWidthChanged: {
        checkWidth();
    }

    function checkWidth(){
        if (width - filtermenu.width <= segmentedButton.width + 2 * segmentedButton.spacing){
            segmentedButton.visible = false;
        }
        else{
            segmentedButton.visible = true;
        }
    }

    function updateText(){
    }

    function onLocalizationChanged(language){
        updateText()
    }

    SegmentedButton {
        id: segmentedButton_
        anchors.left: parent.left;
        anchors.verticalCenter: parent.verticalCenter;

        // spacing: Style.sizeMainMargin;
        height: parent.height

        Button {
            id: infoFilter;

            anchors.verticalCenter: parent.verticalCenter;
            checkable: true
            checked: true
            text: qsTr("Info")
            iconSource: "../../../../" + Style.getIconPath("Icons/Info", Icon.State.On, Icon.Mode.Normal);
            widthFromDecorator: true;
            heightFromDecorator: true;
            onCheckedChanged: {
                mainItem.baseElement.filterChanged("InfoFilter", checked);
            }
        }

        Button {
            id: warningFilter;

            anchors.verticalCenter: parent.verticalCenter;
            checkable: true
            checked: true
            text: qsTr("Warning")
            iconSource: "../../../../" + Style.getIconPath("Icons/Warning", Icon.State.On, Icon.Mode.Normal);
            widthFromDecorator: true;
            heightFromDecorator: true;
            onCheckedChanged: {
                mainItem.baseElement.filterChanged("WarningFilter", checked);
            }
        }

        Button {
            id: errorFilter;

            anchors.verticalCenter: parent.verticalCenter;
            checkable: true
            checked: true
            text: qsTr("Error");
            iconSource: "../../../../" + Style.getIconPath("Icons/Error", Icon.State.On, Icon.Mode.Normal);
            widthFromDecorator: true;
            heightFromDecorator: true;
            onCheckedChanged: {
                if (mainItem.baseElement){
                    mainItem.baseElement.filterChanged("ErrorFilter", checked);
                }
            }
        }

        Button {
            id: criticalFilter;

            anchors.verticalCenter: parent.verticalCenter;
            checkable: true
            checked: true
            text: qsTr("Critical");
            iconSource: "../../../../" + Style.getIconPath("Icons/Critical", Icon.State.On, Icon.Mode.Normal);
            widthFromDecorator: true;
            heightFromDecorator: true;
            onCheckedChanged: {
                if (mainItem.baseElement){
                    mainItem.baseElement.filterChanged("CriticalFilter", checked);
                }
            }
        }

        Button {
            id: verboseFilter;

            anchors.verticalCenter: parent.verticalCenter;
            checkable: true
            checked: false
            iconSource: "../../../../" + Style.getIconPath("Icons/Diagnostics", Icon.State.On, Icon.Mode.Normal);
            widthFromDecorator: true;
            heightFromDecorator: true;
            text: qsTr("Verbose");
            Component.onCompleted: {
                if (mainItem.baseElement){
                    mainItem.baseElement.filterChanged("VerboseFilter", false);
                }
            }

            onCheckedChanged: {
                if (mainItem.baseElement){
                    mainItem.baseElement.filterChanged("VerboseFilter", checked);
                }
            }
        }

    }

    FilterPanelDecorator {
        id: filtermenu_
        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;

        baseElement: mainItem.baseElement;

        width: 325;
    }
}

