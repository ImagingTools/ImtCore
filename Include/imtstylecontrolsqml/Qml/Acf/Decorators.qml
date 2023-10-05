pragma Singleton
import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0

Item {
    property Component topButtonDecorator: topButtonDecoratorComp
    property Component topPanelDecorator: topPanelDecoratorComp
    property Component buttonDecorator: buttonDecoratorComp
    property Component roundButtonDecorator: roundButtonDecoratorComp

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

    Component{
        id: buttonDecoratorComp;

        ButtonDecorator{
            width: 160;
            height: 30;
            radius: 4;

        }
    }

    Component{
        id: roundButtonDecoratorComp;

        ButtonDecorator{
            width: 30;
            height: width;
            radius: width;
        }
    }
}
