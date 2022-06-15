import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Item {
    id: topPanelDecorator;

    property alias centerPanel: centerPanel;
    property Item topPanel;

    Component.onCompleted: {
        console.log("topPanelDecorator onCompleted")
        console.log("Style.topRightPanelDecoratorPath", Style.topRightPanelDecoratorPath)
    }

    TopCenterPanel {
        id: centerPanel;

        anchors.left: parent.left;

        width: 0.9 * topPanelDecorator.width;
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

        width: 0.1 * topPanelDecorator.width;
        height: topPanelDecorator.height;

        decoratorSource: Style.topRightPanelDecoratorPath;

        gradient: centerPanel.gradient;

        onDecoratorSourceChanged: {
            console.log("rightPanel onDecoratorSourceChanged", Style.topRightPanelDecoratorPath)
        }
    }
}

