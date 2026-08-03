import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

DecoratorBase {
    id: topPanelDecorator;

    height: Style.sizePanelsHeight !== undefined ? Style.sizePanelsHeight : Style.headerHeight;

    property Item centerPanel: null;
    property Item topPanel: null;

    // Flat, with a hairline for an edge. The four gradient stops this replaces
    // spent their last three percent faking that hairline, which left a muddy
    // grey band under a panel that was otherwise nearly white.
    Rectangle {
        anchors.fill: parent;

        color: Style.baseColor;
    }

    Rectangle {
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;

        height: 1;
        opacity: 0.5;
        color: Style.borderColor;
    }

    // The panel is handed down so the corner can measure itself against the bar
    // it sits in and give way when the window narrows.
    TopLeftPanel {
        id: topLeftPanel;
        anchors.left: parent.left;
        widthFromDecorator: true;
        height: topPanelDecorator.height;
        topPanel: topPanelDecorator;
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
		width: Style.sizeHintBXS;
        height: topPanelDecorator.height;
    }
}

