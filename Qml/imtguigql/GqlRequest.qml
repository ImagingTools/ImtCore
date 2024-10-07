import QtQuick 2.12

QtObject {
    id: root;

    property string json;
    property string state;

    function setGqlQuery(gqlData, headers){
        root.state = "Loading"

        let xhr = new XMLHttpRequest
        xhr.open("POST", "../../graphql")

        if(headers){
            for(let name in headers){
                xhr.setRequestHeader(name, headers[name])
            }
        }

        xhr.send(gqlData)

        xhr.onreadystatechange = function(){
            console.log("onreadystatechange", xhr.readyState, xhr.responseText);

            if (xhr.readyState === XMLHttpRequest.DONE){
                root.json = xhr.responseText;

                root.state = "Ready";
            }
        }
    }
}
