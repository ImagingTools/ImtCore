import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

DecoratorBase  {
    id: topPanelDecorator;

	signal setDesignScheme(string designId);

    Rectangle {
        anchors.fill: parent

        gradient: Gradient {
            GradientStop { position: 0.0; color: Style.imagingToolsGradient1; }
            GradientStop { position: 0.97; color: Style.imagingToolsGradient2; }
            GradientStop { position: 0.98; color: Style.imagingToolsGradient3; }
            GradientStop { position: 1.0; color: Style.imagingToolsGradient4; }
        }
    }

    property bool isQtStyle: false;


    Row{
        id: buttonRow;

        anchors.centerIn: parent;
        height: parent.height
        spacing: 40;

        Button{
            id: themeButton;

            anchors.verticalCenter:  parent.verticalCenter;

            KeyNavigation.right: qtStyleButton
            KeyNavigation.tab: qtStyleButton

			property bool isLightTheme: true
			text: isLightTheme ? "Dark theme" : "Light theme";
            onClicked: {
				isLightTheme = !isLightTheme
				if(isLightTheme){
					topPanelDecorator.setDesignScheme("Light")
				}
				else {
					topPanelDecorator.setDesignScheme("Dark")
				}
            }
        }

        Button{
            id: qtStyleButton;

            anchors.verticalCenter:  parent.verticalCenter;

            KeyNavigation.right: acfStyleButton
            KeyNavigation.tab: acfStyleButton

            text: "Qt style";
            onClicked: {
                topPanelDecorator.isQtStyle = true;
                Events.sendEvent("OnStyleDecoratorChanged", "StyleQt");
            }
        }

        Button{
            id: acfStyleButton;

            anchors.verticalCenter:  parent.verticalCenter;

            //KeyNavigation.right: themeButton
            //KeyNavigation.tab: themeButton

            text: "Acf style";
            onClicked: {
                topPanelDecorator.isQtStyle = false;
                Events.sendEvent("OnStyleDecoratorChanged", "StyleAcf");
            }
        }
    }
}

