import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

DecoratorBase {
    id: topPanelDecorator;

    height: 60;

    property Item centerPanel: null;
    property Item topPanel: null;

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: Style.imagingToolsGradient1; }
            GradientStop { position: 0.97; color: Style.imagingToolsGradient2; }
            GradientStop { position: 0.98; color: Style.imagingToolsGradient3; }
            GradientStop { position: 1.0; color: Style.imagingToolsGradient4; }
        }
    }

    TopLeftPanel {
        id: topLeftPanel;
        anchors.left: parent.left;
        widthFromDecorator: true;
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

