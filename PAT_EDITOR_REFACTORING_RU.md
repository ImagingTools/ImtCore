# Рефакторинг редактора PAT - Удаление зависимости от GraphQL

## Проблема

Редактор PAT (Personal Access Token) в QML (`PatTokenParamEditor.qml`) имел прямые зависимости от GraphQL через компоненты `GqlSdlRequestSender`. Эта тесная связь означала:

1. **Привязка к протоколу**: Редактор мог работать только с GraphQL
2. **Трудно поддерживать**: Изменения протокола требовали редактирования UI компонента
3. **Не переиспользуемый**: Другие компоненты не могли легко переиспользовать операции с PAT
4. **Трудно тестировать**: Невозможно тестировать UI без GraphQL сервера

## Решение: Паттерн Provider

Мы ввели абстракцию **Provider** следуя установленному паттерну в `imtauthgui` (например, `UserTokenProvider`, `SuperuserProvider`).

### Архитектура

```
┌─────────────────────────────────────┐
│   PatTokenParamEditor.qml           │
│   (UI Компонент - не зависит от     │
│    протокола)                       │
│   - Использует сигналы и функции    │
└───────────────┬─────────────────────┘
                │
                │ Использует интерфейс
                ▼
┌─────────────────────────────────────┐
│   PatTokenProvider.qml              │
│   (Слой абстракции)                 │
│   - Предоставляет: сигналы и функции│
│   - Скрывает: реализацию протокола  │
└───────────────┬─────────────────────┘
                │
                │ Реализовано с помощью
                ▼
┌─────────────────────────────────────┐
│   GqlSdlRequestSender               │
│   (Реализация GraphQL)              │
│   - Может быть заменен на REST/gRPC │
└─────────────────────────────────────┘
```

## Детали реализации

### PatTokenProvider.qml

Провайдер предоставляет чистый интерфейс:

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

### PatTokenParamEditor.qml

Редактор теперь:
1. Создает экземпляр `PatTokenProvider`
2. Подключается к сигналам провайдера для получения результатов
3. Вызывает функции провайдера для операций
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
property PatTokenProvider patTokenProvider: PatTokenProvider {
    onTokenCreated: function(success, message, token) {
        if (success) {
            // обработка успеха
        } else {
            ModalDialogManager.showErrorDialog(message)
        }
    }
}

function addNewToken(name, description, scopes, expiresAt) {
    patTokenProvider.createToken(
        AuthorizationController.getUserId(),
        name,
        description,
        scopes,
        expiresAt
    )
}
```

## Преимущества

1. **Независимость от протокола**: Можно переключиться с GraphQL на REST/gRPC, изменив только провайдер
2. **Разделение ответственности**: Логика UI отделена от логики протокола
3. **Тестируемость**: Можно создать mock провайдеры для тестирования
4. **Переиспользуемость**: Другие компоненты могут использовать тот же провайдер
5. **Поддерживаемость**: Изменения протокола не влияют на код UI

## Переход на другой протокол

Чтобы использовать другой протокол (например, REST):

1. Создайте `PatTokenRestProvider.qml` с тем же интерфейсом
2. Реализуйте функции используя REST API вместо GraphQL
3. Замените `PatTokenProvider` на `PatTokenRestProvider` в редакторе

**Изменения в PatTokenParamEditor.qml не нужны!**

## Измененные файлы

- **Qml/imtauthgui/PatTokenProvider.qml** (новый) - Абстракция провайдера
- **Qml/imtgui/Params/PatTokenParamEditor.qml** - Рефакторинг для использования провайдера
- **Qml/imtauthgui/qmldir** - Зарегистрирован компонент провайдера
- **Qml/imtauthgui/imtauthguiqml.qrc** - Добавлен провайдер в ресурсы

## Тестирование

Рефакторинг сохраняет ту же функциональность:
1. Загрузка списка токенов при инициализации компонента
2. Создание новых токенов с обработкой успеха/ошибок
3. Удаление токенов с подтверждением
4. Отзыв токенов
5. Отображение деталей токенов в UI

Все операции работают идентично, но теперь через слой абстракции провайдера.
