pragma Singleton
import QtQuick 2.0
import Acf 1.0
import QtQuick.Controls 2.15

Item {
    property Component buttonDecorator: buttonDecoratorComp
    property Component roundButtonDecorator: roundButtonDecoratorComp

    Component {
        id: buttonDecoratorComp

        Button{
            width: 160;
            height: 40;
            text: !baseButton ? "" : baseButton.text;
            icon.source: !baseButton ? "" : baseButton.iconSource;
            checkable: !baseButton ? false : baseButton.checkable;
            checked: !baseButton ? false : !baseButton.checkable ? false : baseButton.checked;
            highlighted: !baseButton ? false : baseButton.focus;
            property var baseButton;
            onClicked: {
                //console.log("Qt Button clicked!!!")
                if(baseButton){
                    baseButton.clicked()
                }
            }
            onToggled: {
                if(checked){
                    console.log("Qt Button toggled!!!")
                    if(baseButton){
                        baseButton.toggled()
                    }
                }
            }

        }
    }

    Component {
        id: roundButtonDecoratorComp

        RoundButton{
            width: 40;
            height: 40;
            radius: width;
            text: !baseButton ? "" : baseButton.text;
            icon.source: !baseButton ? "" : baseButton.iconSource;
            highlighted: !baseButton ? false : baseButton.focus;
            property var baseButton;
            onClicked: {
                //console.log("Qt Button clicked!!!")
                if(baseButton){
                    baseButton.clicked()
                }
            }

        }
    }

}
