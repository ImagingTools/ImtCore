import QtQuick

// topMargin / bottomMargin / leftMargin / rightMargin of Flickable and ListView: the range of contentX / contentY
// widened by them (ScreenMobileComponents of QmlAppTemplate: a list 16 px under the header)
Item {
    width: 200; height: 200
    Flickable {
        id: f
        width: 100; height: 100
        contentWidth: 150; contentHeight: 300
        topMargin: 10; bottomMargin: 20; leftMargin: 5; rightMargin: 7
    }
    ListView {
        id: lv
        y: 100; width: 100; height: 100
        topMargin: 16; bottomMargin: 8
        model: 10
        delegate: Rectangle { width: 100; height: 30 }
    }
    Component.onCompleted: {
        console.log(f.contentY, f.contentX, f.topMargin, f.bottomMargin, f.atYBeginning, f.originY)
        console.log(lv.contentY, lv.originY, lv.topMargin, lv.atYBeginning, lv.contentHeight, lv.itemAtIndex(0).y)
        lv.positionViewAtEnd()
        console.log(lv.contentY, lv.atYEnd)
        lv.positionViewAtBeginning()
        console.log(lv.contentY, lv.atYBeginning)
        f.contentY = 1000; f.returnToBounds()
        console.log(f.contentY)
        f.contentX = -100; f.returnToBounds()
        console.log(f.contentX)
        f.flick(0, 5000)
        Qt.quit()
    }
}
