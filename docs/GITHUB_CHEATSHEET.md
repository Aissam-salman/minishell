# 🚀 Cheat Sheet GitHub - Travail en Équipe avec Pull Requests

## 📋 Vue d'ensemble du projet
- **Branche principale** : `main`
- **Branches de développement** : `aiss`, `tim`
- **Objectif** : Merger les changements dans `main` via Pull Requests (comme en entreprise)

---

## 🌿 1. Gestion des branches

### Vérifier votre branche actuelle
```bash
git branch
```

### Changer de branche
```bash
# Passer à votre branche de travail
git checkout aiss    # ou git checkout tim

# Retourner à main
git checkout main
```

### Créer une nouvelle branche (si besoin)
```bash
# Depuis main
git checkout main
git pull origin main
git checkout -b nom-de-la-branche
```

### Mettre à jour votre branche avec les derniers changements de main
```bash
git checkout aiss    # Votre branche
git fetch origin
git merge origin/main    # Ou: git rebase origin/main
```

---

## 💾 2. Workflow de travail quotidien

### Travailler sur votre branche
```bash
# 1. Assurez-vous d'être sur votre branche
git checkout aiss

# 2. Faites vos modifications dans le code

# 3. Vérifier les fichiers modifiés
git status

# 4. Ajouter les fichiers modifiés
git add .                    # Ajouter tous les fichiers
git add fichier.c            # Ou fichier par fichier

# 5. Commiter avec un message clair
git commit -m "feat: ajout de la fonction de parsing"

# 6. Pousser vers GitHub
git push origin aiss
```

### Messages de commit professionnels
```bash
git commit -m "feat: nouvelle fonctionnalité"
git commit -m "fix: correction du bug de segfault"
git commit -m "refactor: réorganisation du code"
git commit -m "docs: mise à jour de la documentation"
git commit -m "test: ajout de tests unitaires"
git commit -m "style: formatage du code"
```

---

## 🔄 3. Pull Requests (PR) - Méthode Professionnelle

### Étape 1 : Préparer votre branche
```bash
# S'assurer que votre branche est à jour avec main
git checkout aiss
git fetch origin
git merge origin/main

# Résoudre les conflits si nécessaire
# Puis commit et push
git push origin aiss
```

### Étape 2 : Créer une Pull Request sur GitHub

1. **Aller sur GitHub** → Votre repository
2. Cliquer sur **"Pull requests"** → **"New pull request"**
3. **Sélectionner les branches** :
   - `base: main` ← `compare: aiss` (ou `tim`)
4. **Remplir la PR** :
   - **Titre** : Description courte et claire
   - **Description** : Expliquer ce qui a été fait
     ```markdown
     ## Changements
     - Ajout de la fonction de parsing
     - Correction des fuites mémoire
     - Mise à jour des tests
     
     ## Tests effectués
     - Testé avec valgrind
     - Tous les tests passent
     
     ## Notes
     - À revoir : la gestion des erreurs
     ```
5. Cliquer sur **"Create pull request"**

### Étape 3 : Code Review (Revue de code)
- **Reviewer** : L'autre personne examine le code
  - Commenter les lignes de code directement sur GitHub
  - Approuver ou demander des changements
- **Auteur** : Répond aux commentaires et fait les modifications
  ```bash
  # Faire les corrections sur votre branche
  git add .
  git commit -m "fix: corrections selon la review"
  git push origin aiss
  # La PR se met à jour automatiquement
  ```

