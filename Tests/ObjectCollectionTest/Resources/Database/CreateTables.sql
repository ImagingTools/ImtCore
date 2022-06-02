
/**
    Logical package/container of features.
*/
CREATE TABLE Packages(
    Id VARCHAR (1000) NOT NULL,
	Name VARCHAR (1000) NOT NULL,
	Description VARCHAR (1000),
	Added TIMESTAMP,
	LastModified TIMESTAMP,
	PRIMARY KEY (Id)
);


/**
    Features assigned to defined packages.
*/
CREATE TABLE Features(
    Id VARCHAR (1000) NOT NULL,
	Name VARCHAR (1000) NOT NULL,
	Description VARCHAR (1000),
	PackageId VARCHAR (1000) NOT NULL,
	PRIMARY KEY (Id, PackageId),
	FOREIGN KEY (PackageId) REFERENCES Packages(Id) ON DELETE CASCADE ON UPDATE CASCADE
);


/**
    Dependencies between features. A Feature B can technically depend on Feature A (By example Feature <Biometric Access> is based on Feature <User Management>.
	In this case including the feature B into a product license means that the Feature A is should be also included.
*/
CREATE TABLE FeatureDependencies(
  FeatureId VARCHAR (1000) NOT NULL,
  FeaturePackageId VARCHAR (1000) NOT NULL,
  DependencyId VARCHAR (1000) NOT NULL,
  DependencyPackageId VARCHAR (1000) NOT NULL,
  PRIMARY KEY (FeatureId, FeaturePackageId, DependencyId, DependencyPackageId),
  FOREIGN KEY (FeatureId, FeaturePackageId) REFERENCES Features(Id, PackageId) ON DELETE CASCADE ON UPDATE CASCADE,
  FOREIGN KEY (DependencyId, DependencyPackageId) REFERENCES Features(Id, PackageId) ON DELETE CASCADE ON UPDATE CASCADE
);


/**
    List of products.
*/
CREATE TABLE Products(
  Id VARCHAR (1000) NOT NULL,
  Name VARCHAR (1000) NOT NULL,
  Description VARCHAR (1000),
  Added TIMESTAMP,
  LastModified TIMESTAMP,
  PRIMARY KEY (Id)
);


/**
    List of available product licenses
*/
CREATE TABLE ProductLicenses(
  Id VARCHAR (1000) NOT NULL,
  Name VARCHAR (1000) NOT NULL,
  Description VARCHAR (1000),
  ProductId VARCHAR (1000) NOT NULL,
  PRIMARY KEY (Id, ProductId),
  FOREIGN KEY (ProductId) REFERENCES Products(Id) ON DELETE CASCADE ON UPDATE CASCADE
);


