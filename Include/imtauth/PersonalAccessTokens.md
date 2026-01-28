# Personal Access Token Management

## Overview

The Personal Access Token (PAT) management system provides secure, long-lived API authentication tokens for the imtauth library. This feature allows users to generate tokens that can be used to authenticate API requests without exposing their primary credentials.

## Features

- **Secure Token Generation**: Tokens are generated using cryptographically secure random data
- **Token Hashing**: Tokens are stored as SHA-256 hashes for security
- **Scoped Permissions**: Each token can have specific permission scopes
- **Expiration Support**: Tokens can have optional expiration dates
- **Revocation**: Tokens can be revoked at any time
- **Last Used Tracking**: The system tracks when each token was last used

## Architecture

### Core Components

#### Interfaces
- **IPersonalAccessToken**: Interface for token data objects
  - Token ID, user ID, name, description
  - Scopes (permissions)
  - Creation, last used, and expiration timestamps
  - Revocation status

- **IPersonalAccessTokenManager**: Interface for token management
  - Create, validate, revoke, and delete tokens
  - Retrieve tokens by ID or user ID

#### Implementations
- **CPersonalAccessToken**: Data class implementing IPersonalAccessToken
- **CPersonalAccessTokenManagerComp**: Manager component with:
  - Secure token generation (`imt_pat_` prefix + base64-encoded random data)
  - SHA-256 hashing for storage
  - Token validation logic

### Database Layer

#### Database Delegates
- **CPersonalAccessTokenDatabaseDelegateComp**: Base database delegate
- **CSqlitePersonalAccessTokenDatabaseDelegateComp**: SQLite-specific implementation
  - Creates `PersonalAccessTokens` table
  - Indexes on user_id, token_hash, and created_at for performance
  - Foreign key constraint to Users table

### GraphQL Layer

#### Controllers
- **CPersonalAccessTokenCollectionControllerComp**: GraphQL controller
  - Handles CreatePersonalAccessToken mutation
  - Handles RevokePersonalAccessToken mutation
  - Handles ValidatePersonalAccessToken query
  - Handles list, get, update, delete operations

#### Metadata
- **CPersonalAccessTokenMetaInfoCreatorComp**: Creates metadata for tokens
- **CPersonalAccessTokenMetaInfoDelegateComp**: Metadata delegate for GraphQL

## API Usage

### GraphQL Schema

The PersonalAccessTokens.sdl schema defines the following operations:

#### Mutations

**CreatePersonalAccessToken**: Create a new token
```graphql
mutation {
  CreatePersonalAccessToken(input: {
    userId: "user123"
    name: "My API Token"
    description: "Token for CI/CD pipeline"
    scopes: ["read:users", "write:data"]
    expiresAt: "2024-12-31T23:59:59Z"
  }) {
    id
    token
    success
    message
  }
}
```

**Important**: The `token` field in the response contains the actual token value. This is the **only time** the token will be shown. Save it securely!

**RevokePersonalAccessToken**: Revoke an existing token
```graphql
mutation {
  RevokePersonalAccessToken(input: {
    id: "token-id-here"
  }) {
    success
    message
  }
}
```

#### Queries

**PersonalAccessTokensList**: List all tokens for a user
```graphql
query {
  PersonalAccessTokensList(input: {
    userId: "user123"
    viewParams: {
      limit: 10
      offset: 0
    }
  }) {
    items {
      id
      userId
      name
      description
      scopes
      createdAt
      lastUsedAt
      expiresAt
      revoked
    }
  }
}
```

**ValidatePersonalAccessToken**: Validate a token
```graphql
query {
  ValidatePersonalAccessToken(input: {
    token: "imt_pat_..."
  }) {
    valid
    userId
    scopes
    message
  }
}
```

### C++ API Usage

```cpp
// Get the token manager
imtauth::IPersonalAccessTokenManager* tokenManager = ...;

// Create a token
QByteArray userId = "user123";
QString name = "My API Token";
QString description = "Token for automated scripts";
QByteArrayList scopes;
scopes << "read:api" << "write:data";
QDateTime expiresAt = QDateTime::currentDateTime().addDays(90);

auto result = tokenManager->CreateToken(
    userId, name, description, scopes, expiresAt);

if (result.success) {
    qDebug() << "Token created:" << result.tokenId;
    qDebug() << "Raw token (save this!):" << result.rawToken;
}

// Validate a token
QByteArray userId;
QByteArrayList scopes;
bool isValid = tokenManager->ValidateToken(rawToken, userId, scopes);

// Revoke a token
tokenManager->RevokeToken(tokenId);

// Delete a token
tokenManager->DeleteToken(tokenId);
```

## Security Considerations

### Token Storage
- Tokens are **never stored in plaintext**
- Only SHA-256 hashes are stored in the database
- The raw token is shown only once at creation time

### Token Format
- Tokens use the prefix `imt_pat_` for easy identification
- Contains 32 bytes of cryptographically secure random data
- Encoded in base64url format for safe transmission

### Best Practices
1. **Treat tokens like passwords**: Never commit them to source control
2. **Use scoped permissions**: Grant only the minimum required permissions
3. **Set expiration dates**: Use short-lived tokens when possible
4. **Revoke unused tokens**: Regular cleanup of old/unused tokens
5. **Monitor usage**: Track the `lastUsedAt` field for anomalies

### Validation
- Expired tokens are automatically rejected
- Revoked tokens are immediately invalidated
- Token validation is constant-time to prevent timing attacks

## Database Schema

```sql
CREATE TABLE PersonalAccessTokens (
    Id TEXT PRIMARY KEY NOT NULL,
    TypeId TEXT NOT NULL,
    Name TEXT,
    Description TEXT,
    UserId TEXT NOT NULL,
    TokenHash TEXT NOT NULL,
    Scopes TEXT,
    CreatedAt TEXT NOT NULL,
    LastUsedAt TEXT,
    ExpiresAt TEXT,
    Revoked INTEGER DEFAULT 0,
    FOREIGN KEY(UserId) REFERENCES Users(Id) ON DELETE CASCADE
);

CREATE INDEX idx_pat_user_id ON PersonalAccessTokens(UserId);
CREATE INDEX idx_pat_token_hash ON PersonalAccessTokens(TokenHash);
CREATE INDEX idx_pat_created_at ON PersonalAccessTokens(CreatedAt);
```

## Testing

Unit tests are provided in `ImtCore/Tests/PersonalAccessTokenTest/`:
- Token creation
- Token validation
- Token revocation
- Token expiration

Run tests to verify the implementation:
```bash
# Build and run tests (adjust path as needed)
cmake --build . --target PersonalAccessTokenTest
./PersonalAccessTokenTest
```

## Future Enhancements

Potential improvements for future versions:
- Rate limiting per token
- IP address restrictions
- Token usage analytics
- Automatic token rotation
- Multiple token formats (JWT-based PATs)
- Fine-grained permission system integration