### Étape 4 : Merger la Pull Request
Une fois approuvée :
1. Sur GitHub, dans la PR, cliquer sur **"Merge pull request"**
2. Choisir le type de merge :
   - **Create a merge commit** (recommandé pour débuter)
   - Squash and merge (combine tous les commits)
   - Rebase and merge (linéarise l'historique)
3. Confirmer le merge
4. **Supprimer la branche** (optionnel mais propre)

### Étape 5 : Mettre à jour votre repo local
```bash
git checkout main
git pull origin main

# Mettre à jour l'autre branche si besoin
git checkout aiss
git merge main
```

---

## 🤝 4. Workflow en équipe

### Synchronisation régulière
```bash
# Tous les matins, mettre à jour main
git checkout main
git pull origin main

# Puis mettre à jour votre branche
git checkout aiss
git merge main
```

### Si quelqu'un a mergé dans main
```bash
# Récupérer les changements
git fetch origin

# Intégrer dans votre branche
git checkout aiss
git merge origin/main

# Résoudre les conflits si nécessaire
# Puis push
git push origin aiss
```

---

## ⚠️ 5. Gestion des conflits

### Quand un conflit apparaît
```bash
git merge origin/main
# CONFLICT (content): Merge conflict in fichier.c
```

### Résoudre le conflit
1. **Ouvrir le fichier** en conflit :
   ```c
   <<<<<<< HEAD
   // Votre code
   =======
   // Code de l'autre branche
   >>>>>>> origin/main
   ```

2. **Éditer manuellement** pour garder le bon code

3. **Marquer comme résolu** :
   ```bash
   git add fichier.c
   git commit -m "merge: résolution des conflits avec main"
   git push origin aiss
   ```

---

## 🎯 6. Bonnes pratiques

### ✅ À FAIRE
- Commiter régulièrement (petits commits)
- Messages de commit clairs et en français/anglais
- Pull régulièrement de `main` pour éviter les gros conflits
- Tester son code avant de push
- Faire des PR petites et ciblées
- Reviewer le code de l'autre personne
- Communiquer avec l'équipe

### ❌ À ÉVITER
- Commiter directement sur `main` (toujours passer par PR)
- Messages de commit vagues ("fix", "update", "wip")
- Grosses PR avec trop de changements
- Ne jamais pull de `main`
- Push du code qui ne compile pas
- Ignorer les commentaires de review

---

## 📊 7. Commandes utiles

### Voir l'historique
```bash
git log --oneline --graph --all
```

### Annuler le dernier commit (non pushé)
```bash
git reset --soft HEAD~1    # Garde les modifications
git reset --hard HEAD~1    # Supprime les modifications
```

### Voir les différences
```bash
git diff                    # Changements non stagés
git diff --staged           # Changements stagés
git diff main..aiss         # Différence entre branches
```

### Nettoyer les branches locales
```bash
git fetch --prune           # Nettoyer les références
git branch -d nom-branche   # Supprimer une branche mergée
git branch -D nom-branche   # Forcer la suppression
```

### Stash (mettre de côté temporairement)
```bash
git stash                   # Mettre de côté
git stash list              # Voir les stash
git stash pop               # Récupérer le dernier stash
```

---

## 🔥 8. Workflow complet exemple

### Personne 1 (Aiss) :
```bash
# 1. Créer/aller sur sa branche
git checkout aiss

# 2. Coder la fonctionnalité
# ... modifications ...

# 3. Commit et push
git add .
git commit -m "feat: implémentation du lexer"
git push origin aiss

# 4. Créer une PR sur GitHub

# 5. Attendre la review de Tim

# 6. Faire les corrections si nécessaire
git add .
git commit -m "fix: corrections selon review"
git push origin aiss

# 7. Tim merge la PR sur GitHub

# 8. Mettre à jour local
git checkout main
git pull origin main
```

### Personne 2 (Tim) :
```bash
# 1. Review la PR de Aiss sur GitHub
# - Lire le code
# - Commenter
# - Approuver ou demander des changements

# 2. Merger la PR sur GitHub une fois approuvée

# 3. Travailler sur sa propre fonctionnalité
git checkout tim
git merge origin/main       # Intégrer les changements de Aiss

# 4. Coder, commit, push
git add .
git commit -m "feat: implémentation du parser"
git push origin tim

# 5. Créer sa propre PR

# 6. Aiss fait la review, puis merge
```

---

## 🆘 9. Aide rapide

### En cas de problème
```bash
# Voir l'état actuel
git status

# Annuler les modifications non commitées
git restore fichier.c

# Retourner à un état propre
git stash

# Voir les branches distantes
git branch -r

# Forcer la synchronisation avec origin
git fetch origin
git reset --hard origin/main    # ⚠️ Perte des modifications locales
```

### Ressources
- [GitHub Docs - Pull Requests](https://docs.github.com/en/pull-requests)
- [Git Documentation](https://git-scm.com/doc)
- `git help <commande>` pour l'aide détaillée

---

## 📝 Template de PR

```markdown
## Description
[Expliquer ce que fait cette PR]

## Type de changement
- [ ] Nouvelle fonctionnalité
- [ ] Correction de bug
- [ ] Refactoring
- [ ] Documentation

## Changements principaux
- 
- 
- 

## Tests effectués
- [ ] Compilation sans warnings
- [ ] Tests manuels
- [ ] Valgrind (pas de fuites)
- [ ] Norminette (si applicable)

## Checklist
- [ ] Mon code suit le style du projet
- [ ] J'ai commenté les parties complexes
- [ ] J'ai mis à jour la documentation
- [ ] Mes commits ont des messages clairs
- [ ] J'ai testé mon code

## Screenshots (si applicable)
[Ajouter des captures d'écran si nécessaire]

## Notes pour les reviewers
[Points spécifiques à vérifier]
```

---

**🎉 Bon travail en équipe !**
