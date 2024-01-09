import QtQuick 2.12
import imtgui 1.0
import imtcontrols 1.0

DecoratorBase {
    id: topPanelDecorator;

    height: 60;

    property Item centerPanel: null;
    property Item topPanel: null;

    Component.onCompleted: {
        console.log("topPanelDecorator base onCompleted")
    }

    Rectangle {
        anchors.fill: parent

        gradient: Gradient {
            GradientStop { position: 0.0; color: Style.imagingToolsGradient1; }
            GradientStop { position: 0.97; color: Style.imagingToolsGradient2; }
            GradientStop { position: 0.98; color: Style.imagingToolsGradient3; }
            GradientStop { position: 1.0; color: Style.imagingToolsGradient4; }
        }
    }

    TopCenterPanel {
        id: topCenterPanel;

        anchors.left: parent.left;

        width: topPanelDecorator.width - rightPanel.width;
        height: topPanelDecorator.height;
    }

    TopRightPanel {
        id: rightPanel;

        z: 100;

        anchors.right: parent.right;

        width: 100;
        height: topPanelDecorator.height;
    }
}

