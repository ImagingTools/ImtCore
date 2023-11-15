const { Item } = require('./Item')
const { QVar, QBool, QString } = require('../utils/properties')

class FileDialog extends Item {
    static defaultProperties = {
        selectMultiple: { type: QBool, value: false },
        defaultSuffix: { type: QString, value: '' },
        title: { type: QString, value: '' },
        nameFilters: { type: QVar, value: undefined },
        fileUrl: { type: QString, value: '' },
        fileUrls: { type: QVar, value: undefined },
        folder: { type: QString, value: '' },
        selectExisting: { type: QBool, value: false },
        shortcuts: { type: QVar, value: undefined },

    }

    /*
    this.$cP('selectMultiple', false, this.$selectMultipleChanged)
        this.$cP('defaultSuffix', '')
        this.$cP('title', '')
        this.$cP('nameFilters', [], this.$nameFiltersChanged)
        this.$cP('fileUrl', '')
        this.$cP('fileUrls', [])
        this.$cP('folder', '')
        this.$cP('selectExisting', false)
        this.$cP('shortcuts', {
            'desktop': '',
            'documents': '',
            'home': '',
            'music': '',
            'movies': '',
            'pictures': '',
        })

    */

    static defaultSignals = {
        accepted: { params: [] },
        rejected: { params: [] },


    }

    // not implemented yet
 
}

module.exports.FileDialog = FileDialog