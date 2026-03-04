import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

ElementView {
    id: root

    controlComp: spinBoxComp
    property SpinBox spinBox: null
    property bool readOnly: false
    property real startValue: 0
    property real from: 0
    property real to: 999999
    property real stepSize: 1
    property real value: startValue

    onStartValueChanged: {
        if (spinBox){
            spinBox.startValue = startValue
        }
    }

    onValueChanged: {
        if (spinBox){
            spinBox.value = value
        }
    }

    onFromChanged: {
        if (spinBox){
            spinBox.from = from
        }
    }

    onToChanged: {
        if (spinBox){
            spinBox.to = to
        }
    }

    onStepSizeChanged: {
        if (spinBox){
            spinBox.stepSize = stepSize
        }
    }

    onReadOnlyChanged: {
        if (spinBox){
            spinBox.editable = !readOnly
        }
    }

    Component {
        id: spinBoxComp
        SpinBox {
            startValue: root.startValue
            from: root.from
            to: root.to
            value: root.value
            stepSize: root.stepSize
            editable: !root.readOnly
            Component.onCompleted: {
                root.spinBox = this
            }
            
            onValueChanged: {
                root.value = value
            }
        }
    }
}


