# Regeln zur Organisation von Berechtigungen in ImtCore- und Lisa-basierten Lösungen

> **Sprache:** Deutsch · [Русский](Permissions-Rules.ru.md) · [English](Permissions-Rules.en.md)
>
> **Zweck.** Einheitliche Regeln zum Beschreiben, Zuordnen, Prüfen und Testen von Berechtigungen in Produkten auf Basis von ImtCore und Lisa.
> **Quellen.** Code aus `ItDevelopment`: ImtCore `442be502b9` (2026-10-15), Lisa `a598f56` (PR #28 „Full-permissions“), ProLife `d4d9a910`, Puma `e8b53c6`; Partitura-Dateien (`.acc`), Produktdateien `*Features.h`, QML und Tests. Die Pfade unten sind relativ zu `ItDevelopment/`.
> **Implementierungsstand.** Branches `feature/permission-requirements` (ImtCore), `feature/organization-permissions-by-path` (Puma), `feature/permission-model-tests` (ProLife) und `feature/permission-paths` (Lisa), Stand 2026-10-19: ausschließlich Pfadberechtigungen, neue Lisa-/ProLife- und Organisationsbäume sowie Revisionen pro Entität.
> **Lesehinweis.** Abschnitte 3–12 beschreiben die Regeln und das **Zielverhalten**. Wo der Code noch abweicht, verweist Abschnitt 13 auf die Abweichung. Abschnitt 13 enthält außerdem offene Fragen.

---

## 1. Zuständigkeiten der Komponenten

| Komponente | Zuständigkeit für Berechtigungen |
|---|---|
| **Lisa** | Katalog für Produkte, Features und Lizenzen. Hier wird der **Feature-Baum** eines Produkts bearbeitet (`FeatureEditor`) und `<Product>Features.h` generiert (`ImtCore/Include/imtlicgql/CProductInfoFileGeneratorComp`). Lisa ist selbst ein Produkt (`Lisa`) mit eigenem Baum. |
| **ImtCore** | Laufzeit: Modell (`imtlic`, `imtauth`), Prüfung (`imtservergql`, `imtserverapp`, `imtauthgql`), GUI (`imtauthgui`, `imtlicgui`) und fertige Partitura-Komponenten (`ImtCoreVoce.arp`, `ImtAuthVoce.arp`, `ImtUserAdministrationVoce.arp`). |
| **Puma** | Dienst für Benutzerkonten (Benutzer, Rollen, Gruppen, Organisationen). Produktserver erhalten Benutzer und Berechtigungen darüber (`AdminId = su`). |
| **Produkt** (Lisa, ProLife, NeoPro, Acula, Agentino, IotPlatform) | Beschreibt den eigenen Feature-Baum und deklariert Berechtigungen für Serverbefehle und GUI-Seiten in `.acc`-Dateien. |

## 2. Ablauf von Anfang bis Ende

```
Lisa FeatureEditor ──generate──► <Product>Features.h ──FillProduct()──► IProductInfo (Produkt-Feature-Baum)
                                                                                  │
Rollen / Gruppen / lokale Berechtigungen (nur Blätter) ──► Puma                 │
         Produktberechtigungen, nach Organisation gefiltert                      ▼
                                                CAuthenticationManagerComp: erstellt IGqlContext
                                                (user, tenant, product, PAT-scopes, IsTenantOwner)
                                                                                  │
                                                wirksame Berechtigungen = (Berechtigungen ∩ PAT-scopes)
                                                + eingeschlossene Requirements, ∩ Organisationsberechtigungen
                                                                                  │
                ┌─────────────────────────────────────────────────────────────────┤
                ▼                                                                 ▼
      Serverbefehle (GraphQL)                                      GUI: Seiten und Befehle
      CPermissibleGqlRequestHandlerComp                            CCommandsController / PagesController
      ICommandPermissionsProvider + IPermissionChecker             liefern nur zugängliche Elemente
                │                                                                 │
                └──────────────── imtauth::HasPermission ◄───────────────────────┘
                                  (einheitliche Vergleichsregel)
```

**Grundsatz:** Die Sicherheitsgrenze liegt auf dem Server. Die GUI (Seiten und Schaltflächen ausblenden) dient der Bedienbarkeit, nicht dem Schutz.

## 3. Berechtigungen beschreiben (Feature-Baum)

**R3.1. Einzige Quelle ist der Produkt-Feature-Baum in Lisa.** `<Product>Features.h` wird generiert und darf nicht manuell bearbeitet werden. Beim Start ruft der Server `FillProduct(*IProductInfo)` auf (`Lisa/Impl/LisaServer/Main.cpp`); für GraphQL-Anfragen liest `ProductPermissions` den Baum aus der Ressource `:/Features/<Product>Features` (Komponente `SingleProductPermission`). Lisa importiert `<Product>Features.h` ebenso wie XML: Die Datei legt ein neues Produkt an; ein Produkt mit derselben `ProductId` wird nicht importiert, ebenso wenig eine Wurzel, die bereits unter einer anderen UUID im Feature-Katalog vorhanden ist. Features aus der Datei werden in den Katalog geschrieben: neue werden angelegt, vorhandene durch die Dateiversion ersetzt (der Importdialog weist darauf hin).

**R3.2. Berechtigungsknoten:** `SetIsPermission(true)`, `SetOptional(false)`. Eine Berechtigung erlaubt eine Aktion in der Anwendung und ist kein verkaufbares Feature. Vermarktbare Features (`IsPermission=false`, `Optional=true`) gehören nicht zum Berechtigungsbaum. In allen aktuellen Produkten sind alle Knoten Berechtigungen.

**R3.3. Knotenkennung (`FeatureId`):** lateinische Zeichen, PascalCase, kein `/`, eindeutig unter den Geschwistern. Die Entität benennt den Bereich; daher sind Kennungen darin kurz (`View`, `Edit`, `Add`) und werden in anderen Bereichen wiederverwendet. Eine Berechtigung ist ausschließlich der **vollständige Pfad** (Abschnitt 4). Pfade werden in der Datenbank für Rollen und Benutzer gespeichert; beim Umbenennen eines Knotens ist eine Migration erforderlich. Anzeigenamen verwenden `QT_TRANSLATE_NOOP("Feature", "...")`.

**R3.4. Typische Bereichsstruktur** (Lisa und ProLife, nach dem Organisationsbaum aus R3.6):

```
/<Area>Management                     Bereichsgruppe (FeatureManagement, SensorManagement, ...)
├─ ViewPage                           Blatt: Bereichsseite (Menü, Seite)
├─ View                               Blatt: Daten (Sammlung, Dokumente, Befehle)
├─ ViewAll, ViewHistory → View        Blätter, falls vorhanden (ProLife)
├─ Edit → View                        Gruppe: beliebige Änderung
│  ├─ Add                             Blatt: Erstellen
│  ├─ Change                          Blatt oder Feldgruppe: Change/{Description, SerialNumber, ...}
│  └─ Remove                          Blatt: Löschen
└─ Revisions                          Revisionen der Dokumente dieses Bereichs
   ├─ View → <Area>/View
   ├─ Restore → Revisions/View
   └─ Delete → Revisions/View
```

**`ViewPage` und `View` sind unabhängig** und haben keine Requirements zueinander: Bereichsdaten können ohne die zugehörige Seite angezeigt werden (Sensortabelle in einem anderen Editor, Auswahlliste, API); eine Seite ohne `View` bleibt leer. Für die Seite ist `ViewPage`, für die Daten `View` erforderlich. Eine Rolle muss beide Berechtigungen vergeben, wenn beide benötigt werden.

Besondere Aktionen eines Bereichs sind Blätter neben `View` (`/SensorManagement/Bind`, `/SensorManagement/CreateLicenseFile`) oder unter `Edit` (`/LicenseManagement/Edit/Split`, `/LicenseManagement/Edit/Revoke`). Administration: `/Administration/{Users,Roles,Groups}/{ViewPage, View, ViewHistory → View, Edit → View/{Add, Change, Remove}, Revisions}`. Arbeitsbereich: `/WorkspaceManagement/{ViewPage, ViewUserActions, ViewAllUserActions → ViewUserActions}` (ProLife hat zusätzlich `ViewAnalytics`). Suche: `/SearchManagement/ViewPage`—diese Bereiche haben keine eigene Sammlung. Eine bereichsübergreifende Revisionsgruppe gibt es nicht mehr (R6.7).

**R3.5. Benutzer und Rollen speichern ausschließlich Blätter** (als vollständige Pfade). Eine Gruppe dient nur zur Gruppierung von Blättern und kann nicht vergeben werden. Requirements einer Gruppe gelten für deren Blätter, nicht für die Gruppe selbst (Abschnitt 5). Gruppenpfade werden nur in **Prüfungen** verwendet (R4.4).

**R3.6. Der Organisationsbaum ist getrennt.** Berechtigungen innerhalb einer Organisation werden von `imtauth::COrganizationsPermissionsProviderComp` und nicht vom Produkt festgelegt. Produkt- und Organisationsbaum dürfen nicht vermischt werden. Es gibt eine Wurzel und Bereiche nach dem Muster aus R3.4:

```
/OrganizationManagement
├─ Edit/{ChangeName, ChangeDescription}
├─ Members/{View, Edit → Members/View /{Invite, Exclude, ChangeRole}}
├─ Roles/{View, Edit → Roles/View /{Add, Change, Remove}}
├─ Groups/{View, Edit → Groups/View /{Add, Change, Remove}}
├─ Permissions/{View, Change → Permissions/View}
└─ Connections/{View, ViewCode, Edit → Connections/View /{Connect, Remove}}
```

Berechtigungen dieses Baums werden wie Produktberechtigungen nur als vollständige Pfade gespeichert und geprüft (Abschnitt 4).

**Zwei `/OrganizationManagement`-Bäume mit unterschiedlichen Aufgaben.**
- **Organisationsbaum** (oben): statische Berechtigungen eines Mitglieds, also was ein Benutzer **innerhalb** der Organisation tun darf. Sie werden in der Mitgliedschaft gespeichert (`ITenantMembership::GetOrganizationPermissions`), vom Server direkt in den Befehlen geprüft (`imtauthgql::HasTenantManageAccess`, `CanManageMembers`) und dem Client als `currentUserOrganizationPermissions` übergeben. Eigentümer, Ersteller und Superuser benötigen keine Blattprüfung.
- **Produktbaum** (ProLife) regelt ausschließlich die Sichtbarkeit von Seite, Daten und Befehlen:

```
/OrganizationManagement
├─ ViewPage        Seite „Organisationen“
├─ View            Organisationsdaten und alle Organisationsbefehle (Lesen, Ändern, Mitglieder, Verbindungen, ...)
├─ Add             Organisation erstellen
├─ Remove          Organisation löschen
├─ Revisions/{View → View, Restore, Delete}
└─ Shares/Edit → View   Freigaben von Dokumenten der Organisation verwalten
```

Interne Organisationsbefehle werden mit der Produktberechtigung `/OrganizationManagement/View` deklariert; welches konkrete Handeln erlaubt ist, bestimmt das Mitgliedsblatt:

| Befehle | Berechtigungsblatt des Mitglieds |
|---|---|
| Organisation ändern (`UpdateTenant`) | `Edit/ChangeName` oder `Edit/ChangeDescription` |
| `SetTenantActive`, `RemoveTenant` | Nur Eigentümer, Ersteller und Superuser |
| Mitglieder und Einladungen lesen | `Members/View` (eigene Mitgliedschaft erfordert keine Berechtigung) |
| Mitglied einladen, ausschließen oder Rolle ändern | Jedes Blatt `Members/Edit/*` (13.23) |
| `GetTenantPermissions` | `Permissions/View` oder `Roles/View` |
| Berechtigungen eines anderen Mitglieds lesen (`GetOrganizationPermissions`) | `Permissions/View` |
| Berechtigungen eines Mitglieds ändern (`UpdateMembershipPermissions`) | Jedes Blatt `Members/Edit/*` (13.23) |
| Verbindungen, Anfragen, Angebote, Verträge, Grants, Nachrichten und Anträge lesen | `Connections/View` |
| Verbindungscode | `Connections/ViewCode` |
| Erstellen, Annehmen, Ablehnen oder Abbrechen; Verträge, Grants, Nachrichten oder Anträge ändern; Verbindungscode ändern | `Connections/Edit/Connect` |
| Verbindung entfernen | `Connections/Edit/Remove` |
| Freigaben: auf Gruppen beschränken, Status, Entfernen (`SetDocumentShareGroups`, `SetDocumentSharesStatus`, `RemoveOrganizationDocumentShares`) | Produktberechtigung `/OrganizationManagement/Shares/Edit` (ProLife, Attribut `EditCommandPermissions` der Komposition `DocumentShareController`); Lesen (`GetOrganizationDocumentShares`) ist `View` |

## 4. Kennungsformat und Vergleichsregel

**Einziges Format ist der vollständige Knotenpfad:** `/Administration/Users/Edit/Change` (`imtlic::CalculateFeaturePath`), immer mit führendem `/`. Eine bloße `FeatureId` (`ChangeUser`) ist **keine** Berechtigung: Sie gewährt einem Benutzer nichts, stimmt mit keiner Prüfung überein und entfaltet keine Requirements. Gespeicherte bloße Kennungen müssen in Pfade migriert werden (13.5).

**R4.1. Einheitliche Regel: `imtauth::HasPermission(Benutzerberechtigungen, angeforderte Berechtigung)`** (`ImtCore/Include/imtauth/imtauth.h`); auf dem Client `PermissionsController.hasPermission` (QML, siehe 13.3). Jede Berechtigungsprüfung muss diese Funktion verwenden; `contains`/`indexOf` auf Berechtigungslisten sind unzulässig.

**Baum für die Beispiele**—ein fiktiver Baum, entsprechend den Unit-Tests `PermissionRequirementsTest` (Produktbäume folgen R3.4; lange Kennungen erleichtern das Verständnis):

```
/Administration
├─ ViewAdministration          Gruppe
│  ├─ ViewUsers                Blatt
│  └─ ViewRoles                Blatt
└─ EditAdministration          Gruppe
   ├─ EditUser                 Gruppe
   │  ├─ ChangeUser            Blatt
   │  └─ RemoveUser            Blatt
   └─ EditRole                 Gruppe
      └─ ChangeRole            Blatt
/RevisionManagement            Gruppe
├─ ViewRevisions               Blatt
└─ ExportRevision              Blatt
/Reports/ViewUsers             Blatt (gleiche Kennung unter einem anderen Elternknoten)
```

**Vergleichsregel**

| Benutzer besitzt (Blatt) | Prüfung | Ergebnis |
|---|---|---|
| `/Administration/EditAdministration/EditUser/ChangeUser` | derselbe vollständige Pfad | **ja**—dasselbe Blatt |
| `/Administration/EditAdministration/EditUser/ChangeUser` | `/Administration/EditAdministration/EditUser` | **ja**—Gruppenprüfung bedeutet „mindestens ein Blatt darin“ |
| `/Administration/EditAdministration/EditUser/ChangeUser` | `/Administration` | **ja**—dieselbe Regel auf jeder Ebene |
| `/Administration/EditAdministration/EditUser/ChangeUser` | `/Administration/EditAdministration/EditUser/RemoveUser` | nein—Geschwisterblatt |
| `/Administration/ViewAdministration/ViewUsers` | `/Administration/EditAdministration` | nein—der Benutzer besitzt keine Blätter in dieser Gruppe |
| `/Administration/ViewAdministration/ViewUsers` | `/Reports/ViewUsers` | nein—gleiche Kennung unter anderem Elternknoten ist eine andere Berechtigung |
| `ChangeUser` (bloße Kennung, alte Daten) | `/Administration/EditAdministration/EditUser/ChangeUser` | nein—eine bloße Kennung ist keine Berechtigung |
| `/Administration/EditAdministration/EditUser/ChangeUser` | `ChangeUser` (bloße Kennung) | nein—Prüfung auf eine bloße Kennung schlägt immer fehl |
| `/Administration/EditAdministration/EditUser` (als Berechtigung gespeicherte Gruppe) | `/Administration/EditAdministration/EditUser/ChangeUser` | nein—eine Gruppe kann nicht vergeben werden (R3.5) und deckt kein Blatt ab |
| beliebig | leerer String | nein |

**R4.2.** Pfadgrenze ist `/`: `/Administration/EditAdministration/EditUser` deckt `.../EditUser/ChangeUser` ab, aber nicht `.../EditUsers` (anderer Knoten mit ähnlichem Namen).
**R4.3.** Überall sind vollständige Pfade erforderlich: `.acc`, `Features.h`, C++, QML, Requirements, PAT-scopes sowie Rollen- und Benutzerberechtigungen. Eine gemeinsame ImtCore-Komponente, der der Produktpfad nicht im Voraus bekannt ist, erhält ihn vom Host über eine Eigenschaft (`revisionsPermissionPath`, `permissionPath`), statt eine bloße Kennung zu prüfen. Eine Deklaration mit bloßer Kennung öffnet einen Befehl niemandem außer Superusern und Organisationsinhabern (13.21).
**R4.4.** Ein Gruppenpfad ist in einer **Prüfung** zulässig (z. B. enthält `PagePermissions` `/FeatureManagement/EditFeature`) und bedeutet: „Der Benutzer besitzt mindestens ein Blatt darin.“ Eine Gruppe kann nicht vergeben werden (R3.5).

## 5. Requirements—welche Berechtigungen eingeschlossen werden

**Bedeutung.** `requirements` eines Knotens sind die Berechtigungen, die er **einschließt**. `ChangeUser` → `ViewUsers` bedeutet: Wer `ChangeUser` besitzt, besitzt auch `ViewUsers`. Umgekehrt gilt es nicht: `ViewUsers` gewährt kein `ChangeUser`. Eingeschlossene Berechtigungen werden nicht in einer Rolle gespeichert, sondern bei der Prüfung berechnet.

**R5.1. Vier Abhängigkeitstypen.** Quelle und Ziel können jeweils Blatt oder Gruppe sein. Benutzerberechtigungen enthalten immer nur Blätter (R3.5).

| Abhängigkeit | Beispiel | Bedeutung |
|---|---|---|
| Blatt → Blatt | `ChangeUser` → `ViewUsers` | Wer `ChangeUser` besitzt, besitzt auch `ViewUsers` |
| Gruppe → Blatt | Gruppe `EditUser` → `ViewUsers` | Jedes Blatt in `EditUser` (`ChangeUser`, `RemoveUser`, …) schließt `ViewUsers` ein |
| Blatt → Gruppe | `ChangeUser` → Gruppe `ViewAdministration` | `ChangeUser` schließt **alle** Blätter in `ViewAdministration` ein |
| Gruppe → Gruppe | Gruppe `EditAdministration` → Gruppe `ViewAdministration` | Jedes Blatt in `EditAdministration` schließt alle Blätter in `ViewAdministration` ein |

**R5.2. Berechnung**—bei jeder Berechtigungsprüfung anhand des Produktbaums (`IProductInfo`) aus dem Anfragekontext:
1. Ausgangsmenge sind die Produktblätter des Benutzers nach Organisationsfilterung; bei einem PAT werden sie mit den Token-scopes geschnitten (Abschnitt 10).
2. Jedes Blatt schließt seine eigenen Requirements und die Requirements aller Gruppen ein, in denen es liegt.
3. Ein Blattziel fügt dieses Blatt hinzu; ein Gruppenziel alle Blätter der Gruppe zum Prüfzeitpunkt.
4. Die resultierenden Blätter werden auf dieselbe Weise expandiert, solange die Menge wächst: transitiv und einschließlich der Requirements ihrer Gruppen. Zyklen sind zulässig.
5. Das Ergebnis wird auf die Organisationsberechtigungen begrenzt (`GetTenantPermissions()`).
6. Das sind die **wirksamen Berechtigungen**; geprüft wird mit `HasPermission(wirksame Berechtigungen, angeforderte Berechtigung)` (Abschnitt 4).

Ohne `IProductInfo` im Kontext werden Requirements nicht expandiert, und der Benutzer hat nur die vergebenen Blätter. Das Vorhandensein von `FeatureContainer` in `AuthenticationManager` wird beim Serverstart geprüft.

**R5.3. Abhängigkeiten in Richtung „stärkere Berechtigung → was sie einschließt“ eintragen:** `Edit*` → `View*`, niemals umgekehrt. Meist genügt ein Eintrag für die Gruppe: `EditUser` → `ViewUsers` gilt für `ChangeUser`, `RemoveUser` und `AddUser`. Lisa, ProLife und der Organisationsbaum sind so konfiguriert; NeoPro, IotPlatform und Acula verwenden noch die umgekehrte Richtung (13.1).

**R5.4. Einschränkungen** (werden von `FeatureEditor` und dem `Features.h`-Generator geprüft):
- Das Ziel muss ein existierender Knoten desselben Produkts sein und als vollständiger Pfad angegeben werden; `LicenseEditor` kennzeichnet externe Requirements mit „from outside this product“.
- Ein Knoten darf nicht von **sich selbst, seinen Vorfahren oder seinen Nachfahren** abhängen.

**R5.5. Gruppenziele werden zum Prüfzeitpunkt expandiert.** Ein neu zu einer Zielgruppe hinzugefügtes Blatt erhalten sofort alle Benutzer mit einer Abhängigkeit zu dieser Gruppe, ohne dass ihre Rollen geändert werden. Daher zeigt `FeatureEditor` für jeden Knoten die resultierenden Einschließungen (z. B. „`ChangeUser` schließt ein: `ViewUsers`, `ViewRoles`“) und warnt beim Hinzufügen eines Blatts zu einer Gruppe, die als Abhängigkeitsziel verwendet wird.

**R5.6. Der Server gibt dem Client wirksame Berechtigungen zurück** (vergebene + eingeschlossene); andernfalls wären Schaltflächen, die durch eingeschlossene Berechtigungen freigeschaltet werden, verborgen. Der Rollen-Editor zeigt eingeschlossene Berechtigungen separat und deaktiviert an: `ViewUsers` kann nicht entfernt werden, solange die Rolle `ChangeUser` besitzt.

### Beispiele

Der Baum stammt aus Abschnitt 4. Die Tabellen verwenden Kurzformen der vollständigen Pfade: `ChangeUser` = `/Administration/EditAdministration/EditUser/ChangeUser`, Gruppe `EditUser` = `/Administration/EditAdministration/EditUser` usw. `→` bedeutet „schließt ein“.

**Blatt → Blatt**

| Benutzer besitzt | Requirements | Prüfung | Ergebnis |
|---|---|---|---|
| `ChangeUser` | `ChangeUser` → `ViewUsers` | `ChangeUser` | **ja**—Berechtigung vergeben |
| `ChangeUser` | `ChangeUser` → `ViewUsers` | `ViewUsers` | **ja**—durch `ChangeUser` eingeschlossen |
| `ChangeUser` | `ChangeUser` → `ViewUsers` | `ViewRoles` | nein—nur `ViewUsers` ist eingeschlossen |
| `ViewUsers` | `ChangeUser` → `ViewUsers` | `ChangeUser` | nein—View gewährt kein Edit |
| `ChangeUser` | `ChangeUser` → `ViewUsers` | Gruppe `ViewAdministration` | **ja**—das eingeschlossene `ViewUsers` liegt in der Gruppe |
| `ChangeUser` | `ChangeUser` → `ViewUsers`, `ViewUsers` → `ViewRevisions` | `ViewRevisions` | **ja**—Einschließung ist transitiv |

**Gruppe → Blatt**

| Benutzer besitzt | Requirements | Prüfung | Ergebnis |
|---|---|---|---|
| `RemoveUser` | Gruppe `EditUser` → `ViewUsers` | `ViewUsers` | **ja**—Gruppenabhängigkeit gilt für deren Blatt |
| `ChangeRole` | Gruppe `EditUser` → `ViewUsers` | `ViewUsers` | nein—`ChangeRole` gehört nicht zu `EditUser` |

**Blatt → Gruppe**

| Benutzer besitzt | Requirements | Prüfung | Ergebnis |
|---|---|---|---|
| `ChangeUser` | `ChangeUser` → Gruppe `ViewAdministration` | `ViewRoles` | **ja**—alle Blätter der Gruppe sind eingeschlossen |
| `ChangeUser` | `ChangeUser` → Gruppe `ViewAdministration` | `/Reports/ViewUsers` | nein—gleichnamiges Blatt außerhalb der Gruppe ist nicht eingeschlossen |
| `ViewRoles` | `ChangeUser` → Gruppe `ViewAdministration` | `ChangeUser` | nein—die Abhängigkeit wirkt nicht rückwärts |

**Gruppe → Gruppe**

| Benutzer besitzt | Requirements | Prüfung | Ergebnis |
|---|---|---|---|
| `ChangeRole` | Gruppe `EditAdministration` → Gruppe `ViewAdministration` | `ViewUsers` | **ja**—jedes Blatt des Edit-Zweigs schließt alle Blätter des View-Zweigs ein |
| `ViewUsers` | Gruppe `EditAdministration` → Gruppe `ViewAdministration` | `ChangeRole` | nein—die Abhängigkeit wirkt nicht rückwärts |
| `ChangeUser` | Gruppe `EditAdministration` → Gruppe `ViewAdministration`; Gruppe `ViewAdministration` → `ViewRevisions` | `ViewRevisions` | **ja**—eingeschlossene Blätter bringen Requirements ihrer Gruppe mit |
| `ChangeUser` | wie oben | `ExportRevision` | nein—nur `ViewRevisions` ist eingeschlossen |

**Grenzen und Sonderfälle**

| Benutzer besitzt | Requirements | Prüfung | Ergebnis |
|---|---|---|---|
| `ChangeUser`; PAT-scopes `[ChangeUser]` | `ChangeUser` → `ViewUsers` | `ViewUsers` | **ja**—Requirements werden aus den Token-scopes expandiert |
| `ChangeUser`, `RemoveUser`; PAT-scopes `[ChangeUser]` | `ChangeUser` → `ViewUsers` | `RemoveUser` | nein—außerhalb der Token-scopes |
| `ChangeUser`; Organisation erlaubt `ChangeUser`, nicht aber `ViewUsers` | `ChangeUser` → `ViewUsers` | `ViewUsers` | nein—Organisationsgrenze. Der Organisationsberechtigungs-Editor speichert diese Menge nicht: Er ergänzt eingeschlossene Blätter selbst (Abschnitt 10) |
| `ChangeUser`; neues Blatt `ViewGroups` wird zu `ViewAdministration` hinzugefügt | `ChangeUser` → Gruppe `ViewAdministration` | `ViewGroups` | **ja**—sofort nach Aktualisierung des Baums, ohne Rollenänderung (R5.5) |
| `ChangeUser` (bloße Kennung, alte Daten) | `ChangeUser` → `ViewUsers` | `ViewUsers` | nein—bloße Kennung ist keine Berechtigung und schließt nichts ein; alte Daten werden separat migriert (13.5) |
| `ChangeUser`; kein `IProductInfo` im Kontext | `ChangeUser` → `ViewUsers` | `ViewUsers` | nein—Requirements werden nicht expandiert; dies ist ein Konfigurationsfehler (R5.2) |

**Unzulässige Abhängigkeiten** (vom Generator und von `FeatureEditor` zurückgewiesen, R5.4)

| Abhängigkeit | Grund |
|---|---|
| `ChangeUser` → `ChangeUser` | Abhängigkeit von sich selbst |
| `ChangeUser` → Gruppe `EditUser` | Abhängigkeit vom Elternknoten: `ChangeUser` würde `RemoveUser` und `AddUser` erhalten |
| `ChangeUser` → Gruppe `/Administration` | Abhängigkeit von einem Vorfahren: der gesamte Bereich einschließlich `ChangeRole` würde vergeben |
| Gruppe `EditUser` → `ChangeUser` | Abhängigkeit von einem Nachfahren |
| Gruppe `EditAdministration` → Gruppe `EditUser` | Abhängigkeit von einem Nachfahren: `ChangeRole` würde `RemoveUser` erhalten |

## 6. Berechtigungen Serverbefehlen zuordnen

### 6.1. Komponenten (Partitura, Paket `ImtServerAppPck`)

| Komponente | Zweck |
|---|---|
| `SingleCommandPermissionsProvider` | Ein `CommandId` und eine Liste von `CommandPermissions`. |
| `CommandPermissionsProviderJoiner` | Führt Anbieter zu einem (`CommandPermissionsProvider[]`) zusammen; bei doppeltem `CommandId` gewinnt der **erste** Anbieter (13.9). |
| `ObjectCollectionPermissions` (`ImtAuthVoce`) | Vorgegebene Zuordnung von Standardbefehlen einer Sammlung (`GetObjectData`, `InsertNewObject`, …) zu Berechtigungen. |
| `CPermissibleGqlRequestHandlerComp` und Unterklassen | Handler mit den Attributen `CommandPermissions` (Anbieter) und `PermissionChecker`. |
| `ImtAuthPck/CheckPermissions` / `CheckPermissionOr` | Prüfer für **alle** (AND) / **beliebige** (OR). |

### 6.2. Prüfalgorithmus (`imtservergql::CheckRequestPermissions`)

1. Kein Anbieter verbunden → **zulassen** (13.10).
2. Kein Kontext oder Benutzer → **ablehnen**.
3. Superuser (`IsAdmin`) → **zulassen**. Der Organisationsinhaber wird wie alle geprüft: der gesamte scope der Organisation liegt bereits in den Berechtigungen seines Kontexts (Abschnitt 10).
4. Befehl ist nicht im Anbieter enthalten → **ablehnen** (`denyUndeclared = true`).
5. Kein Prüfer verbunden oder die Berechtigungsliste des Befehls ist leer → **zulassen**.
6. Die Anfrage enthält den Header `permissionPath` (6.4) → der Pfad muss **einer der Befehlsberechtigungen entsprechen oder innerhalb dieser liegen** und zu den wirksamen Berechtigungen des Benutzers gehören.
7. Andernfalls vergleicht der Prüfer die **wirksamen** Benutzerberechtigungen (Abschnitt 5) mit der deklarierten Liste.

### 6.3. Regeln

- **R6.1.** Jeder Befehl eines von `CPermissibleGqlRequestHandlerComp` abgeleiteten Handlers muss in einem Anbieter deklariert sein. Nicht deklarierte Befehle werden abgelehnt.
- **R6.2.** Immer **beide** Attribute verbinden (`CommandPermissions` und `PermissionChecker`). Fehlt eines, ist der Handler für alle offen (fail-open; siehe `ImtCore/Docs/GQL_CONTEXT_THREAD_PROPAGATION_BUG.md`).
- **R6.3.** Öffentliche Befehle (Anmeldung, Token, Sitzungsaktualisierung, Registrierung) werden nicht als permissible-Handler eingerichtet.
- **R6.4.** Konvention „Aktion → Berechtigung“ (R3.4): Lesen → `<Area>/View`; Erstellen → `<Area>/Edit/Add`; Ändern, Umbenennen, Beschreibung ändern → `<Area>/Edit/Change` (oder Feldblatt `Edit/Change/<Field>`); Löschen → `<Area>/Edit/Remove`. Beispiel: Lisa `ProductPermissions.acc`. Export → `<Area>/View`, Import → `<Area>/Edit/Add`, gelöschtes Objekt wiederherstellen → `<Area>/Edit/Remove` (13.21).
- **R6.5.** Ein Serverbefehl erhält in der Regel **eine** Berechtigung; mehrere bedeuten AND (`CheckPermissions`). GUI-Elemente (Seiten, Schaltflächen) verwenden einen **OR**-Prüfer (`Command.acc` und `Page.acc` verwenden `CheckPermissionOr`).
- **R6.6.** Für Standardsammlungen `ObjectCollectionPermissions` verwenden: `GetObjectData/Headers/ElementIds/MetaInfo/…` → `<Area>/View`; `InsertNewObject` → `<Area>/Edit/Add`; `SetObjectData/Name/Description` → `<Area>/Edit/Change`; `RemoveElements(Set)` → `<Area>/Edit/Remove`.
- **R6.7. Revisionen sind eine Untergruppe `Revisions` jedes Bereichs** (R3.4). Revisionscontroller (`CDocumentRevisionControllerComp`, entfernter `CRemoteDocumentRevisionControllerComp`) erhalten `RevisionPermissionPaths`, einen Pfad für jeden Eintrag in `CollectionIds`, in derselben Reihenfolge (`Devices` → `/SensorManagement/Revisions`). Ein Befehl erfordert das Blatt für seine Aktion im Bereich der Sammlung: `GetRevisionInfoList` → `Revisions/View`, `RestoreRevision` → `Revisions/Restore`, `DeleteRevision` → `Revisions/Delete` (`imtservergql::CheckRevisionPermissions`). Für eine Sammlung ohne Pfad und für jeden anderen Befehl (Platzhalter `BackupRevision`, `ExportRevision`) wird der Zugriff abgelehnt. Revisionen einer Entität gewähren keinen Zugriff auf Revisionen einer anderen. Clientseitige Verlaufskomponenten (`DocumentHistoryView`, `DocumentRevisionDialog`, Sammlungsdelegaten) erhalten den Bereichspfad vom Host über `revisionsPermissionPath`.
- **R6.8.** `*` darf nicht in Berechtigungsdeklarationen verwendet werden: Der OR-Prüfer lässt damit jeden Benutzer durch (13.15).

### 6.4. `permissionPath`

Eine wiederverwendbare Komponente (Ticketbereich, Chat, `ClientStyle`) kann an verschiedenen Stellen eingebettet sein. Der Host übergibt ihr `permissionPath` (QML-Eigenschaft → HTTP-Header `permissionPath`); der Pfad beginnt immer mit `/` (falls nötig, wird es ergänzt). Verwenden, wenn ein Befehl mehrere Kontexte mit unterschiedlichen Berechtigungen bedient.

Beispiel: Der Befehl `TicketsList` ist mit `/Support/ViewTickets` und `/Devices/ViewDeviceTickets` deklariert.

| Header `permissionPath` | Wirksame Berechtigungen des Benutzers | Ergebnis |
|---|---|---|
| `/Devices/ViewDeviceTickets` | `/Devices/ViewDeviceTickets` | **ja**—für den Befehl deklariert und vom Benutzer besessen |
| `/Devices/ViewDeviceTickets` | `/Support/ViewTickets` | nein—der Befehl läuft im Gerätekontext; Support-Berechtigung gilt dort nicht |
| `/Devices` | `/Devices/ViewDeviceTickets` | nein—Pfad ist weiter als die deklarierte Berechtigung; sonst würde jedes Blatt unter `/Devices` genügen (13.12) |

## 7. GUI-Seiten und Befehle

- **Seiten.** `PagePermissions` = `['<Area>/ViewPage']`—nur das Seitenblatt (R3.4). Befehle und Daten einer Sammlung, einschließlich `GetCommands`, bleiben unter `<Area>/View`: Eine Sammlung kann auch ohne Seite angezeigt werden. Die Administrationsseite wird über eines von `/Administration/{Users,Roles,Groups}/ViewPage` geöffnet; verschachtelte Seiten haben jeweils ihr eigenes `ViewPage` (`AdministrationView.qml`). Beispiel: `Lisa/Partitura/LisaQmlVoce.arp/Pages.acc`.
- **Sammlungs- und Dokumentbefehle.** Attribute `NewPermissions`, `EditPermissions`, `SavePermissions`, `UndoPermissions`, `RedoPermissions`, `ClosePermissions` in `StandardCollectionCommands` / `StandardDocumentCommands`. Beispiel (Lisa, Features): `New` → `/FeatureManagement/Edit/Add`; `Edit` → `Edit/Change | Edit/Remove | View`; `Save/Undo/Redo` → `Edit/Add | Edit/Remove | Edit/Change`; `Close` → jede Berechtigung, einschließlich `View`.
- **Filterung auf dem Server.** `CCommandsControllerComp` und der Seitencontroller geben dem Client nur Elemente zurück, die mit den **wirksamen** Berechtigungen zugänglich sind. Dies ist für **Menüs, Seiten und Befehlsleisten** erforderlich.
- **Lokale Prüfungen in QML.** `PermissionsController.checkPermission(id)` (Singleton `imtauthgui`): Superuser (`login === "su"`) → `true`; andernfalls prüft `hasPermission(permissions, path)` die vom Server empfangenen wirksamen Berechtigungen (R5.6) nach derselben Regel wie auf dem Server (Abschnitt 4): ausschließlich vollständige Pfade; bei einer Gruppenprüfung genügt „mindestens ein Blatt darin“.
- **R7.1.** GUI-Prüfungen duplizieren die Serverprüfung, ersetzen sie aber nicht. Jede über eine Schaltfläche verfügbare Aktion muss durch einen Serverbefehl geschützt sein.
- **R7.2.** Eine Komponente hat genau eine feste Berechtigungsmenge. Berechtigungen nicht in QML an `.acc` vorbei kodieren: Seiten und Befehle beziehen ihre Berechtigungen aus Partitura.

## 8. Herkunft der Benutzerberechtigungen

```
Berechtigungen(product, tenant) = Rollen des Benutzers im Produkt
                                   (lokale Rollenberechtigungen + Elternrollen, transitiv)
                                ∪ Gruppen des Benutzers (Elterngruppen, Gruppenrollen)
                                ∪ lokale Benutzerberechtigungen
    → nur Rollen/Gruppen/Berechtigungen der aktuellen Organisation
    → (bei PAT) ∩ Token-scopes
    → + durch Requirements eingeschlossene Berechtigungen (Abschnitt 5), ∩ Organisationsberechtigungen
    = wirksame Berechtigungen
```

- **R8.1. Berechtigungen sind produktspezifisch** (`IRole::GetProductId`, `IUserBaseInfo::GetRoles(productId)`, `GetPermissions(productId)`). Der Server bestimmt das Anfrageprodukt anhand seiner Konfiguration, nicht der Client (13.13). Rollen aus einem anderen Produkt öffnen keine Befehle dieses Produkts.
- **R8.2. Organisation (Tenant).** Für eine Organisation werden nur ihr **zugeordnete** Rollen, Gruppen und Berechtigungen berücksichtigt; bei „keine Organisation“ nur nicht zugeordnete (globale). Eine Elternrolle/-gruppe aus einer anderen Organisation gewährt nichts. Eine delegierte Rolle (Cross-Org-Delegation) zählt nur, wenn ihr Produkt übereinstimmt (`imtauthgql::AdaptUserForTenant`, `GetEffectiveUserPermissions`, `GetTenantUserPermissions`). Dies gilt auch bei Befehlsprüfungen, einschließlich in Puma selbst (13.14).
- **R8.3. Zyklen in Rollen und Gruppen sind verboten:** Eine Elternrolle/-gruppe, die einen Zyklus schließt, wird mit Fehler abgelehnt (`CreatesRoleCycle`, `CreatesGroupCycle`).
- **R8.4. Verbote (`prohibitions`)** werden gespeichert, aber bei der Berechnung von Berechtigungen **nicht angewendet** (13.8). Nicht verwenden.
- **R8.5.** Für entfernte Verbraucher (ProLife usw.) werden aus Rollen stammende Berechtigungen in einer serialisierbaren Benutzerkopie **materialisiert** (`CUserCollectionControllerComp::CreateAdaptedObjectData`); andernfalls hat ein PAT-Benutzer ohne Rollen keine Berechtigungen.
- **R8.6.** Benutzer sehen ihre zugeordneten Rollen und Gruppen unabhängig von Berechtigungen: Das Lesen eigener Rollen (einschließlich delegierter) und Gruppen wird ohne Prüfung zugelassen. Rollen und Gruppen anderer Benutzer erfordern Berechtigungen (bei Gruppen ist dies derzeit abweichend, siehe 13.7).

## 9. Privilegierte Identitäten

| Identität | Erkennung | Verhalten |
|---|---|---|
| **Superuser** | `CUserInfoComp::IsAdmin()`: `UserId == AdminId` (`AdminId = su` in Kompositionen). Der Client prüft `login == "su"`. | Umgeht alle Prüfungen von Befehlsberechtigungen; der Server gibt eine **leere** Berechtigungsliste zurück. |
| **Organisationsinhaber** | `ITenantInfo::GetOwnerId() == userId` für die aktuelle Organisation (im Token zwischengespeichert). | Umgeht keine Prüfung: hält den gesamten scope der eigenen Organisation als Kontextberechtigungen (`imtgql::CollectTenantOwnerPermissions`) und nichts darüber hinaus; sieht den scope nur lesend. |

**R9.1.** Superuser nicht anhand der Berechtigungsliste erkennen: Sie ist leer und nicht von einer Liste eines Benutzers ohne Berechtigungen zu unterscheiden. Tests verwenden dafür `permissions: ['*']` (`GuiTestKit/fixtures/defineUsers.js` akzeptiert `su` sonst nicht).
**R9.2.** Nur Superuser dürfen Konten aktivieren/deaktivieren (`IsSuperuserRequest`), fremde Profile ändern (`OnSetProfile`), deaktivierte Konten erstellen und PATs mit beliebigen scopes ausgeben.
**R9.3.** Für ein deaktiviertes Konto wird kein Kontext erstellt (`CCS_FORBIDDEN`); Tokens und Sitzungen funktionieren sofort nicht mehr.
**R9.4.** Die Umgehung für privilegierte Identitäten hebt Tokenbeschränkungen nicht auf: Ein PAT mit engen scopes bleibt auch für Superuser und Organisationsinhaber eingeschränkt (13.16).

## 10. Organisationen und PATs

**Organisationen (Tenants).**
- `ITenantInfo::GetTenantPermissions()` ist der **scope**, die Obergrenze: die Produktberechtigungen, die der Organisation zur Verfügung stehen. Innerhalb der Organisation wirkt nichts außerhalb des scope, **ein leerer scope gewährt nichts** (`imtgql::CollectImpliedPermissions`).
- **Nur der Ersteller** der Organisation (`CreatorId`) setzt den scope, in `UpdateTenant` und im Organisationseditor (`imtauthgql::CheckTenantPermissionsChange`): „Only the creator of the organization can change its permissions“. Der Editor erhält `canEditTenantPermissions` und für eine Unterorganisation den scope des Elternteils `parentTenantPermissions`.
- **Der Inhaber** hält den gesamten scope als Berechtigungen (Abschnitt 9) und sieht ihn nur lesend.
- **Anlegen.** Eine Organisation legt der Superuser oder ein Benutzer mit `/OrganizationManagement/Add` an (`imtauthgql::CanCreateOrganization`, in `CreateTenant` und im Dokumentdienst `Tenants`). Eine aus einer anderen Organisation heraus angelegte Organisation ist deren Kind: das Elternteil wird nicht gewählt, es ist die Organisation des Kontexts (ein abweichendes `parentTenantId` wird abgelehnt), und `/OrganizationManagement/Add` muss auch im Eltern-scope liegen. Der scope einer Unterorganisation ist eine Teilmenge des Eltern-scope: „Permission '%1' is not available to the parent organization“; geprüft bei jeder Änderung des scope und bei jedem Speichern des Dokuments (`CTenantDocumentValidatorComp`). `CreateTenant` gibt einer Unterorganisation den Standardsatz, gekürzt auf den Eltern-scope.
- **Kaskade.** Das Einschränken eines scope kürzt den scope aller Nachkommen (`ITenantManager::SetTenantPermissions`).
- **Standard-scope** einer über `CreateTenant` angelegten Stammorganisation: die Blätter `/Administration/{Users, Roles, Groups}/{ViewPage, View, Edit/Add, Edit/Change, Edit/Remove, Revisions/View}` (`CTenantManagerComp`).
- **Die Seite „Organisationen“** ist sichtbar, sobald es mindestens eine Organisation oder eine offene Einladung gibt, außerhalb jeder Organisation auch für Inhaber von `/OrganizationManagement/Add`: der Server fügt `/OrganizationManagement/{ViewPage, View}` innerhalb einer Organisation den Mitgliedern, dem Inhaber und dem Ersteller hinzu, außerhalb jeder Organisation einem Mitglied irgendeiner Organisation, einem Eingeladenen und einem Benutzer, der Organisationen anlegen darf (`CAuthenticationManagerComp::HasOrganizationAccess`). Auf Produktservern (ohne Mitgliedschaftsmanager) außerhalb jeder Organisation nur Inhabern von `Add` (13.30).
- **Cache.** scope, Inhaberkennzeichen und Organisationszugang werden für `TenantDataCacheLifetime` (standardmäßig 10 s) am Token zwischengespeichert: Änderungen erreichen offene Sitzungen spätestens nach dieser Zeit.
- Eine Organisationsrolle darf nur Berechtigungen aus dieser Liste enthalten (`CRoleDocumentValidatorComp`: „Permission '%1' is not allowed for tenant“).
- Organisationsberechtigungen müssen mit Requirements übereinstimmen: Beim Hinzufügen einer Berechtigung ergänzt der Editor selbst die Blätter, die sie einschließt. Andernfalls würde `ChangeUser` ohne `ViewUsers` Änderungen ohne Anzeige ermöglichen.
- `GetProductPermissions(productId, tenantId)` gibt den auf Organisationsberechtigungen beschränkten Baum zurück; `GetUserPermissions(productId)` liefert die Berechtigungen des aktuellen Benutzers (Superuser erhalten den gesamten Baum).
- „Globale“ Berechtigungen in der Autorisierungsantwort sind Benutzerberechtigungen, die zu keiner Organisation gehören, in der der Benutzer Mitglied ist (`CalculateGlobalPermissions`).
- Organisationsverwaltung verwendet einen eigenen Baum (R3.6); Eigentümer/Ersteller gelten als Organisationsadministratoren.

**Personal Access Tokens.**
- Token-scopes sind eine Teilmenge der Berechtigungen des **Erstellers**: `HasPermission(wirksame Berechtigungen des Erstellers, scope)`. Ein Superuser kann beliebige scopes vergeben; ein eingeschränktes Token kann kein umfassenderes Token erstellen.
- Ein Scope ist der Pfad einer Berechtigung oder Gruppe (`/OrderManagement/View`, `/OrderManagement`); ein Scope mit bloßer Kennung (`ViewOrders`) deckt nichts ab.
- PAT-Berechtigungen = `Benutzerberechtigungen ∩ scopes` nach derselben `HasPermission`-Regel (13.4). Requirements werden **aus dieser Schnittmenge** expandiert, nicht aus allen Benutzerberechtigungen: Ein Token mit Scope `ChangeUser` kann Benutzer sehen, aber nichts darüber hinaus (Abschnitt 5). Rollen und Gruppen werden geleert; **eine leere Scope-Liste gewährt keine Berechtigungen** (fail-closed).

## 11. Tests

- **Unit-Tests:** `ImtCore/Tests/PermissionRequirementsTest` (25) prüft `HasPermission` (Tabelle in Abschnitt 4, bloße Kennungen) und Requirement-Expansion (Tabellen in Abschnitt 5: vier Abhängigkeitstypen, Transitivität, Zyklen, Organisationsgrenze, Anfrageprodukt und unzulässige Abhängigkeiten). `ImtCore/Tests/PermissionCheckTest` (38) prüft vollständige Serverbefehle (`CheckRequestPermissions`): Identitäten, Deklarationen, alle Requirement-Typen, Produkt und Organisation, `permissionPath`, GUI-Elemente und Revisionen (`CheckRevisionPermissions`). Neue Fälle wurden mutiert getestet: Jeder schlägt fehl, wenn Vergleiche bloßer Kennungen oder Revisionsprüfungen anhand einer Gruppe bzw. einer fremden Sammlung wieder eingeführt werden.
- **API (Postman):** Puma `TenantApiPostman`, Ordner 20 „Organization permissions by path“ (Organisationsberechtigungen anhand von Pfaden; bloße Kennungen und Gruppen gewähren nichts), Ordner 00D „Organization creators“ (Recht, Organisationen anzulegen) und 23 „Organization permission scope“ (scope vom Ersteller, der Inhaber hält den gesamten scope und kann ihn nicht ändern, Unterorganisation über die API und über das Dokument, ein scope breiter als das Elternteil wird abgelehnt, Kaskade, leerer scope, Sichtbarkeit der Seite durch Einladung); ProLife `ProLifeApiPostman`, Ordner 11 „Permission Model“ (Serverprodukt, PAT-scopes mit Pfaden und bloßen Kennungen, Übereinstimmung der `GetPermissions`-Ergebnisse mit Serverentscheidungen).
- **Server-API-Tests:** Für jeden geschützten Befehl wird ein Benutzer ohne Berechtigung abgewiesen; bei jedem Requirement schließt die Berechtigung ihr Ziel ein, das Ziel gewährt aber nicht umgekehrt die Quelle.
- **GUI (`GuiTestKit`).** Keine Matrizen „Berechtigung → Seite“ pflegen: Der Client wird aus den Benutzerberechtigungen aufgebaut, daher fragen Tests den Client direkt ab—`page.isAvailable()`, `page.commands.isAvailable(id)`, `admin.hasSubPage(id)`. Übersprungen wird nur mit `requires(user, permission)` (nicht `!user.can(x)`: „nicht ermittelbar“ bedeutet nicht „keine Berechtigung“).
- **Benutzer-Fixtures** führen Blätter **so auf, wie der Server sie benennt** (`GetProductPermissions(productId)`). `seed.js` prüft sie gegen den aktuellen Baum; eine veraltete Kennung lässt den Testlauf fehlschlagen. Eingeschlossene Berechtigungen werden nicht in Fixtures aufgeführt, sondern vom Server ergänzt. Liste nicht aus `.acc`/QML ableiten: `EditFeature` in `PagePermissions` ist eine Gruppe, keine vergebbare Berechtigung.

## 12. Checkliste

**Berechtigung hinzufügen:** (1) in Lisa `FeatureEditor` ein Blatt mit `IsPermission=true` nach R3.2–R3.5 anlegen; (2) falls nötig Requirements gemäß R5.3–R5.4 hinzufügen (meist Gruppe `<Area>/Edit` → Blatt `<Area>/View`); (3) `<Product>Features.h` neu generieren; (4) Lokalisierung (`Feature`) ergänzen; (5) `FULL_ACCESS` in GUI-Test-Fixtures aktualisieren.

**Befehl schützen:** (1) `SingleCommandPermissionsProvider` (`CommandId` = GraphQL-Befehlskennung, `CommandPermissions` = vollständiger Blattpfad); (2) zu `CommandPermissionsProviderJoiner` hinzufügen; (3) Handler von `CPermissibleGqlRequestHandlerComp` ableiten und `CommandPermissions` sowie `PermissionChecker` verbinden; (4) für kontextabhängige Berechtigungen `permissionPath` verwenden (6.4); (5) testen, dass ein Benutzer ohne Berechtigung abgewiesen wird.

**Seite/Schaltfläche:** `PagePermissions` verwendet das Blatt `<Area>/ViewPage`; `*Permissions` in `Standard*Commands` verwenden Datenblätter (`<Area>/View`, `<Area>/Edit/*`).

**Revisionen einer neuen Sammlung:** Untergruppe `<Area>/Revisions` zum Baum hinzufügen (R3.4) → `<Area>/Revisions` an der Position der Sammlung in `RevisionPermissionPaths` des Revisionscontrollers eintragen (R6.7) → `revisionsPermissionPath` am QML-Verlaufshost setzen.

**Neues Produkt:** Baum in Lisa erstellen → `Features.h` → `FillProduct` in `Main.cpp` → Komponente `SingleProductPermission` (`ProductId`, `PathToPermissionsFile`) → `FeatureContainer` in `AuthenticationManager` (ohne diese werden Requirements nicht expandiert, R5.2) → `AdminId=su`, `PatPrefix`.

## 13. Codeabweichungen und offene Fragen

Ausgangszustand: ImtCore `442be502b9`; „Behoben“ bedeutet, dass die Änderung in den Branches aus der Kopfzeile enthalten ist. „Laut Code“ ist eine Schlussfolgerung aus dem Code, nicht durch einen Test bestätigt.

| Nr. | Feststellung | Stelle | Maßnahme |
|---|---|---|---|
| 13.1 | **Der Mechanismus war korrekt, aber Daten und Gruppen waren falsch.** Der Code bedeutet „A besitzen ⇒ A's Requirements besitzen“ (`imtlic::CollectImpliedPermissions`, aufgerufen von `IGqlContext::GetImpliedPermissions()`; `GetPermissions(productId) ∪ GetImpliedPermissions()` in `imtservergql::GetUserPermissions`). In allen Produktdaten war die Richtung umgekehrt: `View*` → `Edit*`-Gruppe (Lisa, ProLife, NeoPro, IotPlatform, Acula, `COrganizationsPermissionsProviderComp`). Bei funktionierender Expansion hätte eine Rolle mit nur `View*` serverseitig `Edit*` erhalten. Außerdem gab ein Gruppen-Requirement den Gruppenpfad statt ihrer Blätter zurück; Ergebnisse waren nicht durch Organisationsberechtigungen begrenzt; PAT-Expansion erfolgte anhand aller Benutzerberechtigungen statt der scopes; R5.4 wurde nicht geprüft. | `imtlic/CProductInfo.cpp`, `imtgql/CGqlContext.cpp`, `imtservergql/imtservergql.h`, `*Features.h` | Im Code behoben (Gruppenziele zu Blättern expandieren, Gruppen-Requirements, Organisationsgrenze, PAT-Expansion aus scopes, Abhängigkeiten innerhalb desselben Zweigs verhindern) sowie in Lisa-, ProLife- und Organisationsbaumdaten: neue Bäume (R3.4, R3.6) verwenden `Edit → View`. **Offen:** NeoPro, IotPlatform, Acula, Agentino; vorhandene Rollen prüfen. |
| 13.2 | Behoben: `GetUserPermissions` verwendet die Berechtigungen des Anfrageprodukts (`GetPermissions(productId)`). | `imtservergql/imtservergql.h` | — |
| 13.3 | Clientseitiges `PermissionsController.hasPermission` implementierte keine **Gruppenprüfung** (das Blatt `.../EditUser/ChangeUser` wurde auf dem Client bei Prüfung von `.../EditUser` mit `false`, auf dem Server mit `true` bewertet). | `ImtCore/Qml/imtauthgui/PermissionsController.qml` | Behoben: nur Pfade; Gruppenprüfung bedeutet „Blatt darin vorhanden“. |
| 13.4 | PAT-scope-Filterung verwendete **exakte Übereinstimmung** (`scopeSet.contains`), während die Vergabe von scopes `HasPermission` verwendete. Bloße Kennungen führten zu Abweichungen. | `imtservergql/CAuthenticationManagerComp.cpp` | Behoben: `HasPermission` verwenden; bloße scopes decken nichts ab. |
| 13.5 | Bloße Kennungen werden nicht mehr unterstützt (R4.3), aber gespeicherte Rollen-, Benutzer-, Organisationsmitglied-, PAT-scope- und Organisationsberechtigungen sind teilweise bloße Kennungen oder **alte Pfade** (`/SensorManagement/EditSensor/AddSensor`). Nach dem Deployment funktionieren diese nicht mehr. `imtauthdb::CPermissionPathMigrationComp` (Datenbankmigrationsschritt mit `imtdb::IMigrationController`, in `ImtAuthDbPck`; noch in keiner Komposition eingebunden) wandelt bloße Kennungen nur anhand des aktuellen Baums in Pfade um: alte IDs (`AddSensor`) sind im neuen Baum nicht vorhanden, kurze IDs (`Add`) sind mehrdeutig; beide bleiben daher unverändert. Auch Lisas Feature-Katalog in der Datenbank enthält noch den alten Baum. | Puma- und Lisa-Datenbanken, `imtauthdb/CPermissionPathMigrationComp` | **Vor dem Deployment offen:** Datenbank anhand einer Zuordnung „alte ID / alter Pfad → neuer Pfad“ migrieren (für Lisa und ProLife vorhanden) und Lisa-Katalog aktualisieren. |
| 13.6 | Inkonsistente Zuordnung: `PackagesSetDescription` → `AddFeature` (R6.4 verlangt `ChangeFeature`, wie bei `ProductsSetDescription`). | `Lisa/Partitura/LisaVoce.arp/FeaturesPermissions.acc` | Klären. |
| 13.7 | `CUserGroupCollectionControllerComp` überspringt `GroupItem`, `GetObjectData` und `GetObjectTypeId` für **jede Gruppe**; ein aus einer anderen Organisation delegierter Benutzer kann **jeden** Gruppenbefehl einschließlich Ändern und Löschen ausführen. | `imtauthgql/CUserGroupCollectionControllerComp.cpp` | Nur das Lesen eigener Gruppen ohne Prüfung zulassen (R8.6). |
| 13.8 | `prohibitions` werden gespeichert und serialisiert, aber bei der Berechtigungsberechnung nicht berücksichtigt (`CRole::GetProhibitions` gibt eine leere Liste zurück). | `imtauth/CRole.cpp`, `CUserBaseInfo.cpp` | Implementieren oder entfernen. |
| 13.9 | Bei gleichem `CommandId` gewinnt im Joiner stillschweigend der erste Anbieter. | `imtserverapp/CCommandPermissionsProviderJoinerComp.cpp` | Diagnose ergänzen. |
| 13.10 | Fail-open: Nicht verbundene `CommandPermissions`/`PermissionChecker` öffnen den Befehl für alle. | `imtservergql::CheckRequestPermissions` | Konfiguration beim Start prüfen. |
| 13.11 | Der Test `testContainerCannotBeRequirementTarget` hat einen falschen Namen: Sein Ziel `/Products/EditProducts` ist ein Blatt, keine Gruppe. Ein Lauf der erstellten `PermissionRequirementsTest.exe` (2026-10-16) bestand 12 Tests und schlug bei 7 fehl; bei allen fehlgeschlagenen Expansionstests lieferte `CollectImpliedPermissions` eine leere Liste. Das Binary könnte mit einer alten `imtlic.lib` erstellt worden sein (paralleler Build). | `ImtCore/Tests/PermissionRequirementsTest` | Behoben: Test überarbeitet (25 Fälle), `PermissionCheckTest` hinzugefügt (38); alle bestehen (Abschnitt 11). |
| 13.12 | `permissionPath` wurde als „deklarierte Berechtigung liegt innerhalb des Pfads“ geprüft: Gruppenpfad `/Devices` wurde für einen mit `/Devices/ViewDeviceTickets` deklarierten Befehl akzeptiert; danach genügte **jedes** Blatt unter `/Devices`. Außerdem ersetzte der Pfad die AND-Liste des Befehls durch eine einzelne Berechtigung. Dasselbe galt für `IsElementAccessible`. | `imtservergql/imtservergql.h`, `imtserverapp/imtserverapp.h` | Behoben (`imtauth::IsPermissionPathDeclared`): Pfad ist gleich der deklarierten Berechtigung oder liegt innerhalb davon. |
| 13.13 | Das Anfrageprodukt stammte aus dem Client-Header `productId`; ohne Header wurden Berechtigungen aus **allen** Produkten gesammelt. Requirements wurden immer anhand der Berechtigungen aller Produkte expandiert (`m_userInfoPtr->GetPermissions()`). | `imtservergql/CAuthenticationManagerComp.cpp`, `imtgql/CGqlContext.cpp` | Behoben: Produkt stammt aus der Serverkonfiguration (`ProductInfo`), Header ist nur Fallback; Requirements expandieren anhand der Anfrageprodukt-Berechtigungen. |
| 13.14 | Laut Code wird der Benutzer in Puma im Kontext nicht an die Organisation angepasst (`AdaptUserForTenant` wird nur beim Zurückgeben von Daten aufgerufen); daher berücksichtigen Puma-Befehlsprüfungen Rollen aus allen Organisationen des Benutzers. Produktserver erhalten den bereits von Puma angepassten Benutzer. | `imtservergql/CAuthenticationManagerComp.cpp`, Puma `PumaServerBase.acc` | Mit Test bestätigen; Benutzer beim Erstellen des Kontexts anpassen. |
| 13.15 | `"*"` in der Berechtigungsliste eines Elements oder Befehls lässt jeden Benutzer durch `CheckPermissionOr`. | `imtauth/CCheckPermissionOrComp.cpp` | In Deklarationen verbieten (R6.8). |
| 13.16 | Schritt 3 des Algorithmus (Superuser/Eigentümer → erlauben) lief vor allen Berechtigungsprüfungen. Hatte ein PAT des Organisationsinhabers `IsTenantOwner`, erhielt ein Token mit engen scopes vollen Zugriff. | `imtservergql::CheckRequestPermissions` | Behoben: die Umgehung für den Inhaber entfällt, der Inhaber hält den scope als Berechtigungen; ein PAT erhält keine Inhaberberechtigungen. |
| 13.17 | `CGuiElementRepresentationControllerComp` behandelt fehlendes `UserInfo` als Superuser (zeigt alles) und sammelt Berechtigungen aus allen Produkten; ein Organisationsinhaber sieht eine gefilterte GUI, obwohl der Server ihn zulässt. | `imtserverapp/CGuiElementRepresentationControllerComp.cpp` | Niedrige Priorität: Die GUI ist keine Sicherheitsgrenze. |
| 13.18 | Der Produktbaum wird für jede Anfrage erneut durchlaufen (`CollectProductRequirements` im Anfragekontext, mit tiefen Feature-Kopien). | `imtlic/CProductInfo.cpp` | Requirements werden bei Bedarf expandiert (R5.2), der analysierte Produktbaum sollte jedoch zwischengespeichert werden, statt ihn je Anfrage zu kopieren. |
| 13.19 | Organisationsmitglied-Berechtigungen umgingen R4.1: exakte `contains`-Prüfungen mit bloßen Kennungen (`CanManageMembers`, Berechtigungszuweisung an Mitglieder, `HasTenantManageAccess`, Organisationsadmin-Kennzeichen) sowie bloße Gruppen-IDs (`EditOrganization`, `EditOrganizationMember`) in `CTenantCollectionDocumentServiceComp`. Eine bloße Gruppen-ID gewährte die gesamte Mitgliederverwaltung; pfadbasierte Berechtigungen funktionierten nicht. | `imtauthgql/CTenantMembershipManagerControllerComp.cpp`, `TenantAccess.cpp`, `CTenantCollectionDocumentServiceComp.cpp`, `CTenantManagerControllerComp.cpp`, `CCrossOrgGrantCollectionDocumentServiceComp.cpp` | Im Branch `feature/permission-requirements` behoben: Prüfungen verwenden `HasPermission` mit vollständigen Blattpfaden aus dem Organisationsbaum (`imtauthgql/TenantAccess.h`); eine Gruppe ist keine Berechtigung (R3.5). Organisationsbaum neu aufgebaut (R3.6); der Editor erhält vollständige Pfade (`BuildPermissionGroupsFromProvider`) statt bloßer Kennungen. |
| 13.20 | Revisionen verwendeten eine bereichsübergreifende Gruppe `RevisionManagement`: `ViewRevisions` öffnete den Verlauf **aller** Entitäten, und der Client prüfte die bloße Kennung `ViewRevisions`. | `CDocumentRevisionControllerComp`, `CRemoteDocumentRevisionControllerComp`, `ImtCoreVoce.arp/RevisionPermissions.acc`, `imtdocgui` | Behoben: Untergruppe `Revisions` in jedem Bereich (R3.4, R6.7); Revisionscontroller in Puma, ProLife, Lisa sowie Puma-Client verwenden `RevisionPermissionPaths`; `RevisionPermissions.acc` wurde entfernt. Offen: Revisionsbefehle sind als `Query` deklariert; Wiederherstellung und Löschen sollten `Mutation` sein. |
| 13.21 | Export, Import, Wiederherstellung von Entitäten und „Alle anzeigen“ (`ViewAll*`) waren mit bloßen Kennungen ohne Baumknoten deklariert und daher nur für Superuser und Organisationsinhaber verfügbar. | `ImtUserAdministrationVoce.arp/{User,Role,Group,Tenant}Permissions.acc`, ProLife `{Account,Software,Order,Device}Permissions.acc`, `*Page.acc` | Behoben: Export → `<Area>/View`, Import → `<Area>/Edit/Add`, Wiederherstellung → `<Area>/Edit/Remove` (bei Organisationen `/OrganizationManagement/Add` und `/Remove`), `ViewAll*` → `<Area>/ViewAll`. Nur `ViewServerPage` (ProLife-Servereinstellungen) bleibt absichtlich eine bloße Kennung und ist nur für Superuser verfügbar. |
| 13.22 | In `PagesController.acc` war die Administrationsseite mit dem nicht existierenden Pfad `/Administration/EditAdministration/EditRole/AddGroup` deklariert; außerdem fehlte die Leseberechtigung für Rollen in der Liste. | `ImtCore/Partitura/ImtGraphQlVoce.arp/PagesController.acc` | Behoben: Seite wird durch jedes von `/Administration/{Users,Roles,Groups}/ViewPage` geöffnet (R3.4). |
| 13.23 | Mitgliedsblätter unterscheiden Aktionen nicht: Einladen, Ausschließen und Rollenwechsel sowie das Ändern der Mitgliedsberechtigungen (`UpdateMembershipPermissions`) werden durch **jedes** Blatt `Members/Edit/*` erlaubt; `Permissions/Change` wird nicht geprüft. | `imtauthgql/CTenantMembershipManagerControllerComp.cpp` (`CanManageMembers`) | Aktionsspezifisches Blatt prüfen. |
| 13.24 | Organisationsrollen, -gruppen und -benutzer werden mit `Get/Update{Role,Group,User}{From}Representation` bearbeitet, die nur unter Produktpfaden `/Administration/{Roles,Groups,Users}/...` deklariert sind. Mitgliedsberechtigungen `Roles/*` und `Groups/*` öffnen diese Befehle nicht (die Produktpfade für Organisationen in den Deklarationen zeigten bereits vor dieser Arbeit auf nicht vorhandene Knoten und wurden entfernt). | `ImtAuthVoce.arp/{Role,User,UserGroup}CollectionDocumentService.acc`, `imtauthgql`-Dokumentdienste | Entscheiden, ob die Dokumentdienste im Organisationskontext Mitgliedsblätter wie `Roles/Edit/Change` prüfen sollen. |
| 13.25 | Die Suchseite ist mit `*` deklariert (für alle sichtbar, 13.15); ProLifes `/SearchManagement/ViewPage` wird in Deklarationen nicht verwendet. Die Seite ist produktübergreifend, Lisa hat kein entsprechendes Blatt. | `ImtGraphQlVoce.arp/PagesController.acc` (`SearchPage`) | Die Seite in der Komposition jedes Produkts mit einem Produktblatt deklarieren. |
| 13.26 | `GraphQLDemultiplexer` (`CGqlDemultiplexerComp`) hat keine Attribute `CommandPermissions`/`PermissionChecker`: `TenantController.acc` setzt sie, sie werden aber ignoriert, daher prüft Puma die Deklarationen aus `TenantPermissions.acc` nie. Organisationsbefehle sind nur durch die Prüfungen in den Controllern geschützt (`HasTenantManageAccess`, `CanCreateOrganization`, ...). | `imtservergql/CGqlDemultiplexerComp.*`, `ImtUserAdministrationVoce.arp/TenantController.acc` | Entscheiden: Deklarationen am Demultiplexer prüfen (ändert alle Organisationsbefehle) oder die toten Attribute entfernen. |
| 13.27 | Jeder Benutzer außerhalb einer Organisation konnte eine Stammorganisation anlegen und ihr einen beliebigen scope geben. | `CTenantManagerControllerComp::OnCreateTenant`, `CTenantCollectionDocumentServiceControllerComp` | Behoben: nur der Superuser oder ein Inhaber von `/OrganizationManagement/Add`. |
| 13.28 | scope und Inhaberkennzeichen wurden für `TokenCacheLifetime` (5 Minuten) am Token zwischengespeichert: ein eingeschränkter scope, eine Inhaberübertragung oder eine Mitgliedschaft erreichten offene Sitzungen verspätet. | `CAuthenticationManagerComp::TryGetCachedTenantData` | Behoben: eigene `TenantDataCacheLifetime` (10 s) für scope, Inhaberkennzeichen und Organisationszugang; die Abfrage von Mitgliedschaften und Einladungen liegt im selben Cache. |
| 13.29 | Das Elternteil eines neuen Organisationsdokuments folgt dem Kontext der letzten Änderung; ein in A begonnenes und nach dem Wechsel nach B gespeichertes Dokument bleibt eine Unterorganisation von A. | `CTenantCollectionDocumentServiceComp` | Akzeptiert: `Add` wird auch beim Speichern geprüft, der Validator prüft den scope gegen das Elternteil. |
| 13.30 | Außerhalb jeder Organisation zeigen Produktserver die Seite „Organisationen“ nur Inhabern von `Add`: sie haben keine Mitgliedschafts- und Einladungsmanager. | `CAuthenticationManagerComp::HasOrganizationAccess` | Akzeptiert: ohne das Recht, Organisationen anzulegen, ist die Seite nur innerhalb einer Organisation oder mit Einladung sichtbar (auf Puma); entfernte Abfrage an Puma ist Phase 1b. |

## 14. Aktueller Stand der Produkte

| Produkt | Berechtigungsknoten | Requirements-Einträge | Format | Richtung | Datei |
|---|---:|---:|---|---|---|
| Lisa | 74 | 28 | Pfade, Baum R3.4 | `Edit` → `View`, Revisionen → `View` (korrekt) | `Lisa/Impl/LisaServer/LisaFeatures.h` |
| ProLife | 139 | 43 | Pfade, Baum R3.4 | `Edit` → `View`, `ViewAll`/`ViewHistory` → `View`, Revisionen → `View` (korrekt) | `ProLife/Impl/ProLifeServer/ProLifeFeatures.h` |
| IotPlatform | 89 | 7 | Pfade | `View*` → `Edit*` | `IotPlatform/Impl/IotPlatformServerExe/IotPlatformFeatures.h` |
| NeoPro | 35 | 4 | Pfade | `View*` → `Edit*` | `NeoPro/Impl/NeoProServer/NeoProFeatures.h` |
| Acula | 21 | 4 | Pfade | `View*` → `Edit*` | `Acula/Impl/AculaServer/AculaFeatures.h` |
| Agentino | 18 | 0 | — | — | `Agentino/Impl/AgentinoServer/AgentinoFeatures.h` |

## 15. Zentrale Dateien

| Thema | Datei |
|---|---|
| Vergleichsregel | `ImtCore/Include/imtauth/imtauth.h` (`HasPermission`) |
| Befehlsprüfung | `ImtCore/Include/imtservergql/imtservergql.h`, `CPermissibleGqlRequestHandlerComp.*` |
| Prüfer | `ImtCore/Include/imtauth/CCheckPermissionCompBase.cpp`, `CCheckPermissionOrComp.cpp` |
| GUI-Elemente | `ImtCore/Include/imtserverapp/imtserverapp.h` (`IsElementAccessible`), `imtservergql/CCommandsControllerComp.cpp` |
| Kontext und Requirements | `ImtCore/Include/imtgql/CGqlContext.*`, `imtlic/CProductInfo.cpp`, `imtlic/IFeatureInfo.h` |
| Anfragekontext, PAT, Organisation | `ImtCore/Include/imtservergql/CAuthenticationManagerComp.cpp` |
| Benutzerberechtigungen | `imtauth/CUserBaseInfo.cpp`, `CUserInfo.cpp`, `CRole.cpp`, `imtauthgql/imtauthgql.h` |
| GraphQL-Berechtigungsbaum | `ImtCore/Sdl/imtauth/1.0/Permissions.sdl`, `imtauthgql/CPermissionsProviderComp.cpp` |
| Befehlsberechtigungen (Partitura) | `ImtCore/Partitura/ImtCoreVoce.arp/*Permissions.acc`, `ImtAuthVoce.arp/ObjectCollectionPermissions.acc`, `Lisa/Partitura/LisaVoce.arp/*Permissions.acc` |
| Client | `ImtCore/Qml/imtauthgui/PermissionsController.qml`, `PermissionsProvider.qml`, `Qml/imtlicgui/FeatureEditor.qml` |
| Revisionen | `ImtCore/Include/imtservergql/imtservergql.h` (`CheckRevisionPermissions`), `CDocumentRevisionControllerComp.*`, `imtclientgql/CRemoteDocumentRevisionControllerComp.*`, `Qml/imtdocgui/DocumentHistoryView.qml` |
| Organisationsbaum | `ImtCore/Include/imtauth/COrganizationsPermissionsProviderComp.cpp`, `imtauthgql/TenantAccess.h` |
| Tests | `ImtCore/Tests/PermissionRequirementsTest`, `ImtCore/Tests/PermissionCheckTest`, Puma `Tests/TenantApiPostman`, ProLife `Tests/ProLifeApiPostman`, `ImtCore/Tests/GuiTestKit`, `Lisa/Tests/LisaGui` |
