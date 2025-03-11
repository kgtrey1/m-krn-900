# Project description:

## Deadline

Le projet doit être rendu avant le 14 février à 23h59.
 
### Sujet du projet

#### Mise en contexte
- ~otp~ :: one time password
- Mise en pratique de votre apprentissage de dev kernel
- Réflexion, imagination pour trouver des solutions

#### Objectif
- Mettre en place un système (lkm + outillage) visant à offrir une solution ~otp~ via un ou plusieurs "device" et un ou plusieurs utilitaires.
- La solution doit mettre en oeuvre 2 méthodes d'otp:
    1. une liste de mot de passe
    2. un algo basé sur une clé et le temps.
- On doit envisager de créer plusieurs device pour gérer
  plusieurs otp.

#### Livrable
1. un module kernel (sources, makefile)
2. un ou plusieurs utilitaires (c ou script)
3. la documentation associée (peut être contenu dans les sources)

#### Fonctionnalités

##### lkm
- Créer un ou plusieurs devices
- Configurer le device
    1. clé / Durée de validité
    2. Listes des mots de passe 
- Permettre 
- Afficher / supprimer / modifier la clé, la durée, la liste

##### utilitaires
- gestion du module: 
  - Afficher / supprimer / modifier la clé, la durée, la liste (donc config du module)
- client otp:
  - calculer un mdp (coder l'algo ou la liste)
  - vérifier un mdp 

#### Exemple de validation
- Dans une fenêtre, on exécute cat /dev/otpN qui renvoie un otp
- Dans une autre fenêtre, on exécute un programme qui demande un otp et qui le valide

#### Critères de validation
- avoir un lkm et les utilitaires associés
- 2 méthodes
- Qualité du code