/**
    A product license is defined as a set of features
*/
CREATE TABLE ProductLicenseFeatures(
  ProductId VARCHAR (1000) NOT NULL,
  LicenseId VARCHAR (1000) NOT NULL,
  PackageId VARCHAR (1000) NOT NULL,
  FeatureId VARCHAR (1000) NOT NULL,
  PRIMARY KEY (ProductId, LicenseId, PackageId, FeatureId),
  FOREIGN KEY (LicenseId, ProductId) REFERENCES ProductLicenses(Id, ProductId) ON DELETE CASCADE ON UPDATE CASCADE,
  FOREIGN KEY (FeatureId, PackageId) REFERENCES Features(Id, PackageId) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TYPE AccountType AS ENUM ('private', 'company');

CREATE TABLE Accounts(
  Id VARCHAR (1000) NOT NULL,
  Name VARCHAR (1000) NOT NULL,
  Description VARCHAR (1000),
  Type AccountType NOT NULL,
  OwnerMail VARCHAR (1000) NOT NULL,
  OwnerFirstName VARCHAR (1000) NOT NULL,
  OwnerLastName VARCHAR (1000) NOT NULL,
  PRIMARY KEY (Id)
);


CREATE TABLE ProductInstances(
    InstanceId VARCHAR (1000) NOT NULL,
	ProductId VARCHAR (1000) NOT NULL,
	AccountId VARCHAR (1000) NOT NULL,
	Name VARCHAR (1000) NOT NULL,
	Description VARCHAR (1000),
	Added TIMESTAMP,
	LastModified TIMESTAMP,
	PRIMARY KEY (InstanceId),
	FOREIGN KEY (AccountId) REFERENCES Accounts(Id) ON DELETE CASCADE ON UPDATE CASCADE,
	FOREIGN KEY (ProductId) REFERENCES Products(Id) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE ProductInstanceLicenses(
    InstanceId VARCHAR (1000) NOT NULL,
	LicenseId VARCHAR (1000) NOT NULL,
	ProductId VARCHAR (1000) NOT NULL,
	ExpirationDate DATE,
	PRIMARY KEY (InstanceId, LicenseId, ProductId),
	FOREIGN KEY (LicenseId, ProductId) REFERENCES ProductLicenses(Id, ProductId) ON DELETE CASCADE ON UPDATE CASCADE,
	FOREIGN KEY (InstanceId) REFERENCES ProductInstances(InstanceId) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE Tests(
    Id VARCHAR (1000) NOT NULL,
	Name VARCHAR (1000),
	Description VARCHAR (1000),
	PRIMARY KEY (Id)
);


INSERT INTO Packages(Id, Name, Description, Added) VALUES('StandardFramework', 'Standard Framework', 'Common features for all products', NOW());
INSERT INTO Packages(Id, Name, Description, Added) VALUES('TCVisionFramework', 'TCVision Framework', 'Common features for all products of the TCVision family', NOW());
INSERT INTO Packages(Id, Name, Description, Added)  VALUES('RTVisionFramework', 'RTVision Framework', 'Common features for all products of the RTVision family', NOW());
INSERT INTO Packages(Id, Name, Description, Added)  VALUES('RTVision3dFramework', 'RTVision.3d Framework', 'Common features of the RTV-3D-products', NOW());

INSERT INTO Features(Id, Name, PackageId) VALUES('#UserManagement', 'User Management', 'StandardFramework');
INSERT INTO Features(Id, Name, PackageId) VALUES('#DataExport', 'Data Export', 'StandardFramework');
INSERT INTO Features(Id, Name, PackageId) VALUES('#BiometricAccess', 'Biometric Authentification', 'StandardFramework');
INSERT INTO Features(Id, Name, PackageId) VALUES('#Report', 'Report', 'StandardFramework');
INSERT INTO Features(Id, Name, PackageId) VALUES('#ModelSearch', 'Model Search', 'StandardFramework');
INSERT INTO Features(Id, Name, PackageId) VALUES('#Automatic', 'Automatic', 'StandardFramework');

INSERT INTO Features(Id, Name, PackageId) VALUES('#ResultOverview', 'Result Overview', 'RTVisionFramework');
INSERT INTO Features(Id, Name, PackageId) VALUES('#PatchInspection', 'Patch Inspection', 'RTVisionFramework');

INSERT INTO Features(Id, Name, PackageId) VALUES('#PrimerInspection', 'Primer Inspection', 'RTVision3dFramework');
INSERT INTO Features(Id, Name, PackageId) VALUES('#VolumeInspection', 'Volume Inspection', 'RTVision3dFramework');
INSERT INTO Features(Id, Name, PackageId) VALUES('#Teaching', 'Teaching', 'RTVision3dFramework');
INSERT INTO Features(Id, Name, PackageId) VALUES('#PositionCorrection', 'Position Correction', 'RTVision3dFramework');

INSERT INTO Products(Id, Name, Description) VALUES('TCVision.l', 'TCVision.l', 'Shell inspection (Liner)');
INSERT INTO Products(Id, Name, Description) VALUES('TCVision.e', 'TCVision.e', 'End inspection');

INSERT INTO Products(Id, Name, Description) VALUES('RTVision', 'RTVision', 'Glue width inspection based in 2D-space');
INSERT INTO ProductLicenses(Id, Name, Description, ProductId) VALUES('12.1234', 'Standard', 'Standard license for RTVision product', 'RTVision');

INSERT INTO Products(Id, Name, Description) VALUES('RTVision.3d', 'RTVision.3d', 'Complete glue evaluation in 3D-space');
INSERT INTO ProductLicenses(Id, Name, Description, ProductId) VALUES('12.10128', 'Standard', 'Standard license for RTVision.3d product', 'RTVision.3d');
INSERT INTO ProductLicenses(Id, Name, Description, ProductId) VALUES('12.10135', 'Advanced', 'Advanced license for RTVision.3d product', 'RTVision.3d');
INSERT INTO ProductLicenses(Id, Name, Description, ProductId) VALUES('12.10140', 'Position Correction', 'Standard + position correction license for RTVision.3d product', 'RTVision.3d');

