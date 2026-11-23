import QtQuick

// an alias of the data of an inner item (MediaCardBackground of QmlAppTemplate)
Item {
    property alias media: mediaContainer.data
    default property alias content: contentContainer.data
    width: 100; height: 80
    Item { id: mediaContainer; objectName: "mediaContainer"; width: 100; height: 40 }
    Item { id: contentContainer; objectName: "contentContainer"; y: 40; width: 100; height: 40 }
}
