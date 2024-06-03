import QtQuick 2.15

QtObject {
    id: root;

    property var doubleRegExp: RegularExpressionValidator {
        regularExpression: /^[-+]?(?:0|[1-9][0-9]*)\.[0-9]+$/;
    }

    property var regExp: new RegExp(doubleRegExp.regularExpression)

    function isValid(value){
        return regExp.test(value)
    }
}


