# imtlicgql Module

## Overview

The **imtlicgql** module provides a GraphQL-based web API layer over the imtlic licensing domain model. It exposes licensing functionality (Products, Features, Licenses, Accounts) through standardized GraphQL operations, enabling modern web applications, mobile apps, and other clients to query and manipulate licensing data.

## Key Features

- **GraphQL API**: Modern, efficient API with strongly-typed schema
- **CRUD Operations**: Create, Read, Update, Delete for all licensing entities
- **Collection Management**: Bulk operations with filtering and pagination
- **Dependency Resolution**: Navigate feature and license dependencies
- **Permission Queries**: Feature-based access control queries
- **Import/Export**: Product definition file handling
- **Real-time Updates**: Subscription support (if enabled in imtservergql)
- **Metadata Serialization**: JSON conversion for all licensing objects

## Architecture

### Three-Tier Design

**Tier 1: Collection Controllers**
- Handle collections of objects (Products, Features, Licenses, Accounts)
- Support CRUD operations on collections
- Provide filtering, pagination, and search
- Synchronize related collections

**Tier 2: Object Controllers**
- Handle individual object operations
- Simpler interface for single-item CRUD
- Delegate to collection controllers

**Tier 3: Data Providers & Delegates**
- Data Providers: Query handling and data access
- Metadata Delegates: JSON serialization/deserialization
- Dependency Providers: Feature and license dependency resolution

### Component Overview

| Component | Type | Purpose |
|-----------|------|---------|
| `CFeatureCollectionControllerComp` | Collection Controller | Feature collection CRUD |
| `CLicenseCollectionControllerComp` | Collection Controller | License collection CRUD |
| `CProductCollectionControllerComp` | Collection Controller | Product collection CRUD |
| `CAccountControllerComp` | Collection Controller | Account management |
| `CFeatureControllerComp` | Object Controller | Single feature operations |
| `CLicenseControllerComp` | Object Controller | Single license operations |
| `CProductControllerComp` | Object Controller | Single product operations |
| `CProductInfoProviderComp` | Data Provider | Product licensing queries |
| `CPermissionsProviderComp` | Data Provider | Permission queries |
| `CFeatureDependenciesProviderComp` | Data Provider | Dependency navigation |
| `CProductPermissionsControllerComp` | Router | Permission query routing |
| `CFeatureMetaInfoDelegateComp` | Delegate | Feature JSON mapping |
| `CLicenseMetaInfoDelegateComp` | Delegate | License JSON mapping |
| `CProductMetaInfoDelegateComp` | Delegate | Product JSON mapping |
| `CProductInfoFileGeneratorComp` | Generator | C++ header generation |

## GraphQL API

### Query Examples

**List Products:**
```graphql
query {
  products(filter: {categoryId: "software"}, limit: 10, offset: 0) {
    items {
      productId
      name
      description
      category
      features {
        featureId
        name
        optional
      }
    }
    totalCount
  }
}
```

**Get Product Details:**
```graphql
query {
  product(id: "product-office-suite") {
    productId
    name
    description
    features {
      featureId
      name
      optional
      permission
    }
    licenses {
      licenseId
      name
      features {
        featureId
        name
      }
    }
  }
}
```

**Query Feature Dependencies:**
```graphql
query {
  featureDependencies(featureId: "feature-advanced-analytics") {
    directDependencies {
      featureId
      name
    }
    transitiveDependencies {
      featureId
      name
    }
    dependentFeatures {
      featureId
      name
    }
  }
}
```

**Check Permissions:**
```graphql
query {
  permissions(productId: "product-office-suite") {
    featureId
    isPermission
    isEnabled
  }
}
```

### Mutation Examples

**Create Product:**
```graphql
mutation {
  createProduct(input: {
    name: "Enterprise Suite"
    description: "Complete enterprise solution"
    categoryId: "enterprise-software"
    features: [
      {
        featureId: "feature-core"
        name: "Core Functionality"
        optional: false
      },
      {
        featureId: "feature-advanced"
        name: "Advanced Features"
        optional: true
      }
    ]
  }) {
    productId
    name
  }
}
```

**Create License:**
```graphql
mutation {
  createLicense(input: {
    productId: "product-enterprise-suite"
    name: "Professional License"
    description: "Full-featured professional edition"
    features: ["feature-core", "feature-advanced"]
  }) {
    licenseId
    name
  }
}
```

**Update Product:**
```graphql
mutation {
  updateProduct(id: "product-enterprise-suite", input: {
    name: "Enterprise Suite v2"
    features: [
      # Updated feature list
    ]
  }) {
    productId
    name
    updatedAt
  }
}
```

**Delete License:**
```graphql
mutation {
  deleteLicense(id: "license-obsolete") {
    success
    message
  }
}
```

**Import Product:**
```graphql
mutation {
  importProduct(file: $productFile) {
    productId
    name
    featuresCount
  }
}
```

### Subscription Examples

If supported by imtservergql framework:

```graphql
subscription {
  productUpdated(productId: "product-office-suite") {
    productId
    name
    updatedAt
    changeType
  }
}
```

```graphql
subscription {
  licenseActivated(productInstanceId: "instance-customer-123") {
    licenseInstanceId
    expirationDate
    status
  }
}
```

## Data Flow

