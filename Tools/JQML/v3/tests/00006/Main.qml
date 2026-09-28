pragma ComponentBehavior: Bound
import QtQuick

Item {

    property string stringValue: ""
    property int intValue: 1
    property real realValue: 1.0
    property bool boolValue: true
    property var variantValue
    property color colorValue: "#ff0000"

    onStringValueChanged: {
        console.log("stringValueChanged", stringValue)
    }
    onIntValueChanged: {
        console.log("intValueChanged", intValue)
    }
    onRealValueChanged: {
        console.log("realValueChanged", realValue)
    }
    onBoolValueChanged: {
        console.log("boolValueChanged", boolValue)
    }
    onVariantValueChanged: {
        console.log("variantValueChanged", variantValue)
    }
    onColorValueChanged: {
        console.log("colorValueChanged", colorValue)
    }

    function prepare(nextBool) {
        stringValue = "sentinel"
        intValue = 42
        realValue = 42.5
        boolValue = nextBool
        variantValue = "sentinel"
        colorValue = "#ff0000"
    }

    Component.onCompleted: {
        prepare(true)
        try {
            stringValue = null
        } catch (error) {
            // console.log("error setting stringValue to null")
        }
        try {
            intValue = null
        } catch (error) {
            // console.log("error setting intValue to null")
        }
        try {
            realValue = null
        } catch (error) {
            // console.log("error setting realValue to null")
        }
        try {
            boolValue = null
        } catch (error) {
            // console.log("error setting boolValue to null")
        }
        try {
            variantValue = null
        } catch (error) {
            // console.log("error setting variantValue to null")
        }
        try {
            colorValue = null
        } catch (error) {
            // console.log("error setting colorValue to null")
        }


        prepare(false)
        try {
            stringValue = undefined
        } catch (error) {
            // console.log("error setting stringValue to undefined")
        }
        try {
            intValue = undefined
        } catch (error) {
            // console.log("error setting intValue to undefined")
        }
        try {
            realValue = undefined
        } catch (error) {
            // console.log("error setting realValue to undefined")
        }
        try {
            boolValue = undefined
        } catch (error) {
            // console.log("error setting boolValue to undefined")
        }
        try {
            variantValue = undefined
        } catch (error) {
            // console.log("error setting variantValue to undefined")
        }
        try {
            colorValue = undefined
        } catch (error) {
            // console.log("error setting colorValue to undefined")
        }


        prepare(true)
        try {
            stringValue = 0
        } catch (error) {
            // console.log("error setting stringValue to 0")
        }
        try {
            intValue = 0
        } catch (error) {
            // console.log("error setting intValue to 0")
        }
        try {
            realValue = 0
        } catch (error) {
            // console.log("error setting realValue to 0")
        }
        try {
            boolValue = 0
        } catch (error) {
            // console.log("error setting boolValue to 0")
        }
        try {
            variantValue = 0
        } catch (error) {
            // console.log("error setting variantValue to 0")
        }
        try {
            colorValue = 0
        } catch (error) {
            // console.log("error setting colorValue to 0")
        }


        prepare(false)
        try {
            stringValue = 1
        } catch (error) {
            // console.log("error setting stringValue to 1")
        }
        try {
            intValue = 1
        } catch (error) {
            // console.log("error setting intValue to 1")
        }
        try {
            realValue = 1
        } catch (error) {
            // console.log("error setting realValue to 1")
        }
        try {
            boolValue = 1
        } catch (error) {
            // console.log("error setting boolValue to 1")
        }
        try {
            variantValue = 1
        } catch (error) {
            // console.log("error setting variantValue to 1")
        }
        try {
            colorValue = 1
        } catch (error) {
            // console.log("error setting colorValue to 1")
        }

        prepare(true)
        try {
            stringValue = ''
        } catch (error) {
            // console.log("error setting stringValue to ''")
        }
        try {
            intValue = ''
        } catch (error) {
            // console.log("error setting intValue to ''")
        }
        try {
            realValue = ''
        } catch (error) {
            // console.log("error setting realValue to ''")
        }
        try {
            boolValue = ''
        } catch (error) {
            // console.log("error setting boolValue to ''")
        }
        try {
            variantValue = ''
        } catch (error) {
            // console.log("error setting variantValue to ''")
        }
        try {
            colorValue = ''
        } catch (error) {
            // console.log("error setting colorValue to ''")
        }

        prepare(false)
        try {
            stringValue = '0'
        } catch (error) {
            // console.log("error setting stringValue to '0'")
        }
        try {
            intValue = '0'
        } catch (error) {
            // console.log("error setting intValue to '0'")
        }
        try {
            realValue = '0'
        } catch (error) {
            // console.log("error setting realValue to '0'")
        }
        try {
            boolValue = '0'
        } catch (error) {
            // console.log("error setting boolValue to '0'")
        }
        try {
            variantValue = '0'
        } catch (error) {
            // console.log("error setting variantValue to '0'")
        }
        try {
            colorValue = '0'
        } catch (error) {
            // console.log("error setting colorValue to '0'")
        }


        prepare(false)
        try {
            stringValue = '1'
        } catch (error) {
            // console.log("error setting stringValue to '1'")
        }
        try {
            intValue = '1'
        } catch (error) {
            // console.log("error setting intValue to '1'")
        }
        try {
            realValue = '1'
        } catch (error) {
            // console.log("error setting realValue to '1'")
        }
        try {
            boolValue = '1'
        } catch (error) {
            // console.log("error setting boolValue to '1'")
        }
        try {
            variantValue = '1'
        } catch (error) {
            // console.log("error setting variantValue to '1'")
        }
        try {
            colorValue = '1'
        } catch (error) {
            // console.log("error setting colorValue to '1'")
        }


        prepare(true)
        try {
            stringValue = false
        } catch (error) {
            // console.log("error setting stringValue to false")
        }
        try {
            intValue = false
        } catch (error) {
            // console.log("error setting intValue to false")
        }
        try {
            realValue = false
        } catch (error) {
            // console.log("error setting realValue to false")
        }
        try {
            boolValue = false
        } catch (error) {
            // console.log("error setting boolValue to false")
        }
        try {
            variantValue = false
        } catch (error) {
            // console.log("error setting variantValue to false")
        }
        try {
            colorValue = false
        } catch (error) {
            // console.log("error setting colorValue to false")
        }

        prepare(false)
        try {
            stringValue = true
        } catch (error) {
            // console.log("error setting stringValue to true")
        }
        try {
            intValue = true
        } catch (error) {
            // console.log("error setting intValue to true")
        }
        try {
            realValue = true
        } catch (error) {
            // console.log("error setting realValue to true")
        }
        try {
            boolValue = true
        } catch (error) {
            // console.log("error setting boolValue to true")
        }
        try {
            variantValue = true
        } catch (error) {
            // console.log("error setting variantValue to true")
        }
        try {
            colorValue = true
        } catch (error) {
            // console.log("error setting colorValue to true")
        }


        prepare(false)
        try {
            stringValue = 'blue'
        } catch (error) {
            // console.log("error setting stringValue to 'blue'")
        }
        try {
            intValue = 'blue'
        } catch (error) {
            // console.log("error setting intValue to 'blue'")
        }
        try {
            realValue = 'blue'
        } catch (error) {
            // console.log("error setting realValue to 'blue'")
        }
        try {
            boolValue = 'blue'
        } catch (error) {
            // console.log("error setting boolValue to 'blue'")
        }
        try {
            variantValue = 'blue'
        } catch (error) {
            // console.log("error setting variantValue to 'blue'")
        }
        try {
            colorValue = 'blue'
        } catch (error) {
            // console.log("error setting colorValue to 'blue'")
        }

        Qt.quit()
    }
}