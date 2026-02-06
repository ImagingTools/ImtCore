# API Test Utilities

This directory is a placeholder for API testing utilities and helpers.

## Purpose

When you need to add common utilities for API testing (e.g., authentication helpers, request builders, response validators), place them here.

## Example Structure

```
API/
├── README.md              # This file
├── auth-helper.js         # Authentication utilities
├── request-builder.js     # API request builders
└── validators.js          # Response validators
```

## Usage

API tests use Newman (Postman CLI) by default. Postman collections placed in the container's `/app/tests/API/` directory will be automatically detected and executed.

If you need custom JavaScript utilities for Newman tests, you can add them here and reference them from your Postman collections or pre-request scripts.
