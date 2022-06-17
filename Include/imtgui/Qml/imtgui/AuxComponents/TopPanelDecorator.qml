import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: topPanelDecorator;

    height: 60;

    property Item centerPanel: topCenterPanel;
    property Item topPanel;

    Component.onCompleted: {
        console.log("topPanelDecorator onCompleted")
    }

    TopCenterPanel {
        id: topCenterPanel;

        anchors.left: parent.left;

        width: 0.95 * topPanelDecorator.width;
        height: topPanelDecorator.height;

        topPanel: topPanelDecorator.topPanel;

       // decoratorSource: Style.topCenterPanelDecoratorPath;

        gradient: Gradient {
            GradientStop { position: 0.0; color: Style.imagingToolsGradient1; }
            GradientStop { position: 0.97; color: Style.imagingToolsGradient2; }
            GradientStop { position: 0.98; color: Style.imagingToolsGradient3; }
            GradientStop { position: 1.0; color: Style.imagingToolsGradient4; }
        }
    }

    TopRightPanel {
        id: rightPanel;

        anchors.right: parent.right;

        width: topPanelDecorator.width - topCenterPanel.width;
        height: topPanelDecorator.height;

        decoratorSource: Style.topRightPanelDecoratorPath;

        gradient: Gradient {
            GradientStop { position: 0.0; color: Style.imagingToolsGradient1; }
            GradientStop { position: 0.97; color: Style.imagingToolsGradient2; }
            GradientStop { position: 0.98; color: Style.imagingToolsGradient3; }
            GradientStop { position: 1.0; color: Style.imagingToolsGradient4; }
        }

        Component.onCompleted: {
            console.log("rightPanel");
            console.log("rightPanel.width", rightPanel.width);
            console.log("rightPanel.height", rightPanel.height);
            rightPanel.decoratorSource = "TopRightPanelDecorator.qml";
        }

        onDecoratorSourceChanged: {
            console.log("rightPanel onDecoratorSourceChanged", Style.topRightPanelDecoratorPath)
        }
    }
}

