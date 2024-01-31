# 2023-m2cns-rd-ChaineDeValeurs


## Contexte
Le but de ce projet est de développer une solution basée sur des dispositifs connectés pour améliorer la gestion et le recyclage des produits de santé jetables.

Les quantités de produits plastiques utilisés dans les activités de santé, tels que les seringues, les poches de perfusion et les gants chirurgicaux, ont atteint plus de 36 milliards d'euros dans le monde en 2020 et devraient atteindre 55 milliards d'euros d'ici 2025, selon le Conseil de Recyclage des Plastiques de Santé. La plupart de ces produits sont éliminés dans des décharges ou incinérés, même si une partie importante est non contaminée et donc recyclable. Les solutions de dispositifs connectés ont le potentiel d'impact sur la chaîne de valeur de ces produits en améliorant leur manipulation tout en garantissant la sécurité et l'efficacité des processus.

La première étape consiste à mener une étude sur les domaines où les technologies de l'Internet des Objets (IoT) pourraient améliorer la manipulation des produits liés à la santé. Cela inclut des aspects tels que la gestion des stocks, les conteneurs de déchets intelligents, la gestion des déchets et la surveillance des conditions de stérilisation des équipements. Ensuite, nous somme chargé de concevoir et de mettre en œuvre une solution concrète basée sur des dispositifs connectés qui répond à l'un des domaines identifiés.

![Schéma explicatif](lien_vers_image_schema.png)

Pour plus de détails, veuillez consulter le [rapport](./docs/report.pdf) et la [présentation](./docs/presentation.pdf).

## Installation et Configuration
[Expliquez comment installer et configurer votre solution technique. Vous pouvez faire référence à des readme.md spécifiques dans les sous-dossiers pour plus de détails.]

## Arduino

### Instructions d'installation du code Arduino

Le code fourni pour ce projet sera destiné à être utilisé sur trois cartes ESP32 distinctes, chacune remplissant un rôle spécifique dans le système de gestion des déchets de santé.

#### ESP1 - Gestion de la jetée des déchets
Ce code sera chargé sur la première carte ESP32 et sera responsable de la gestion de la jetée des déchets. Il s'occupera d'enclancher le mécanisme de jetée et de transmettre les informations nécessaires a la platform IoT.

#### ESP2 - Gestion de l'aiguillage
Sur la deuxième carte ESP32, le code sera chargé pour gérer l'aiguillage des déchets. Cette carte recevra les informations de la Platform IoT concernant le type de déchet jeté et dirigera ensuite le déchet vers le bon conteneur en fonction de sa catégorie.

#### ESP3 - Gestion de l'arrivée des déchets
Enfin, sur la troisième carte ESP32, le code sera chargé de gérer l'arrivée des déchets dans le réceptacle final. Cette carte s'assurera que les déchets sont correctement reçus et enregistrés dans le système.

### Configuration des branchements matériels

Les détails spécifiques sur la configuration des branchements matériels seront fournis dans le code Arduino de chaque ESP. Cependant, voici une vue d'ensemble générale des composants matériels nécessaires et de leur configuration typique :

- ESP1 : Actionneurs de jetée de déchets a base de servomoteurs.
- ESP2 : Aiguillage a l'aide de servomoteurs.
- ESP3 : Capteurs de détection Ultrason.

Le code de chaque ESP sera fourni de manière générique, ce qui permettra aux utilisateurs de l'adapter en fonction de leur propre configuration matérielle et de leurs besoins spécifiques.

### IoT Platform
- Instructions d'installation de la plateforme IoT.
- Configuration des fichiers Docker, flux.json, dashboard.json, etc.

### Machine Learning
- Instructions pour la mise en place de l'environnement ML.
- Configuration des données, des modèles ML, et des fonctions pour l'entraînement des modèles.

## Fonctionnement du Système
[Décrivez le fonctionnement du système et montrez un exemple. Utilisez des images des branchements, des captures d'écran de la plateforme, etc. Une vidéo de démonstration bien scénarisée peut aussi être incluse.]

## Tests et Analyses de Performance
[Décrivez les tests effectués sur le système, y compris les KPIs (Key Performance Indicators). Discutez de la robustesse, de la précision, des performances, etc.]
## Encadrant
- Hamidi Massinissa
## Auteurs
- Laib Ramy
- Saraoui Keltouma


