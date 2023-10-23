import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0



Rectangle {
    id: inputsRepresentaitonPage;

    anchors.fill: parent;


    property string styleName: "Qt";
    property bool isQtStyle: false;

    Component.onCompleted: {
        Events.subscribeEvent("QtStyle", inputsRepresentaitonPage.setQtStyle);
        Events.subscribeEvent("AcfStyle", inputsRepresentaitonPage.setAcfStyle);
        //textField.setFocus(true);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("QtStyle", inputsRepresentaitonPage.setQtStyle);
        Events.unSubscribeEvent("AcfStyle", inputsRepresentaitonPage.setAcfStyle);

    }

    function setQtStyle(){
        inputsRepresentaitonPage.isQtStyle = true;
    }

    function setAcfStyle(){
        inputsRepresentaitonPage.isQtStyle = false;

    }

    TreeItemModel{
        id: comboModel;

        Component.onCompleted: {
            var index = comboModel.InsertNewItem();
            comboModel.SetData("Id",index, index);
            comboModel.SetData("Name", "Name 1", index);

            index = comboModel.InsertNewItem();
            comboModel.SetData("Id",index, index);
            comboModel.SetData("Name", "Name 2", index);

            index = comboModel.InsertNewItem();
            comboModel.SetData("Id",index, index);
            comboModel.SetData("Name", "Name 3", index);
        }

    }

    Column{
        anchors.centerIn: parent;
        spacing: 100;
        Row{
            id: inputsRow;

            anchors.horizontalCenter: parent.horizontalCenter;

            height: 40;

            spacing: 50;

            TextField{
                id: textField;

                anchors.verticalCenter: parent.verticalCenter;

                decoratorComponent: inputsRepresentaitonPage.isQtStyle ? DecoratorsQt.textFieldDecorator :
                                                                      Decorators.textFieldDecorator;
                placeHolderText: "placeHolder";
            }

            Rectangle{
                id: textEditBackground;

                anchors.verticalCenter:  parent.verticalCenter;

                width: textEdit.width + 20;
                height: textEdit.height + 20;

                color: Style.backgroundColor;

                TextEditCustom{
                    id: textEdit;

                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.horizontalCenter: parent.horizontalCenter;

                    decoratorComponent: inputsRepresentaitonPage.isQtStyle ? DecoratorsQt.textEditDecorator :
                                                                          Decorators.textEditDecorator;

                    placeHolderText: "placeHolder";
                }
            }

            ComboBox{
                id: combobox;

                anchors.verticalCenter: parent.verticalCenter;

                model: comboModel;
                placeHolderText: "ComboBox"
                decoratorComponent: inputsRepresentaitonPage.isQtStyle ? DecoratorsQt.comboBoxDecorator :
                                                                      Decorators.comboBoxDecorator;

            }



        }//row

        Row{
            id: checkRow;

            anchors.horizontalCenter: parent.horizontalCenter;
            height: 40;
            spacing: 50;

            CheckBox{
                id: checkBox;

                anchors.verticalCenter: parent.verticalCenter;

                decoratorComponent: inputsRepresentaitonPage.isQtStyle ? DecoratorsQt.checkBoxDecorator :
                                                                                            Decorators.checkBoxDecorator;
                text: "text";


                onClicked: {
                    if(checkBox.checkState !== Qt.Checked){
                        checkBox.checkState = Qt.Checked;
                    }
                    else {
                        checkBox.checkState = Qt.Unchecked;
                    }
                }
            }

            SwitchCustom{
                id: switchCustom;

                anchors.verticalCenter: parent.verticalCenter;

                decoratorComponent: inputsRepresentaitonPage.isQtStyle ? DecoratorsQt.switchDecorator :
                                                                                            Decorators.switchDecorator;
                text: "text";

            }

            Slider{
                anchors.verticalCenter: parent.verticalCenter;

                decoratorComponent: inputsRepresentaitonPage.isQtStyle ? DecoratorsQt.sliderDecorator :
                                                                                            Decorators.sliderDecorator;
                //orientation: Qt.Vertical;
                from: 10;
                to: 100;

            }

            Slider{
                anchors.verticalCenter: parent.verticalCenter;

                decoratorComponent: inputsRepresentaitonPage.isQtStyle ? DecoratorsQt.sliderDecorator :
                                                                                            Decorators.sliderDecorator;
                orientation: Qt.Vertical;
                from: 10;
                to: 100;

            }

//            Item{//testElement
//                id: testElement;

//                anchors.verticalCenter: parent.verticalCenter;

//                width: decorator ? decorator.width : 0;
//                height: decorator ? decorator.height : 0;

////                property Component decoratorComponent: inputsRepresentaitonPage.isQtStyle ? DecoratorsQt.switchDecorator :
////                                                                                            Decorators.switchDecorator;

//                property Component decoratorComponent : DecoratorsQt.sliderDecorator;
//                property var decorator : null;

//                onDecoratorComponentChanged: {
//                    if(!decoratorComponent){
//                        return;
//                    }
//                    if(decorator){
//                        decorator.destroy()
//                    }
//                    decorator = decoratorComponent.createObject(testElement)
//                    decorator.baseElement = testElement
//                    bindWidth.target = decorator
//                    bindHeight.target = decorator
//                }

//                Binding {
//                    id: bindWidth
//                    property: "width"
//                    value: testElement.width;
//                }

//                Binding {
//                    id: bindHeight
//                    property: "height"
//                    value: testElement.height;
//                }

//            }//testElement

        }//row2

    }//Column





    ///////////////////

    PauseAnimation {
        id: pauseMessage;
        function show(text) {
            messageRec.text = text
            pauseMessage.restart()
        }

        duration: 1000;
        onStarted: {
            messageRec.visible = true;
        }
        onFinished: {
            messageRec.visible = false;
        }
    }

    Rectangle{
        id: darkBackground;

        anchors.fill: parent;

        color: "#000000";
        opacity: 0.3;
        visible: false;

    }

    Rectangle{
        id: messageRec;

        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.top
        anchors.topMargin: 50

        property alias text: messageText.text

        width: parent.width - 50;
        height: 40;
        radius:4;

        visible: false;

        Text {
            id: messageText;

            anchors.centerIn: parent;

            width: parent.width - 80;
            wrapMode: Text.WordWrap;
            horizontalAlignment: Text.AlignHCenter;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_title;
            color: Style.errorTextColor;

            text: "Click!";
        }

    }
}
