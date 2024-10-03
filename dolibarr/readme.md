# Dolibarr

## Installation

Sur Discord, télécharger les fichiers et les mettre dans le dossier `conveyor/dolibarr`
- `.env`
- `conf.php`
- `db_backup.sql`
- `dolibarr_20.0.0-4_all.deb`

Puis :
- Lancer Docker Desktop
- Exécuter `sh launch.sh`
- Aller sur `localhost:8080/dolibarr/install`
- Suivre les étapes d'installation
- Pour configurer le lien à la base de données, utiliser les informations suivantes:
  - Nom de la base de données : `MYSQL_DATABASE` dans le `.env`
  - Serveur de base de données : `host.docker.internal`
  - Port : `8081`
  - Décocher "Créer la base de données"
  - Identifiant : `MYSQL_USER` dans le `.env`
  - Mot de passe : `MYSQL_PASSWORD` dans le `.env`
  - Décocher "Créer le propriétaire ou lui affecter les droits à la base"

## Modules

Tout d'abord, il faut activer les modules suivants :

- API / Web services
- Stock

## Ajouts initiaux

### Entrepôts

Ajouter trois entrepôts :

- Le A :

```json
{
  "ref": "A",
  "label": "A",
  "lieu": "A",
  "entity": "1",
  "statut": "1",
  "country_id": "1",
  "country_code": "FR"
}
```

- Le B :

```json
{
  "ref": "B",
  "label": "B",
  "lieu": "B",
  "entity": "1",
  "statut": "1",
  "country_id": "1",
  "country_code": "FR"
}
```

- Le C :

```json
{
  "ref": "C",
  "label": "C",
  "lieu": "C",
  "entity": "1",
  "statut": "1",
  "country_id": "1",
  "country_code": "FR"
}
```

### Produits

- Le cube vert :

```json
{
  "ref": "VERT",
  "label": "Cube vert",
  "fk_default_warehouse": "1",
  "entity": "1",
  "weight": "10",
  "weight_units": "-3",
  "length": "2",
  "length_units": "-2",
  "width": "2",
  "width_units": "-2",
  "height": "2",
  "height_units": "-2",
  "surface": "2",
  "surface_units": "-4",
  "volume": "2",
  "volume_units": "-6",
  "status_buy": "1"
}
```

- Le cube bleu :

```json
{
  "ref": "BLEU",
  "label": "Cube bleu",
  "fk_default_warehouse": "2",
  "entity": "1",
  "weight": "10",
  "weight_units": "-3",
  "length": "2",
  "length_units": "-2",
  "width": "2",
  "width_units": "-2",
  "height": "2",
  "height_units": "-2",
  "surface": "2",
  "surface_units": "-4",
  "volume": "2",
  "volume_units": "-6",
  "status_buy": "1"
}
```

- Le cube jaune :

```json
{
  "ref": "JAUNE",
  "label": "Cube jaune",
  "fk_default_warehouse": "3",
  "entity": "1",
  "weight": "10",
  "weight_units": "-3",
  "length": "2",
  "length_units": "-2",
  "width": "2",
  "width_units": "-2",
  "height": "2",
  "height_units": "-2",
  "surface": "2",
  "surface_units": "-4",
  "volume": "2",
  "volume_units": "-6",
  "status_buy": "1"
}
```
