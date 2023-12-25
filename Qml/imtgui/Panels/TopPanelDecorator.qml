import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: topPanelDecorator;

    height: 60;

    property Item centerPanel: null;
    property Item topPanel: null;

    Component.onCompleted: {
        console.log("topPanelDecorator base onCompleted")
    }

    TopCenterPanel {
        id: topCenterPanel;

        anchors.left: parent.left;

        width: topPanelDecorator.width - rightPanel.width;
        height: topPanelDecorator.height;

        topPanel: topPanelDecorator.topPanel;

        decoratorSource: Style.topCenterPanelDecoratorPath;

        gradient: Gradient {
            GradientStop { position: 0.0; color: Style.imagingToolsGradient1; }
            GradientStop { position: 0.97; color: Style.imagingToolsGradient2; }
            GradientStop { position: 0.98; color: Style.imagingToolsGradient3; }
            GradientStop { position: 1.0; color: Style.imagingToolsGradient4; }
        }
    }

    TopRightPanel {
        id: rightPanel;

        z: 100;

        anchors.right: parent.right;

        width: 100;
        height: topPanelDecorator.height;

        decoratorSource: Style.topRightPanelDecoratorPath;

        gradient: Gradient {
            GradientStop { position: 0.0; color: Style.imagingToolsGradient1; }
            GradientStop { position: 0.97; color: Style.imagingToolsGradient2; }
            GradientStop { position: 0.98; color: Style.imagingToolsGradient3; }
            GradientStop { position: 1.0; color: Style.imagingToolsGradient4; }
        }

    }
}

