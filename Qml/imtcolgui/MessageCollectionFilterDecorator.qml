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
        if (width - filtermenu.width <= content.width + 2 * content.spacing){
            content.visible = false;
        }
        else{
            content.visible = true;
        }
    }

    function updateText(){
        infoFilter.text = qsTr("Info");
        warningFilter.text = qsTr("Warning");
        errorFilter.text = qsTr("Error");
        criticalFilter.text = qsTr("Critical");
    }

    Rectangle{
        anchors.fill: parent;

        color: Style.backgroundColor;
    }


    function onLocalizationChanged(language){
        updateText()
    }



    TreeItemModel {
        id: modelCategogy;

        Component.onCompleted: {
            mainItem.updateModel();
        }
    }

    SegmentedButton {
        id: content;

        anchors.left: parent.left;
        anchors.verticalCenter: parent.verticalCenter;

        // spacing: Style.size_mainMargin;
        height: parent.height


        Button {
            id: infoFilter;

            anchors.verticalCenter: parent.verticalCenter;
            // height: Style.itemSizeMedium
            checkable: true
            checked: true
            iconSource: "../../../../" + Style.getIconPath("Icons/Info", Icon.State.On, Icon.Mode.Normal);
            onCheckedChanged: {
                mainItem.baseElement.filterChanged("InfoFilter", checked);
            }
        }

        Button {
            id: warningFilter;

            anchors.verticalCenter: parent.verticalCenter;
            // height: Style.itemSizeMedium
            checkable: true
            checked: true
            iconSource: "../../../../" + Style.getIconPath("Icons/Warning", Icon.State.On, Icon.Mode.Normal);
            onCheckedChanged: {
                mainItem.baseElement.filterChanged("WarningFilter", checked);
            }
        }

        Button {
            id: errorFilter;

            anchors.verticalCenter: parent.verticalCenter;
            // height: Style.itemSizeMedium
            checkable: true
            checked: true

            iconSource: "../../../../" + Style.getIconPath("Icons/Error", Icon.State.On, Icon.Mode.Normal);
            onCheckedChanged: {
                mainItem.baseElement.filterChanged("ErrorFilter", checked);
            }
        }

        Button {
            id: criticalFilter;

            anchors.verticalCenter: parent.verticalCenter;
            // height: Style.itemSizeMedium
            checkable: true
            checked: true

            iconSource: "../../../../" + Style.getIconPath("Icons/Critical", Icon.State.On, Icon.Mode.Normal);
            onCheckedChanged: {
                mainItem.baseElement.filterChanged("CriticalFilter", checked);
            }
        }

    }

    FilterPanelDecorator {
        id: filtermenu

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;

        baseElement: mainItem.baseElement;

        width: 325;
    }
}

