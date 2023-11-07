import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0

Rectangle {
    id: topPanelDecorator;

    z: -1

    width: 100;
    height: 60;


    gradient: Gradient {
        GradientStop { position: 0.0; color: Style.imagingToolsGradient1; }
        GradientStop { position: 0.97; color: Style.imagingToolsGradient2; }
        GradientStop { position: 0.98; color: Style.imagingToolsGradient3; }
        GradientStop { position: 1.0; color: Style.imagingToolsGradient4; }
    }
    property bool isQtStyle: false;

    onIsQtStyleChanged: {
        Style.isQtStyle = isQtStyle;
    }

    property Component buttonDecoratorAcf: buttonDecorator;
    property Component buttonDecoratorQt: DecoratorsQt.buttonDecorator;


    Row{
        id: buttonRow;

        anchors.centerIn: parent;
        spacing: 40;

        Button{
            id: themeButton;

            anchors.verticalCenter:  parent.verticalCenter;

            decoratorComponent: topPanelDecorator.isQtStyle ? topPanelDecorator.buttonDecoratorQt: topPanelDecorator.buttonDecoratorAcf;
            //decoratorComponent: DecoratorsQt.buttonDecorator;
            //KeyNavigation.right: roundButton
            //KeyNavigation.tab: roundButton

            text: "Dark theme";
            onClicked: {
            }
        }

        Button{
            id: qtStyleButton;

            anchors.verticalCenter:  parent.verticalCenter;

            decoratorComponent: topPanelDecorator.isQtStyle ? topPanelDecorator.buttonDecoratorQt: topPanelDecorator.buttonDecoratorAcf;
            //decoratorComponent: DecoratorsQt.buttonDecorator;
            //KeyNavigation.right: roundButton
            //KeyNavigation.tab: roundButton

            text: "Qt style";
            onClicked: {
                topPanelDecorator.isQtStyle = true;
            }
        }

        Button{
            id: acfStyleButton;

            anchors.verticalCenter:  parent.verticalCenter;

            decoratorComponent: topPanelDecorator.isQtStyle ? topPanelDecorator.buttonDecoratorQt: topPanelDecorator.buttonDecoratorAcf;
            //decoratorComponent: DecoratorsQt.buttonDecorator;
            //KeyNavigation.right: roundButton
            //KeyNavigation.tab: roundButton

            text: "Acf style";
            onClicked: {
                topPanelDecorator.isQtStyle = false;
            }
        }
    }

    Component{
        id: buttonDecorator;

        ButtonDecorator{
            width: 140;
            height: 30;
            radius: 4;
            //            icon.width: 20;
            //            icon.height: 20;
        }
    }
}

