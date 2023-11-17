import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0


Rectangle {
    id: inputsRepresentaitonPage;

    anchors.fill: parent;

    Component.onCompleted: {
        spinBox.forceActiveFocus();
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

                placeHolderText: "TextField";

                KeyNavigation.right: textEdit;
                KeyNavigation.tab: textEdit;

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

                    placeHolderText: "TextEdit";

                    KeyNavigation.right: textArea;
                    KeyNavigation.tab: textArea;
                }
            }

            Rectangle{
                id: textAreaBackground;

                anchors.verticalCenter:  parent.verticalCenter;

                width: textArea.width + 20;
                height: textArea.height + 20;

                color: Style.backgroundColor;

                TextArea{
                    id: textArea;

                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.horizontalCenter: parent.horizontalCenter;

                    placeHolderText: "TextArea";

                    KeyNavigation.right: combobox;
                    KeyNavigation.tab: combobox;
                }
            }

            ComboBox{
                id: combobox;

                anchors.verticalCenter: parent.verticalCenter;

                model: comboModel;
                placeHolderText: "ComboBox"

                KeyNavigation.right: textField;
                KeyNavigation.tab: textField;

            }


        }//row - inputsRow

        Row{
            id: sliderRow;

            anchors.horizontalCenter: parent.horizontalCenter;
            height: 40;
            spacing: 50;

            Slider{
                anchors.verticalCenter: parent.verticalCenter;

                from: 10;
                to: 100;
                value: 50;

            }

            Slider{
                anchors.verticalCenter: parent.verticalCenter;

                orientation: Qt.Vertical;
                from: 10;
                to: 100;

            }

            RangeSlider{
                //orientation: Qt.Vertical;
                from: 10;
                to: 100;
                valueSecond: 80;

            }

        }//row2 - sliderRow

        Row{
            id: checkRow;

            anchors.horizontalCenter: parent.horizontalCenter;
            height: 40;
            spacing: 50;

            CheckBox{
                id: checkBox;

                anchors.verticalCenter: parent.verticalCenter;

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

                text: "text";

            }

            RadioButton{

                anchors.verticalCenter: parent.verticalCenter;

                autoExclusive: false;
                text: "radioButton";
            }

//            RadioButton{
//                anchors.verticalCenter: parent.verticalCenter;

//                autoExclusive: false;
//                text: "radioButton2";
//            }

        }//row3 - checkRow

        Row{
            id: boxRow;

            anchors.horizontalCenter: parent.horizontalCenter;
            height: 40;
            spacing: 50;

            Rectangle{
                id: spinBoxContainer;

                anchors.verticalCenter: parent.verticalCenter;

                width: 160;
                height: 60;

                radius: 4;
                color: "transparent";
                //border.color: "lightgreen";

                SpinBox{
                    id: spinBox;

                    anchors.centerIn: parent;
                }
            }

        }//row4 - boxRow

    }//Column

}
