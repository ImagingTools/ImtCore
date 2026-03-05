import {QtObject} from './QtObject'

export class ListElement extends QtObject  {
    constructor(args) {
        super(args)
        if(args.parent && !args.appended)
        args.parent.append(this)
    }
    $domCreate(){}
}

QML.ListElement = ListElement