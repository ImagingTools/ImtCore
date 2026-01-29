# Рефакторинг редактора PAT - Удаление зависимости от GraphQL

## Проблема

Редактор PAT (Personal Access Token) в QML (`PatTokenParamEditor.qml`) имел прямые зависимости от GraphQL через компоненты `GqlSdlRequestSender`. Эта тесная связь означала:

1. **Привязка к протоколу**: Редактор мог работать только с GraphQL
2. **Трудно поддерживать**: Изменения протокола требовали редактирования UI компонента
3. **Не переиспользуемый**: Другие компоненты не могли легко переиспользовать операции с PAT
4. **Трудно тестировать**: Невозможно тестировать UI без GraphQL сервера

## Решение: Паттерн Controller

Мы ввели паттерн **Controller** следуя установленной архитектуре в кодовой базе (например, `CommandsController`/`GqlBasedCommandsController`, `CollectionDataController`/`GqlBasedCollectionDataController`).

### Архитектура

```
┌─────────────────────────────────────┐
│   PatTokenParamEditor.qml           │
│   (UI Компонент - не зависит от     │
│    протокола)                       │
│   - Использует сигналы и функции    │
└───────────────┬─────────────────────┘
                │
                │ Использует контроллер
                ▼
┌─────────────────────────────────────┐
│   GqlBasedPatTokenController.qml    │
│   (в модуле imtguigql)              │
│   - Реализация GraphQL              │
└───────────────┬─────────────────────┘
                │
                │ Наследуется от
                ▼
┌─────────────────────────────────────┐
│   PatTokenController.qml            │
│   (в модуле imtauthgui)             │
│   - Интерфейс без протокола         │
│   - Определяет сигналы и функции    │
└─────────────────────────────────────┘
```

## Детали реализации

### PatTokenController.qml (Базовый)

Базовый контроллер определяет интерфейс без привязки к протоколу:

**Сигналы** (для результатов):
- `tokenListReceived(var tokenList)` - Список токенов загружен
- `tokenCreated(bool success, string message, string token)` - Токен создан
- `tokenDeleted(bool success, string message)` - Токен удален
- `tokenRevoked(bool success, string message)` - Токен отозван

**Функции** (для операций):
- `getTokenList(userId)` - Получить токены пользователя
- `createToken(userId, name, description, scopes, expiresAt)` - Создать новый токен
- `deleteToken(tokenId)` - Удалить токен навсегда
- `revokeToken(tokenId)` - Отозвать токен (оставить для аудита)

Реализации по умолчанию выдают предупреждения и вызывают сигналы со значениями по умолчанию.

### GqlBasedPatTokenController.qml (Реализация)

Контроллер на основе GraphQL:
1. **Наследуется** от `PatTokenController`
2. **Переопределяет** все функции с реализациями GraphQL
3. **Использует** `GqlSdlRequestSender` для связи с GraphQL
4. **Находится** в модуле `Qml/imtguigql/` (где находятся все GQL реализации)

### PatTokenParamEditor.qml (Потребитель)

Редактор теперь:
1. Создает экземпляр `GqlBasedPatTokenController`
2. Подключается к сигналам контроллера для получения результатов
3. Вызывает функции контроллера для операций
4. **Не знает ничего** о GraphQL или любом другом протоколе

**До рефакторинга:**
```qml
GqlSdlRequestSender {
    id: createTokenRequest
    gqlCommandId: ImtauthPersonalAccessTokensSdlCommandIds.s_createToken
    requestType: 1
    sdlObjectComp: Component {
        CreateTokenPayload {
            onFinished: {
                if (m_success) {
                    // обработка успеха
                }
            }
        }
    }
}
```

**После рефакторинга:**
```qml
property GqlBasedPatTokenController patTokenController: GqlBasedPatTokenController {
    onTokenCreated: function(success, message, token) {
        if (success) {
            // обработка успеха
        } else {
            ModalDialogManager.showErrorDialog(message)
        }
    }
}

function addNewToken(name, description, scopes, expiresAt) {
    patTokenController.createToken(
        AuthorizationController.getUserId(),
        name,
        description,
        scopes,
        expiresAt
    )
}
```

## Преимущества

1. **Независимость от протокола**: Можно переключиться с GraphQL на REST/gRPC, создав новую реализацию контроллера
2. **Разделение ответственности**: Логика UI отделена от логики протокола
3. **Тестируемость**: Можно создать mock контроллеры для тестирования
4. **Переиспользуемость**: Другие компоненты могут использовать тот же контроллер
5. **Поддерживаемость**: Изменения протокола не влияют на код UI
6. **Согласованность паттерна**: Следует установленной архитектуре в кодовой базе

## Переход на другой протокол

Чтобы использовать другой протокол (например, REST):

1. Создайте `RestBasedPatTokenController.qml`, который наследуется от `PatTokenController`
2. Реализуйте функции используя REST API вызовы вместо GraphQL
3. Замените `GqlBasedPatTokenController` на `RestBasedPatTokenController` в редакторе

**Изменения в PatTokenParamEditor.qml или PatTokenController.qml не нужны!**

## Измененные файлы

- **Qml/imtauthgui/PatTokenController.qml** (новый) - Интерфейс базового контроллера
- **Qml/imtguigql/GqlBasedPatTokenController.qml** (новый) - Реализация GraphQL
- **Qml/imtgui/Params/PatTokenParamEditor.qml** - Рефакторинг для использования контроллера
- **Qml/imtauthgui/qmldir** - Зарегистрирован базовый контроллер
- **Qml/imtauthgui/imtauthguiqml.qrc** - Добавлен контроллер в ресурсы
- **Qml/imtguigql/qmldir** - Зарегистрирован GQL контроллер
- **Qml/imtguigql/imtguigqlqml.qrc** - Добавлен GQL контроллер в ресурсы

## Тестирование

Рефакторинг сохраняет ту же функциональность:
1. Загрузка списка токенов при инициализации компонента
2. Создание новых токенов с обработкой успеха/ошибок
3. Удаление токенов с подтверждением
4. Отзыв токенов
5. Отображение деталей токенов в UI

Все операции работают идентично, но теперь через слой абстракции контроллера.

## Согласованность паттерна

Этот рефакторинг следует паттерну **Controller**, уже установленному в кодовой базе:

| Базовый контроллер | Реализация GQL | Расположение |
|-------------------|----------------|--------------|
| CommandsController | GqlBasedCommandsController | Qml/imtguigql/ |
| CollectionDataController | GqlBasedCollectionDataController | Qml/imtguigql/ |
| DataModelController | GqlBasedDataModelController | Qml/imtguigql/ |
| **PatTokenController** | **GqlBasedPatTokenController** | **Qml/imtguigql/** |
