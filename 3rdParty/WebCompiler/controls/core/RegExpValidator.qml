Object {
    property string regExp: '';

    onCompleted: {
        
    }

    onRegExpChanged:{
        let temp = this.regExp.split('/')

        this.__re = new RegExp(temp[1], temp[2])
        console.log(temp, this.__re)
    }
}