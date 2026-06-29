import QtQuick
import imtcontrols 1.0

Item {
    width: 500
    height: 500

    BaseClass {
        id: containerOrigin
        property Storage m_storage

        function getJSONKeyForProperty(propertyId){
            switch (propertyId){
                case 'm_storage': return 'storage'
            }
        }

        function getPropertyType(propertyId){
            switch (propertyId){
                case 'm_storage': return 'Storage'
            }
        }

        function createComponent(propertyId, typename){
            let retVal;
            switch (propertyId){
                case 'm_storage': return Qt.createComponent('Storage.qml')
            }
        }
    }

    BaseClass {
        id: container
        property Storage m_storage

        function getJSONKeyForProperty(propertyId){
            switch (propertyId){
                case 'm_storage': return 'storage'
            }
        }

        function getPropertyType(propertyId){
            switch (propertyId){
                case 'm_storage': return 'Storage'
            }
        }

        function createComponent(propertyId, typename){
            let retVal;
            switch (propertyId){
                case 'm_storage': return Qt.createComponent('Storage.qml')
            }
        }
    }

    BaseModel {
        id: testModel
        

    }

    ListView {
        id: listView
        width: 500
        height: 500
        // model: testModel
        spacing: 10

        delegate: Rectangle {
            width: 500
            height: 30
            color:'green'
           
            Text {
                text: model.item.m_storage.m_name

                onTextChanged: {
                    console.log('onTextChanged', text)
                }
            }
        }
    }

    Component.onCompleted: {
        container.fromJSON('{"storage": {"name": "Hello"}}')
        containerOrigin.fromJSON('{"storage": {"name": "Blablabla"}}')

        testModel.addElement(container)
        listView.model = testModel
        container.m_storage.m_name = 'World'
        container.copyFrom(containerOrigin)

        Qt.quit();
    }
}