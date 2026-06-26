import QtQuick
import imtcontrols 1.0

BaseClass {
    property string m_name: 'Hello'
    function getJSONKeyForProperty(propertyId){
        switch (propertyId){
            case 'm_name': return 'name'
        }
    }
    function getPropertyType(propertyId){
        switch (propertyId){
            case 'm_name': return 'string'
        }
    }
}