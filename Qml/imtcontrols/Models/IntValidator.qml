import QtQuick 2.15

QtObject {
    id: root;

    property var intRegExp: RegularExpressionValidator {
        regularExpression: /^[-+]?[0-9]+$/;
    }

    property var regExp: new RegExp(intRegExp.regularExpression)

    function isValid(value){
        return regExp.test(value)
    }
}


