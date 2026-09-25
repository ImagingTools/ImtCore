const ListModel = require('./Models/ListModel')
const Bool = require('./Bool')
const Var = require('./Var')
const Signal = require('./Signal')
const QtFunctions = require('../Qt/functions')

class BaseModel extends ListModel {
  static meta = Object.assign({}, ListModel.meta, {
    dynamicRoles: {type: Bool, value: true},
    owner: {type: Var, value: null},

    internalModelChanged: {type: Signal, args: ['name', 'sender']},
    finished: {type: Signal, args: []},
  })

  SLOT_ownerChanged() {
    for (let i = 0; i < this.count; i++) {
      let item = this.get(i).item
      if (item !== null && item !== undefined) {
        item.owner = this.owner
      }
    }
  }

  escapeSpecialChars(jsonString) {
    return jsonString.replace(/\\/g, '\\\\')
        .replace(/\"/g, '\\"')
        .replace(/\n/g, '\\n')
        .replace(/\r/g, '\\r')
        .replace(/\t/g, '\\t')
        .replace(/\f/g, '\\f')
  }

  getProperties(item) {
    if (item === null || item === undefined) {
      return []
    }
    return item.getProperties()
  }

  hasNullElements() {
    for (let i = 0; i < this.count; ++i) {
      let item = this.get(i).item
      if (item === null || item === undefined) {
        return true
      }
    }

    return false
  }

  toJson() {
    let json = '['
    for (let i = 0; i < this.count; i++) {
      let item = this.get(i).item
      if (i > 0) {
        json += ','
      }
      if (item === null || item === undefined) {
        json += 'null'
        continue
      }
      let list = this.getProperties(item)
      for (let propertyId of list) {
        if (item.isArrayValueValid &&
            !item.isArrayValueValid(propertyId, item[propertyId])) {
          return ''
        }
      }

      json += '{'
      let isFirst = true
      for (let key of list) {
        if (!isFirst) json += ','
          isFirst = false
          if (item[key] == null) {
            json += '"' + item.getJSONKeyForProperty(key) + '": null'
          }
        else if (typeof item[key] === 'object') {
          if (Array.isArray(item[key])) {
            json += '"' + item.getJSONKeyForProperty(key) + '":'

            json += '['

            for (let k = 0; k < item[key].length; k++) {
              if (k != 0) {
                json += ', '
              }

              if (typeof item[key][k] === 'string') {
                json += '"' + this.escapeSpecialChars(item[key][k]) + '"'
              } else {
                json += item[key][k]
              }
            }

            json += ']'
          } else if (typeof item[key].toJson === 'function') {
            let serializedValue = item[key].toJson()
            if (serializedValue === '') {
              return ''
            }
            json +=
                '"' + item.getJSONKeyForProperty(key) + '":' + serializedValue
          }
        } else {
          let value = item[key] if (value === undefined) {
            value = null
          }
          let safeValue = item[key] if (typeof safeValue === 'string') {
            safeValue = this.escapeSpecialChars(safeValue)
          }

          json += '"' + item.getJSONKeyForProperty(key) + '":' +
              (typeof item[key] === 'string' ? '"' + safeValue + '"' : value)
        }
      }
      json += '}'
    }
    json += ']'
    return json
  }

  toGraphQL() {
    let graphQL = '['
    for (let i = 0; i < this.count; i++) {
      let item = this.get(i).item
      if (i > 0) {
        graphQL += ','
      }
      if (item === null || item === undefined) {
        graphQL += 'null'
        continue
      }
      let list = this.getProperties(item)
      for (let propertyId of list) {
        if (item.isArrayValueValid &&
            !item.isArrayValueValid(propertyId, item[propertyId])) {
          return ''
        }
      }

      graphQL += '{'
      let isFirst = true
      for (let key of list) {
        if (!isFirst) graphQL += ','
          isFirst = false
          if (item[key] == null) {
            graphQL += item.getJSONKeyForProperty(key) + ':null'
          }
        else if (typeof item[key] === 'object') {
          if (Array.isArray(item[key])) {
            graphQL += item.getJSONKeyForProperty(key) + ':'

            graphQL += '['

            for (let k = 0; k < item[key].length; k++) {
              if (k != 0) {
                graphQL += ', '
              }

              if (typeof item[key][k] === 'string') {
                graphQL += '"' + this.escapeSpecialChars(item[key][k]) + '"'
              } else {
                graphQL += item[key][k]
              }
            }

            graphQL += ']'
          } else {
            let serializedValue = item[key].toGraphQL()
            if (serializedValue === '') {
              return ''
            }
            graphQL += item.getJSONKeyForProperty(key) + ':' + serializedValue
          }
        } else {
          let value = item[key] if (value === undefined) {
            value = null
          }

          graphQL += item.getJSONKeyForProperty(key) + ':' +
              (typeof item[key] === 'string' ?
                   '"' + this.escapeSpecialChars(item[key]) + '"' :
                   value)
        }
      }
      graphQL += '}'
    }
    graphQL += ']'
    return graphQL
  }

  isEqualWithModel(model) {
    if (typeof this != typeof model) {
      return false;
    }

    if (this.count !== model.count) {
      return false;
    }

    for (let i = 0; i < this.count; i++) {
      let item1 = this.get(i).item
      let item2 = model.get(i).item
      let item1IsNull = item1 === null || item1 === undefined
      let item2IsNull = item2 === null || item2 === undefined
      if (item1IsNull !== item2IsNull) {
        return false
      }
      if (item1IsNull) {
        continue
      }

      let list1 = this.getProperties(item1)
      let list2 = model.getProperties(item2)

      for (let j = 0; j < list1.length; j++) {
        let key = list1[j]

            if (!list2.includes(key)) {
          return false;
        }

        if (typeof item1[key] !== typeof item2[key]) {
          return false;
        }

        if (typeof item1[key] === 'object') {
          if (item1[key] && item1[key].isEqualWithModel) {
            let ok = item1[key].isEqualWithModel(item2[key])
            if (!ok) {
              return false
            }
          } else if (item1[key] !== item2[key]) {
            return false
          }
        } else {
          if (item1[key] !== item2[key]) {
            return false
          }
        }
      }
    }

    return true;
  }

  copyMe() {
    let retVal = BaseModel.create()
    if (!retVal) {
      return null
    }

    for (let i = 0; i < this.count; i++) {
      let item = this.get(i).item
      retVal.addElement(
          item === null || item === undefined ? null : item.copyMe())
    }

    return retVal
  }

  copyFrom(sourceObject) {
    for (let i = 0; i < sourceObject.count; i++) {
      let item = sourceObject.get(i).item this.addElement(
          item === null || item === undefined ? null : item.copyMe())
    }

    return true
  }

  createFromJson(json) {
    return this.fromJSON(json);
  }

  fromJSON(json) {
    let arr = JSON.parse(json)
    return this.fromObject(arr)
  }

  fromObject(sourceObject) {
    this.clear()

    for (let i = 0; i < sourceObject.length; i++) {
      if (sourceObject[i] === null || sourceObject[i] === undefined) {
        this.addElement(null)
        continue
      }
      let sourceTypename
      if (sourceObject[i]['__typename']) {
        sourceTypename = sourceObject[i]['__typename']
      }
      else {continue} let obj =
          QtFunctions.createComponent(sourceTypename + '.qml')
              .createObject(this)
      if (!obj || !obj.fromObject(sourceObject[i])) {
        if (obj) {
          obj.destroy()
        }
        this.clear()

        return false
      }
      this.addElement(obj)
    }

    this.finished()

    return true
  }

  addElement(element) {
    if (element !== null && element !== undefined) {
      element.owner = this.owner
    }
    this.append({item: element})
    if (this.owner) {
      if (this.owner._internal && this.owner._internal.isTransaction) {
        this.owner._internal.countChanges++
      } else {
        this.owner.modelChanged([])
      }
    }
  }

  removeElement(index) {
    this.remove(index)
    if (this.owner) {
      if (this.owner._internal && this.owner._internal.isTransaction) {
        this.owner._internal.countChanges++
      } else {
        this.owner.modelChanged([])
      }
    }
  }

  getItemsCount() {
    return this.count
  }

  containsKey(key, index) {
    if (index === undefined) {
      index = 0
    }
    if (index < 0 || index >= this.count) {
      return false
    }
    let item = this.get(index).item
    return item !== null && item !== undefined && item[key] !== undefined
  }

  getData(key, index) {
    if (index === undefined) {
      index = 0
    }
    if (index < 0 || index >= this.count) {
      return undefined
    }
    let item = this.get(index).item
    return item === null || item === undefined ? item : item[key]
  }

  setProperty(index, propName, value) {
    let item = this.get(index).item
    if (item === null || item === undefined) {
      return
    }
    if (item[propName] !== value) {
      item[propName] = value
    }
  }

  swapItems(index1, index2) {
    if (index1 < 0 || index1 >= this.count || index2 < 0 ||
        index2 >= this.count) {
      return false
    }

    let sourceItem1 = this.get(index1).item
    let sourceItem2 = this.get(index2).item
    let item1 = sourceItem1 === null || sourceItem1 === undefined ?
        null :
        sourceItem1.copyMe()
    let item2 = sourceItem2 === null || sourceItem2 === undefined ?
        null :
        sourceItem2.copyMe()
    if (item1 !== null) {
      item1.owner = this.owner
    }
    if (item2 !== null) {
      item2.owner = this.owner
    }

    this.get(index1).item = item2 this.get(index2).item = item1

    return true
  }

  insertElement(index, element) {
    if (element !== null && element !== undefined) {
      element.owner = this.owner
    }
    this.insert(index, {item: element})
    if (this.owner) {
      if (this.owner._internal && this.owner._internal.isTransaction) {
        this.owner._internal.countChanges++
      } else {
        this.owner.modelChanged([])
      }
    }
  }
}

module.exports = BaseModel