### Creating a Product
```
1. GraphQL Request (POST /graphql)
   └─> imtservergql: Parse mutation
       └─> imtlicgql: CProductCollectionControllerComp
           ├─> Parse GraphQL input
           ├─> Create IProductInfo via factory
           ├─> Create IFeatureInfo objects
           ├─> Validate product structure
           └─> imtlic: CProductControllerComp
               └─> imtlicdb: Save to database
               <── Product ID
           <── IProductInfo object
       <── GraphQL response
   <── HTTP 200 with product data
```

### Querying Features
```
1. GraphQL Query (POST /graphql)
   └─> imtservergql: Parse query
       └─> imtlicgql: CFeatureCollectionControllerComp
           ├─> Parse filters and pagination
           └─> imtlic: CFeatureInfoProviderComp
               └─> imtlicdb: Query features
               <── IFeatureInfo objects
           ├─> Convert to GraphQL representation
           └─> Apply pagination
           <── Feature list
       <── JSON response
   <── HTTP 200 with features
```

## Usage Examples

### JavaScript/TypeScript Client (Apollo)

```typescript
import { ApolloClient, InMemoryCache, gql } from '@apollo/client';

const client = new ApolloClient({
  uri: 'http://localhost:8080/graphql',
  cache: new InMemoryCache(),
});

// Query products
const GET_PRODUCTS = gql`
  query GetProducts($categoryId: String!) {
    products(filter: { categoryId: $categoryId }) {
      items {
        productId
        name
        description
      }
    }
  }
`;

const { data } = await client.query({
  query: GET_PRODUCTS,
  variables: { categoryId: 'software' },
});

// Create a license
const CREATE_LICENSE = gql`
  mutation CreateLicense($input: LicenseInput!) {
    createLicense(input: $input) {
      licenseId
      name
    }
  }
`;

const result = await client.mutate({
  mutation: CREATE_LICENSE,
  variables: {
    input: {
      productId: 'product-123',
      name: 'Professional License',
      features: ['feature-1', 'feature-2'],
    },
  },
});
```

### React Component

```tsx
import { useQuery, useMutation } from '@apollo/client';
import { GET_PRODUCTS, CREATE_LICENSE } from './queries';

function ProductList() {
  const { loading, error, data } = useQuery(GET_PRODUCTS);
  const [createLicense] = useMutation(CREATE_LICENSE);

  if (loading) return <p>Loading...</p>;
  if (error) return <p>Error: {error.message}</p>;

  return (
    <div>
      {data.products.items.map((product) => (
        <div key={product.productId}>
          <h3>{product.name}</h3>
          <p>{product.description}</p>
          <button
            onClick={() =>
              createLicense({
                variables: {
                  input: {
                    productId: product.productId,
                    name: 'New License',
                    features: [],
                  },
                },
              })
            }
          >
            Add License
          </button>
        </div>
      ))}
    </div>
  );
}
```

### Python Client

```python
import requests

# GraphQL endpoint
url = "http://localhost:8080/graphql"

# Query products
query = """
query {
  products(filter: {categoryId: "software"}) {
    items {
      productId
      name
      description
    }
  }
}
"""

response = requests.post(url, json={'query': query})
data = response.json()

for product in data['data']['products']['items']:
    print(f"Product: {product['name']}")
    print(f"  ID: {product['productId']}")
    print(f"  Description: {product['description']}")
```

## Integration

### With imtlic Module
- Consumes imtlic types (IProductInfo, IFeatureInfo, ILicenseDefinition)
- Uses imtlic factories for object creation
- Implements imtlic provider interfaces
- Delegates to imtlic components for business logic

### With imtservergql Framework
- Extends collection controller base classes
- Uses SDL (Schema Definition Language) for type definitions
- Leverages request/response handling
- Integrates with authentication/authorization

### With Client Applications
- **Web Apps**: JavaScript/TypeScript with Apollo, Relay, or URQL
- **Mobile Apps**: iOS/Android with GraphQL clients
- **Desktop Apps**: Qt applications with HTTP client
- **Development Tools**: GraphQL Playground, GraphiQL

## Best Practices

### API Design
- Use meaningful field names matching domain model
- Provide filtering and pagination for collections
- Document fields with GraphQL descriptions
- Version schema carefully (use deprecation)
- Minimize breaking changes

### Performance
- Implement field-level resolvers
- Use DataLoader for batching
- Cache frequently accessed data
- Optimize database queries
- Consider pagination limits

### Security
- Validate all input data
- Check permissions before operations
- Sanitize user input
- Use authentication tokens
- Implement rate limiting
- Audit sensitive operations

### Error Handling
- Return meaningful error messages
- Include error codes for client handling
- Log errors for debugging
- Don't expose internal details
- Handle validation errors gracefully

## Documentation

- **imtlicgql.h**: Comprehensive namespace documentation
- **LICENSING_ARCHITECTURE.md**: Complete architecture (root directory)
- **GraphQL Schema**: SDL files defining API structure
- **imtlic module**: Core domain model documentation

## Building

### CMake
```bash
cd Include/imtlicgql/CMake
cmake .
make
```

### QMake
```bash
cd Include/imtlicgql/QMake
qmake
make
```

## Related Modules

- **imtlic**: Core licensing domain model
- **imtlicdb**: Database persistence layer
- **imtservergql**: GraphQL server framework
- **imtgql**: GraphQL request handling

## License

See LICENSE file in repository root. ImtCore uses multi-licensing:
- LGPL-2.1-or-later
- GPL-2.0-or-later
- GPL-3.0-or-later
- LicenseRef-ImtCore-Commercial
