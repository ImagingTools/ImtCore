import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

DecoratorBase {
    id: topPanelDecorator;

    height: 60;

    property Item centerPanel: null;
    property Item topPanel: null;

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    TopLeftPanel {
        id: topLeftPanel;
        anchors.left: parent.left;
        width: 100;
        height: topPanelDecorator.height;
    }

    TopCenterPanel {
        id: topCenterPanel;
        anchors.left: topLeftPanel.right;
        anchors.right: rightPanel.left;
        height: topPanelDecorator.height;
        clip: true;
    }

    TopRightPanel {
        id: rightPanel;
        z: 100;
        anchors.right: parent.right;
        width: 100;
        height: topPanelDecorator.height;
    }

    Rectangle {
        anchors.bottom: parent.bottom;
        width: parent.width;
        height: 1;
        color: Style.borderColor;
    }
}

