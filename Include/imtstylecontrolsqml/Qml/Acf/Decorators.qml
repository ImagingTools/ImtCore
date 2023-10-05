pragma Singleton
import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0

Item {
    property Component topButtonDecorator: topButtonDecoratorComp
    property Component topPanelDecorator: topPanelDecoratorComp

    Component {
        id: topButtonDecoratorComp

        TopButtonDecoratorNew{

        }
    }

    Component{
        id: topPanelDecoratorComp;

        TopPanelDecorator{

        }

    }
}
