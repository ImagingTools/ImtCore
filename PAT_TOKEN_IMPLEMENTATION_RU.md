# Реализация редактора PAT токенов в QML Settings

## Краткое описание
Реализован редактор персональных токенов доступа (PAT) для страницы настроек QML в приложении ImtCore.

## Что было сделано

### 1. Созданные файлы

#### PatTokenListParam.qml
- Модель данных для хранения списка PAT токенов
- Методы: добавление, удаление, обновление токенов
- Сериализация/десериализация в JSON

#### PatTokenParamEditor.qml
- Графический редактор PAT токенов
- Возможности:
  - Просмотр списка всех токенов
  - Добавление нового токена через диалог
  - Удаление токена
  - Показ/скрытие значения токена
  - Поля: Название, Значение, Описание, Дата создания

#### PatTokenSettingsController.qml
- Контроллер для управления настройками PAT токенов
- Инициализирует параметр токенов в системе настроек

### 2. Модифицированные файлы

#### SettingsController.qml
- Зарегистрирован новый редактор параметров PatTokenParamEditor
- Добавлено свойство patTokenParamEditorComp

#### ApplicationMain.qml
- Добавлен экземпляр PatTokenSettingsController
- Зарегистрирован раздел "PAT Tokens" в функции fillPreferenceParamsSet()
- Токены теперь доступны в Settings наравне с Network, General и About

#### qmldir и imtguiqml.qrc
- Зарегистрированы новые QML компоненты

## Как использовать

### Доступ к настройкам PAT токенов

1. Запустите приложение
2. Откройте Settings (PreferenceDialog)
3. Выберите раздел "PAT Tokens"
4. Добавляйте, редактируйте или удаляйте токены

### Добавление нового токена

1. Нажмите кнопку "Add PAT Token"
2. Введите название токена (например, "GitHub API Token")
3. Введите значение токена (отображается как пароль)
4. Опционально добавьте описание
5. Нажмите "Add" для сохранения

## Архитектура системы настроек

### Текущая структура

```
PreferenceDialog.qml (диалог настроек)
    ↓
SettingsController.qml (регистрирует редакторы параметров)
    ↓
ParamsSetController (контроллер группы параметров)
    ↓
ParamEditorBase (базовый класс редакторов)
    ↓
Param (модель данных параметра)
```

### Три уровня системы настроек:

1. **Контроллеры** (ParamsSetController):
   - Управляют группами параметров
   - Примеры: ClientSettingsController, UserSettingsController
   - Обрабатывают операции сохранения/загрузки

2. **Параметры** (Param):
   - Отдельные элементы настроек с типом и данными
   - Сериализуются в JSON
   - Примеры: TextParam, PasswordParam, PatTokenListParam

3. **Редакторы** (ParamEditorBase):
   - UI компоненты для редактирования параметров
   - Регистрируются по типу в SettingsController
   - Примеры: TextParamEditor, PasswordParamEditor, PatTokenParamEditor

## Предложения по упрощению работы с Settings

### 1. Создание базовых типов параметров

Создать переиспользуемые типы параметров для общих паттернов:

```qml
// ListParam.qml - Универсальный параметр-список
Param {
    property ListModel items: ListModel {}
    function addItem(itemData) { /* ... */ }
    function removeItem(index) { /* ... */ }
}
```

### 2. Упрощение регистрации контроллеров

Добавить вспомогательную функцию в ApplicationMain.qml:

```qml
function registerSettingsSection(id, name, controllerComp, condition) {
    if (condition === undefined || condition) {
        let controller = controllerComp.createObject(application)
        settingsController.registerParamsSetController(id, name, controller)
    }
}
```

### 3. Декларативное определение настроек

Создать схему настроек в декларативном стиле:

```qml
SettingsSchema {
    sections: [
        SettingsSection {
            id: "PatTokens"
            name: qsTr("PAT Tokens")
            controller: Component { PatTokenSettingsController {} }
        }
    ]
}
```

### 4. Паттерн авто-регистрации

Контроллеры могут регистрировать себя автоматически:

```qml
Component.onCompleted: {
    settingsController.registerParamsSetController("PatTokens", qsTr("PAT Tokens"), this)
}
```

### 5. Валидация настроек

Добавить систему валидации параметров:

```qml
property var validators: []
function validate() { /* проверка всех валидаторов */ }
```

### 6. Отслеживание изменений

Реализовать undo/redo для настроек:

```qml
SettingsHistoryManager {
    function recordChange(paramId, oldValue, newValue) { /* ... */ }
    function undo() { /* ... */ }
    function redo() { /* ... */ }
}
```

## Хранение данных

PAT токены хранятся в настройках приложения в формате JSON:

```json
{
    "tokens": [
        {
            "name": "GitHub API",
            "token": "ghp_xxxxxxxxxxxx",
            "description": "Токен для доступа к GitHub API",
            "createdDate": "2026-01-26T15:55:12.092Z"
        }
    ]
}
```

## Безопасность

- Токены по умолчанию отображаются как пароль
- Кнопка показать/скрыть для просмотра значений
- Рекомендуется в будущем добавить шифрование токенов
- Рекомендуется интеграция с системным хранилищем ключей

## Будущие улучшения

1. **Валидация токенов**: Добавить проверку токенов через API
2. **Срок действия**: Добавить поле даты истечения и предупреждения
3. **Права доступа**: Управление областями действия токенов
4. **Отслеживание использования**: Где и когда используются токены
5. **Безопасное хранение**: Интеграция с системным хранилищем ключей
6. **Импорт/Экспорт**: Импорт токенов из файлов
7. **Шаблоны**: Предконфигурированные шаблоны для популярных сервисов

## Тестирование

Для тестирования редактора PAT токенов:

1. Соберите приложение
2. Запустите приложение
3. Откройте Settings
4. Перейдите в раздел "PAT Tokens"
5. Протестируйте добавление, просмотр и удаление токенов
6. Проверьте, что изменения сохраняются
7. Убедитесь, что "Apply" сохраняет, а "Close" отменяет изменения

## Заключение

Реализация следует существующим паттернам ImtCore и интегрируется с текущей инфраструктурой настроек. Не требуется изменений на стороне сервера. Работает на desktop и web платформах.

Документация на английском языке доступна в файле PAT_TOKEN_IMPLEMENTATION.md.
