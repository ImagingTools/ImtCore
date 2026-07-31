import QtQuick 2.15

QtObject {
    id: root;

    property var macAddressRegExp: RegularExpressionValidator {
        regularExpression: /^([0-9A-Fa-f]{2}[:]){5}([0-9A-Fa-f]{2})$/;
    }

    property var regExp: new RegExp(macAddressRegExp.regularExpression)

    function convert(text){
        let macAddress = text
        let len = macAddress.length;
        let maxLen = 17;

        if(len >= 2 && len < maxLen){
            while(!(macAddress.indexOf(":") < 0)){
                macAddress = macAddress.replace(":", "")
            }

            let resultMacAddress = "";

            for (let i = 1; i <= macAddress.length; i++){
                resultMacAddress += macAddress[i - 1];

                if (i % 2 == 0 && i != 12){
                    resultMacAddress += ":";
                }
            }

            return resultMacAddress;
        }

        return text;
    }

    function isValid(macAddress){
        return regExp.test(macAddress)
    }
}


