# Documentation des Fonctions - Minishell

## Table des Matières
1. [Fonctions Readline](#fonctions-readline)
2. [Entrées/Sorties Standard](#entréessorties-standard)
3. [Gestion de la Mémoire](#gestion-de-la-mémoire)
4. [Gestion des Processus](#gestion-des-processus)
5. [Gestion des Signaux](#gestion-des-signaux)
6. [Système de Fichiers](#système-de-fichiers)
7. [Descripteurs de Fichiers et Pipes](#descripteurs-de-fichiers-et-pipes)
8. [Gestion des Répertoires](#gestion-des-répertoires)
9. [Gestion des Erreurs](#gestion-des-erreurs)
10. [Fonctions Terminal](#fonctions-terminal)
11. [Variables d'Environnement](#variables-denvironnement)
12. [Contrôle du Terminal](#contrôle-du-terminal)
13. [Fonctions Termcap](#fonctions-termcap)

---

## Fonctions Readline

### `readline`
```c
char *readline(const char *prompt);
```

**Description:** Lit une ligne depuis l'entrée standard avec édition de ligne et historique.

**Paramètres:**
- `prompt`: La chaîne à afficher comme invite de commande

**Retour:** Pointeur vers la ligne lue (doit être libéré avec `free`), ou `NULL` en cas d'EOF

**Exemple d'utilisation:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>

int main(void)
{
    char *line;

    while (1)
    {
        line = readline("minishell> ");
        if (!line)  // EOF (Ctrl+D)
            break;
        if (*line)  // Si la ligne n'est pas vide
            printf("Vous avez tapé: %s\n", line);
        free(line);
    }
    return (0);
}
```

---

### `rl_clear_history`
```c
void rl_clear_history(void);
```

**Description:** Efface l'historique complet des commandes.

**Paramètres:** Aucun

**Retour:** Aucun

**Exemple d'utilisation:**
```c
#include <readline/history.h>

void reset_shell_history(void)
{
    rl_clear_history();
    printf("Historique effacé\n");
}
```

---

### `rl_on_new_line`
```c
int rl_on_new_line(void);
```

**Description:** Indique à readline que le curseur est sur une nouvelle ligne.

**Paramètres:** Aucun

**Retour:** 0

**Exemple d'utilisation:**
```c
#include <readline/readline.h>

void handle_multiline_prompt(void)
{
    printf("\n");  // Nouvelle ligne
    rl_on_new_line();  // Informe readline
    rl_redisplay();    // Raffraîchit l'affichage
}
```

---

### `rl_replace_line`
```c
void rl_replace_line(const char *text, int clear_undo);
```

**Description:** Remplace le contenu du buffer de ligne actuel.

**Paramètres:**
- `text`: Le nouveau texte pour la ligne
- `clear_undo`: Si non-zéro, efface la liste d'annulation

**Retour:** Aucun

**Exemple d'utilisation:**
```c
#include <readline/readline.h>

void autocomplete_command(void)
{
    // Remplace la ligne par une commande complète
    rl_replace_line("ls -la /home", 0);
    rl_point = rl_end;  // Place le curseur à la fin
    rl_redisplay();
}
```

---

### `rl_redisplay`
```c
void rl_redisplay(void);
```

**Description:** Force readline à réafficher la ligne en cours.

**Paramètres:** Aucun

**Retour:** Aucun

**Exemple d'utilisation:**
```c
#include <readline/readline.h>

void refresh_prompt(void)
{
    rl_on_new_line();
    rl_redisplay();
}
```

---

### `add_history`
```c
void add_history(const char *line);
```

**Description:** Ajoute une ligne à l'historique des commandes.

**Paramètres:**
- `line`: La ligne à ajouter à l'historique

**Retour:** Aucun

**Exemple d'utilisation:**
```c
#include <readline/readline.h>
#include <readline/history.h>

int main(void)
{
    char *line;

    while (1)
    {
        line = readline("minishell> ");
        if (!line)
            break;
        if (*line)  // N'ajoute que les lignes non vides
            add_history(line);
        // Traiter la commande...
        free(line);
    }
    return (0);
}
```

---

## Entrées/Sorties Standard

### `printf`
```c
int printf(const char *format, ...);
```

**Description:** Écrit une sortie formatée sur stdout.

**Paramètres:**
- `format`: Chaîne de format
- `...`: Arguments variables selon le format

**Retour:** Nombre de caractères écrits, ou valeur négative en cas d'erreur

**Exemple d'utilisation:**
```c
#include <stdio.h>

void display_error(char *cmd, int error_code)
{
    printf("Erreur dans '%s': code %d\n", cmd, error_code);
}
```

---

### `write`
```c
ssize_t write(int fd, const void *buf, size_t count);
```

**Description:** Écrit des données dans un descripteur de fichier.

**Paramètres:**
- `fd`: Descripteur de fichier
- `buf`: Buffer contenant les données à écrire
- `count`: Nombre d'octets à écrire

**Retour:** Nombre d'octets écrits, ou -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <unistd.h>
#include <string.h>

void write_error(char *msg)
{
    write(STDERR_FILENO, msg, strlen(msg));
    write(STDERR_FILENO, "\n", 1);
}

// Exemple: Redirection de sortie
void write_to_file(void)
{
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
        return;
    write(fd, "Hello, World!\n", 14);
    close(fd);
}
```

---

### `read`
```c
ssize_t read(int fd, void *buf, size_t count);
```

**Description:** Lit des données depuis un descripteur de fichier.

**Paramètres:**
- `fd`: Descripteur de fichier
- `buf`: Buffer où stocker les données lues
- `count`: Nombre maximum d'octets à lire

**Retour:** Nombre d'octets lus, 0 pour EOF, -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <unistd.h>
#include <fcntl.h>

void read_from_file(char *filename)
{
    int fd;
    char buffer[1024];
    ssize_t bytes_read;

    fd = open(filename, O_RDONLY);
    if (fd == -1)
        return;

    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytes_read] = '\0';
        write(STDOUT_FILENO, buffer, bytes_read);
    }
    close(fd);
}
```

---

## Gestion de la Mémoire

### `malloc`
```c
void *malloc(size_t size);
```

**Description:** Alloue un bloc de mémoire de taille spécifiée.

**Paramètres:**
- `size`: Taille en octets à allouer

**Retour:** Pointeur vers la mémoire allouée, ou `NULL` si échec

**Exemple d'utilisation:**
```c
#include <stdlib.h>
#include <string.h>

char **allocate_args(int count)
{
    char **args = malloc(sizeof(char *) * (count + 1));
    if (!args)
        return (NULL);
    args[count] = NULL;  // Terminateur
    return (args);
}
```

---

### `free`
```c
void free(void *ptr);
```

**Description:** Libère un bloc de mémoire précédemment alloué.

**Paramètres:**
- `ptr`: Pointeur vers la mémoire à libérer

**Retour:** Aucun

**Exemple d'utilisation:**
```c
#include <stdlib.h>

void free_array(char **array)
{
    int i = 0;

    if (!array)
        return;
    while (array[i])
    {
        free(array[i]);
        i++;
    }
    free(array);
}
```

---

## Gestion des Processus

### `fork`
```c
pid_t fork(void);
```

**Description:** Crée un nouveau processus en dupliquant le processus appelant.

**Paramètres:** Aucun

**Retour:**
- Dans le parent: PID du processus enfant
- Dans l'enfant: 0
- En cas d'erreur: -1

**Exemple d'utilisation:**
```c
#include <unistd.h>
#include <sys/wait.h>

void execute_command(char **args)
{
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        return;
    }
    else if (pid == 0)  // Processus enfant
    {
        execve(args[0], args, NULL);
        perror("execve");
        exit(1);
    }
    else  // Processus parent
    {
        waitpid(pid, NULL, 0);
    }
}
```

---

### `wait`
```c
pid_t wait(int *status);
```

**Description:** Attend qu'un processus enfant se termine.

**Paramètres:**
- `status`: Pointeur où stocker le statut de sortie (peut être NULL)

**Retour:** PID du processus terminé, ou -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <sys/wait.h>
#include <unistd.h>

void wait_for_child(void)
{
    int status;
    pid_t child_pid;

    child_pid = wait(&status);
    if (child_pid == -1)
    {
        perror("wait");
        return;
    }
    if (WIFEXITED(status))
        printf("Enfant %d terminé avec code %d\n",
               child_pid, WEXITSTATUS(status));
}
```

---

### `waitpid`
```c
pid_t waitpid(pid_t pid, int *status, int options);
```

**Description:** Attend qu'un processus enfant spécifique change d'état.

**Paramètres:**
- `pid`: PID du processus à attendre (-1 pour n'importe quel enfant)
- `status`: Pointeur où stocker le statut
- `options`: Options (WNOHANG, WUNTRACED, etc.)

**Retour:** PID du processus, 0 (si WNOHANG), ou -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <sys/wait.h>
#include <unistd.h>

int execute_and_wait(char **args)
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        execve(args[0], args, environ);
        exit(1);
    }

    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        return (WEXITSTATUS(status));
    return (-1);
}
```

---

### `wait3`
```c
pid_t wait3(int *status, int options, struct rusage *rusage);
```

**Description:** Attend un enfant et récupère les statistiques d'utilisation des ressources.

**Paramètres:**
- `status`: Pointeur vers le statut de sortie
- `options`: Options d'attente
- `rusage`: Structure pour les statistiques d'utilisation (peut être NULL)

**Retour:** PID du processus, ou -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <sys/wait.h>
#include <sys/resource.h>

void wait_with_stats(void)
{
    int status;
    struct rusage usage;
    pid_t pid;

    pid = wait3(&status, 0, &usage);
    if (pid != -1)
    {
        printf("CPU user time: %ld.%06ld s\n",
               usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
        printf("CPU system time: %ld.%06ld s\n",
               usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
    }
}
```

---

### `wait4`
```c
pid_t wait4(pid_t pid, int *status, int options, struct rusage *rusage);
```

**Description:** Comme wait3 mais attend un processus spécifique.

**Paramètres:**
- `pid`: PID du processus à attendre
- `status`: Pointeur vers le statut
- `options`: Options d'attente
- `rusage`: Structure pour les statistiques

**Retour:** PID du processus, ou -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <sys/wait.h>
#include <sys/resource.h>

void wait_specific_process(pid_t child_pid)
{
    int status;
    struct rusage usage;

    if (wait4(child_pid, &status, 0, &usage) != -1)
    {
        printf("Process %d utilisé %ld Ko de mémoire\n",
               child_pid, usage.ru_maxrss);
    }
}
```

---

### `exit`
```c
void exit(int status);
```

**Description:** Termine le processus avec un code de sortie.

**Paramètres:**
- `status`: Code de sortie (0 = succès, autre = erreur)

**Retour:** Ne retourne pas

**Exemple d'utilisation:**
```c
#include <stdlib.h>

void cleanup_and_exit(int code)
{
    // Nettoyage des ressources
    rl_clear_history();
    // ...
    exit(code);
}

// Dans un processus enfant
if (fork() == 0)
{
    execve("/bin/ls", args, environ);
    perror("execve failed");
    exit(127);  // Code d'erreur si execve échoue
}
```

---

### `execve`
```c
int execve(const char *pathname, char *const argv[], char *const envp[]);
```

**Description:** Exécute un programme en remplaçant le processus actuel.

**Paramètres:**
- `pathname`: Chemin du programme à exécuter
- `argv`: Tableau d'arguments (terminé par NULL)
- `envp`: Tableau de variables d'environnement (terminé par NULL)

**Retour:** Ne retourne qu'en cas d'erreur (-1)

**Exemple d'utilisation:**
```c
#include <unistd.h>

void execute_ls(char *path)
{
    char *args[] = {"/bin/ls", "-la", path, NULL};
    char *env[] = {"PATH=/bin:/usr/bin", NULL};

    if (fork() == 0)
    {
        execve("/bin/ls", args, env);
        perror("execve");
        exit(1);
    }
}
```

---

## Gestion des Signaux

### `signal`
```c
sighandler_t signal(int signum, sighandler_t handler);
```

**Description:** Définit un gestionnaire pour un signal.

**Paramètres:**
- `signum`: Numéro du signal (SIGINT, SIGQUIT, etc.)
- `handler`: Fonction de traitement ou SIG_IGN/SIG_DFL

**Retour:** Ancien gestionnaire, ou SIG_ERR en cas d'erreur

**Exemple d'utilisation:**
```c
#include <signal.h>
#include <stdio.h>

void sigint_handler(int sig)
{
    printf("\nCtrl+C intercepté (signal %d)\n", sig);
    // Ne pas appeler exit() ici dans un shell
}

void setup_signals(void)
{
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, SIG_IGN);  // Ignore Ctrl+backslash
}
```

---

### `sigaction`
```c
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
```

**Description:** Examine ou modifie l'action associée à un signal (plus robuste que signal).

**Paramètres:**
- `signum`: Numéro du signal
- `act`: Nouvelle action (NULL pour ne pas changer)
- `oldact`: Sauvegarde de l'ancienne action (NULL si non nécessaire)

**Retour:** 0 en cas de succès, -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <signal.h>
#include <stdio.h>

void sigint_handler(int sig)
{
    write(STDOUT_FILENO, "\n", 1);
}

void setup_sigaction(void)
{
    struct sigaction sa;

    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;  // Redémarre les appels système interrompus

    sigaction(SIGINT, &sa, NULL);
}
```

---

### `sigemptyset`
```c
int sigemptyset(sigset_t *set);
```

**Description:** Initialise un ensemble de signaux vide.

**Paramètres:**
- `set`: Pointeur vers l'ensemble de signaux

**Retour:** 0 en cas de succès, -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <signal.h>

void block_signals(void)
{
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);
    sigprocmask(SIG_BLOCK, &set, NULL);
}
```

---

### `sigaddset`
```c
int sigaddset(sigset_t *set, int signum);
```

**Description:** Ajoute un signal à un ensemble de signaux.

**Paramètres:**
- `set`: Pointeur vers l'ensemble de signaux
- `signum`: Numéro du signal à ajouter

**Retour:** 0 en cas de succès, -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <signal.h>

void create_signal_set(sigset_t *set)
{
    sigemptyset(set);
    sigaddset(set, SIGINT);
    sigaddset(set, SIGTERM);
    sigaddset(set, SIGQUIT);
}
```

---

### `kill`
```c
int kill(pid_t pid, int sig);
```

**Description:** Envoie un signal à un processus ou un groupe de processus.

**Paramètres:**
- `pid`: PID du processus (ou 0, -1 pour groupes)
- `sig`: Numéro du signal à envoyer

**Retour:** 0 en cas de succès, -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <signal.h>
#include <unistd.h>

void terminate_child(pid_t child_pid)
{
    // Essaie d'abord une terminaison propre
    kill(child_pid, SIGTERM);
    sleep(1);

    // Force la terminaison si nécessaire
    kill(child_pid, SIGKILL);
}

// Vérifier si un processus existe
int process_exists(pid_t pid)
{
    return (kill(pid, 0) == 0);
}
```

---

## Système de Fichiers

### `access`
```c
int access(const char *pathname, int mode);
```

**Description:** Vérifie les permissions d'accès à un fichier.

**Paramètres:**
- `pathname`: Chemin du fichier
- `mode`: Mode de test (R_OK, W_OK, X_OK, F_OK)

**Retour:** 0 si l'accès est autorisé, -1 sinon

**Exemple d'utilisation:**
```c
#include <unistd.h>
#include <stdio.h>

int check_executable(char *path)
{
    if (access(path, F_OK) == -1)
    {
        printf("%s: fichier introuvable\n", path);
        return (0);
    }
    if (access(path, X_OK) == -1)
    {
        printf("%s: permission refusée\n", path);
        return (0);
    }
    return (1);
}
```

---

### `open`
```c
int open(const char *pathname, int flags, mode_t mode);
```

**Description:** Ouvre un fichier et retourne un descripteur de fichier.

**Paramètres:**
- `pathname`: Chemin du fichier
- `flags`: Flags d'ouverture (O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, etc.)
- `mode`: Permissions si création (ex: 0644)

**Retour:** Descripteur de fichier, ou -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <fcntl.h>
#include <unistd.h>

void redirect_output(char *filename)
{
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("open");
        return;
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);
}

// Ouvrir en mode append
void append_to_file(char *filename, char *content)
{
    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd != -1)
    {
        write(fd, content, strlen(content));
        close(fd);
    }
}
```

---

### `close`
```c
int close(int fd);
```

**Description:** Ferme un descripteur de fichier.

**Paramètres:**
- `fd`: Descripteur de fichier à fermer

**Retour:** 0 en cas de succès, -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <unistd.h>
#include <fcntl.h>

void safe_close(int fd)
{
    if (fd > 2)  // Ne ferme pas stdin/stdout/stderr
    {
        if (close(fd) == -1)
            perror("close");
    }
}
```

---

### `getcwd`
```c
char *getcwd(char *buf, size_t size);
```

**Description:** Obtient le répertoire de travail courant.

**Paramètres:**
- `buf`: Buffer où stocker le chemin (ou NULL pour allocation automatique)
- `size`: Taille du buffer

**Retour:** Pointeur vers le chemin, ou NULL en cas d'erreur

**Exemple d'utilisation:**
```c
#include <unistd.h>
#include <stdio.h>

void print_current_dir(void)
{
    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("Répertoire actuel: %s\n", cwd);
    else
        perror("getcwd");
}

// Avec allocation automatique
char *get_working_directory(void)
{
    return (getcwd(NULL, 0));  // À libérer avec free()
}
```

---

### `chdir`
```c
int chdir(const char *path);
```

**Description:** Change le répertoire de travail courant.

**Paramètres:**
- `path`: Chemin du nouveau répertoire

**Retour:** 0 en cas de succès, -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <unistd.h>
#include <stdio.h>

int builtin_cd(char *path)
{
    if (!path || !*path)
        path = getenv("HOME");

    if (chdir(path) == -1)
    {
        perror("cd");
        return (1);
    }
    return (0);
}
```

---

### `stat`
```c
int stat(const char *pathname, struct stat *statbuf);
```

**Description:** Obtient les informations sur un fichier.

**Paramètres:**
- `pathname`: Chemin du fichier
- `statbuf`: Structure où stocker les informations

**Retour:** 0 en cas de succès, -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <sys/stat.h>
#include <stdio.h>

void display_file_info(char *filename)
{
    struct stat st;

    if (stat(filename, &st) == -1)
    {
        perror("stat");
        return;
    }

    printf("Taille: %ld octets\n", st.st_size);
    printf("Permissions: %o\n", st.st_mode & 0777);

    if (S_ISDIR(st.st_mode))
        printf("Type: répertoire\n");
    else if (S_ISREG(st.st_mode))
        printf("Type: fichier régulier\n");
}
```

---

### `lstat`
```c
int lstat(const char *pathname, struct stat *statbuf);
```

**Description:** Comme stat mais ne suit pas les liens symboliques.

**Paramètres:**
- `pathname`: Chemin du fichier
- `statbuf`: Structure où stocker les informations

**Retour:** 0 en cas de succès, -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <sys/stat.h>

int is_symlink(char *path)
{
    struct stat st;

    if (lstat(path, &st) == -1)
        return (0);
    return (S_ISLNK(st.st_mode));
}
```

---

### `fstat`
```c
int fstat(int fd, struct stat *statbuf);
```

**Description:** Obtient les informations sur un fichier via son descripteur.

**Paramètres:**
- `fd`: Descripteur de fichier
- `statbuf`: Structure où stocker les informations

**Retour:** 0 en cas de succès, -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <sys/stat.h>
#include <fcntl.h>

size_t get_file_size(int fd)
{
    struct stat st;

    if (fstat(fd, &st) == -1)
        return (0);
    return (st.st_size);
}
```

---

### `unlink`
```c
int unlink(const char *pathname);
```

**Description:** Supprime un nom de fichier et éventuellement le fichier lui-même.

**Paramètres:**
- `pathname`: Chemin du fichier à supprimer

**Retour:** 0 en cas de succès, -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <unistd.h>
#include <stdio.h>

void delete_temp_file(char *filename)
{
    if (unlink(filename) == -1)
        perror("unlink");
    else
        printf("Fichier %s supprimé\n", filename);
}
```

---

## Descripteurs de Fichiers et Pipes

### `dup`
```c
int dup(int oldfd);
```

**Description:** Duplique un descripteur de fichier.

**Paramètres:**
- `oldfd`: Descripteur de fichier à dupliquer

**Retour:** Nouveau descripteur, ou -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <unistd.h>

void save_stdout(void)
{
    int saved_stdout = dup(STDOUT_FILENO);
    // Plus tard: restaurer avec dup2(saved_stdout, STDOUT_FILENO)
}
```

---

### `dup2`
```c
int dup2(int oldfd, int newfd);
```

**Description:** Duplique un descripteur de fichier vers un descripteur spécifique.

**Paramètres:**
- `oldfd`: Descripteur source
- `newfd`: Descripteur destination

**Retour:** Nouveau descripteur, ou -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <unistd.h>
#include <fcntl.h>

void redirect_to_file(char *filename)
{
    int fd;
    int saved_stdout;

    // Sauvegarder stdout
    saved_stdout = dup(STDOUT_FILENO);

    // Rediriger vers fichier
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(fd, STDOUT_FILENO);
    close(fd);

    printf("Ceci va dans le fichier\n");

    // Restaurer stdout
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);

    printf("Ceci va sur la console\n");
}
```

---

### `pipe`
```c
int pipe(int pipefd[2]);
```

**Description:** Crée un tube de communication (pipe).

**Paramètres:**
- `pipefd`: Tableau de 2 int pour les descripteurs (pipefd[0] = lecture, pipefd[1] = écriture)

**Retour:** 0 en cas de succès, -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <unistd.h>
#include <string.h>

void execute_pipe(char **cmd1, char **cmd2)
{
    int pipefd[2];
    pid_t pid1, pid2;

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return;
    }

    pid1 = fork();
    if (pid1 == 0)  // Premier enfant
    {
        close(pipefd[0]);  // Ferme lecture
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        execve(cmd1[0], cmd1, environ);
        exit(1);
    }

    pid2 = fork();
    if (pid2 == 0)  // Deuxième enfant
    {
        close(pipefd[1]);  // Ferme écriture
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        execve(cmd2[0], cmd2, environ);
        exit(1);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}
```

---

## Gestion des Répertoires

### `opendir`
```c
DIR *opendir(const char *name);
```

**Description:** Ouvre un flux de répertoire.

**Paramètres:**
- `name`: Chemin du répertoire

**Retour:** Pointeur DIR, ou NULL en cas d'erreur

**Exemple d'utilisation:**
```c
#include <dirent.h>
#include <stdio.h>

void list_directory(char *path)
{
    DIR *dir = opendir(path);
    struct dirent *entry;

    if (!dir)
    {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL)
        printf("%s\n", entry->d_name);

    closedir(dir);
}
```

---

### `readdir`
```c
struct dirent *readdir(DIR *dirp);
```

**Description:** Lit une entrée du répertoire.

**Paramètres:**
- `dirp`: Pointeur DIR retourné par opendir

**Retour:** Pointeur vers structure dirent, ou NULL à la fin ou en cas d'erreur

**Exemple d'utilisation:**
```c
#include <dirent.h>
#include <sys/stat.h>

void list_files_only(char *path)
{
    DIR *dir = opendir(path);
    struct dirent *entry;
    struct stat st;
    char fullpath[1024];

    if (!dir)
        return;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.')  // Ignore fichiers cachés
            continue;

        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
        if (stat(fullpath, &st) == 0 && S_ISREG(st.st_mode))
            printf("%s\n", entry->d_name);
    }

    closedir(dir);
}
```

---

### `closedir`
```c
int closedir(DIR *dirp);
```

**Description:** Ferme un flux de répertoire.

**Paramètres:**
- `dirp`: Pointeur DIR à fermer

**Retour:** 0 en cas de succès, -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <dirent.h>

void safe_closedir(DIR *dir)
{
    if (dir)
    {
        if (closedir(dir) == -1)
            perror("closedir");
    }
}
```

---

## Gestion des Erreurs

### `strerror`
```c
char *strerror(int errnum);
```

**Description:** Retourne une chaîne décrivant le code d'erreur.

**Paramètres:**
- `errnum`: Numéro d'erreur (généralement errno)

**Retour:** Pointeur vers la chaîne d'erreur

**Exemple d'utilisation:**
```c
#include <string.h>
#include <errno.h>
#include <stdio.h>

void print_error(char *prefix)
{
    printf("%s: %s\n", prefix, strerror(errno));
}

// Utilisation après échec
if (open("file.txt", O_RDONLY) == -1)
    printf("Erreur: %s\n", strerror(errno));
```

---

### `perror`
```c
void perror(const char *s);
```

**Description:** Affiche un message d'erreur système sur stderr.

**Paramètres:**
- `s`: Préfixe du message (peut être NULL)

**Retour:** Aucun

**Exemple d'utilisation:**
```c
#include <stdio.h>
#include <fcntl.h>

void try_open_file(char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("minishell");  // Affiche: "minishell: No such file or directory"
        return;
    }
    close(fd);
}
```

---

## Fonctions Terminal

### `isatty`
```c
int isatty(int fd);
```

**Description:** Teste si un descripteur de fichier correspond à un terminal.

**Paramètres:**
- `fd`: Descripteur de fichier à tester

**Retour:** 1 si c'est un terminal, 0 sinon

**Exemple d'utilisation:**
```c
#include <unistd.h>
#include <stdio.h>

void check_interactive(void)
{
    if (isatty(STDIN_FILENO))
        printf("Mode interactif\n");
    else
        printf("Mode non-interactif (pipe ou redirection)\n");
}
```

---

### `ttyname`
```c
char *ttyname(int fd);
```

**Description:** Retourne le nom du terminal associé au descripteur.

**Paramètres:**
- `fd`: Descripteur de fichier

**Retour:** Pointeur vers le nom du terminal, ou NULL en cas d'erreur

**Exemple d'utilisation:**
```c
#include <unistd.h>
#include <stdio.h>

void print_terminal_name(void)
{
    char *name = ttyname(STDIN_FILENO);
    if (name)
        printf("Terminal: %s\n", name);
    else
        printf("Pas un terminal\n");
}
```

---

### `ttyslot`
```c
int ttyslot(void);
```

**Description:** Trouve le slot du terminal dans le fichier utmp.

**Paramètres:** Aucun

**Retour:** Numéro de slot, ou 0 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <unistd.h>
#include <stdio.h>

void show_tty_slot(void)
{
    int slot = ttyslot();
    if (slot > 0)
        printf("TTY slot: %d\n", slot);
}
```

---

### `ioctl`
```c
int ioctl(int fd, unsigned long request, ...);
```

**Description:** Contrôle les périphériques (opérations diverses).

**Paramètres:**
- `fd`: Descripteur de fichier
- `request`: Code de requête
- `...`: Arguments optionnels selon la requête

**Retour:** 0 en cas de succès, -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>

void get_terminal_size(void)
{
    struct winsize w;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
    {
        printf("Lignes: %d, Colonnes: %d\n", w.ws_row, w.ws_col);
    }
}
```

---

## Variables d'Environnement

### `getenv`
```c
char *getenv(const char *name);
```

**Description:** Obtient la valeur d'une variable d'environnement.

**Paramètres:**
- `name`: Nom de la variable

**Retour:** Pointeur vers la valeur, ou NULL si non trouvée

**Exemple d'utilisation:**
```c
#include <stdlib.h>
#include <stdio.h>

void show_path(void)
{
    char *path = getenv("PATH");
    if (path)
        printf("PATH=%s\n", path);
    else
        printf("PATH non défini\n");
}

char *get_home(void)
{
    char *home = getenv("HOME");
    return (home ? home : "/");
}
```

---

## Contrôle du Terminal

### `tcsetattr`
```c
int tcsetattr(int fd, int optional_actions, const struct termios *termios_p);
```

**Description:** Définit les paramètres du terminal.

**Paramètres:**
- `fd`: Descripteur du terminal
- `optional_actions`: Quand appliquer (TCSANOW, TCSADRAIN, TCSAFLUSH)
- `termios_p`: Structure contenant les nouveaux paramètres

**Retour:** 0 en cas de succès, -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <termios.h>
#include <unistd.h>

void disable_echo(void)
{
    struct termios term;

    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~ECHO;  // Désactive l'écho
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void enable_echo(void)
{
    struct termios term;

    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ECHO;  // Active l'écho
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}
```

---

### `tcgetattr`
```c
int tcgetattr(int fd, struct termios *termios_p);
```

**Description:** Obtient les paramètres du terminal.

**Paramètres:**
- `fd`: Descripteur du terminal
- `termios_p`: Structure où stocker les paramètres

**Retour:** 0 en cas de succès, -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <termios.h>
#include <unistd.h>

struct termios save_terminal_settings(void)
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    return (term);
}

void restore_terminal_settings(struct termios term)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}
```

---

## Fonctions Termcap

### `tgetent`
```c
int tgetent(char *bp, const char *name);
```

**Description:** Charge l'entrée termcap pour un terminal.

**Paramètres:**
- `bp`: Buffer (peut être NULL avec ncurses)
- `name`: Nom du type de terminal

**Retour:** 1 si trouvé, 0 si non trouvé, -1 en cas d'erreur

**Exemple d'utilisation:**
```c
#include <term.h>
#include <stdlib.h>

int init_termcap(void)
{
    char *term = getenv("TERM");

    if (!term)
        return (0);

    if (tgetent(NULL, term) <= 0)
    {
        printf("Terminal type '%s' inconnu\n", term);
        return (0);
    }
    return (1);
}
```

---

### `tgetflag`
```c
int tgetflag(char *id);
```

**Description:** Obtient une capacité booléenne du terminal.

**Paramètres:**
- `id`: Code de la capacité (2 caractères)

**Retour:** 1 si présent, 0 si absent

**Exemple d'utilisation:**
```c
#include <term.h>

void check_capabilities(void)
{
    if (tgetflag("am"))  // Auto margin
        printf("Le terminal a des marges automatiques\n");

    if (tgetflag("xn"))  // Newline glitch
        printf("Le terminal a un bug de nouvelle ligne\n");
}
```

---

### `tgetnum`
```c
int tgetnum(char *id);
```

**Description:** Obtient une capacité numérique du terminal.

**Paramètres:**
- `id`: Code de la capacité (2 caractères)

**Retour:** Valeur numérique, ou -1 si absent

**Exemple d'utilisation:**
```c
#include <term.h>
#include <stdio.h>

void show_terminal_dimensions(void)
{
    int cols = tgetnum("co");  // Colonnes
    int lines = tgetnum("li");  // Lignes

    if (cols > 0 && lines > 0)
        printf("Terminal: %d colonnes x %d lignes\n", cols, lines);
}
```

---

### `tgetstr`
```c
char *tgetstr(char *id, char **area);
```

**Description:** Obtient une capacité de type chaîne du terminal.

**Paramètres:**
- `id`: Code de la capacité (2 caractères)
- `area`: Pointeur vers buffer (peut être NULL)

**Retour:** Pointeur vers la chaîne, ou NULL si absent

**Exemple d'utilisation:**
```c
#include <term.h>

void get_terminal_strings(void)
{
    char *clear_screen = tgetstr("cl", NULL);  // Clear screen
    char *move_cursor = tgetstr("cm", NULL);   // Cursor motion
    char *bold = tgetstr("md", NULL);          // Bold mode

    // Utiliser ces chaînes avec tputs()
}
```

---

### `tgoto`
```c
char *tgoto(const char *cap, int col, int row);
```

**Description:** Prépare une chaîne de positionnement du curseur.

**Paramètres:**
- `cap`: Capacité de mouvement (de tgetstr)
- `col`: Colonne destination
- `row`: Ligne destination

**Retour:** Chaîne formatée pour tputs

**Exemple d'utilisation:**
```c
#include <term.h>

int putchar_wrapper(int c)
{
    return (write(STDOUT_FILENO, &c, 1));
}

void move_cursor_to(int row, int col)
{
    char *cm = tgetstr("cm", NULL);
    char *result;

    if (cm)
    {
        result = tgoto(cm, col, row);
        tputs(result, 1, putchar_wrapper);
    }
}
```

---

### `tputs`
```c
int tputs(const char *str, int affcnt, int (*putc)(int));
```

**Description:** Envoie une chaîne de contrôle au terminal.

**Paramètres:**
- `str`: Chaîne à envoyer
- `affcnt`: Nombre de lignes affectées
- `putc`: Fonction pour afficher un caractère

**Retour:** Code retour de putc, ou ERR en cas d'erreur

**Exemple d'utilisation:**
```c
#include <term.h>
#include <unistd.h>

int ft_putchar(int c)
{
    return (write(STDOUT_FILENO, &c, 1));
}

void clear_screen(void)
{
    char *cl = tgetstr("cl", NULL);  // Clear screen capability

    if (cl)
        tputs(cl, 1, ft_putchar);
}

void set_bold(void)
{
    char *bold = tgetstr("md", NULL);
    if (bold)
        tputs(bold, 1, ft_putchar);
}

void reset_attributes(void)
{
    char *me = tgetstr("me", NULL);  // Exit attribute mode
    if (me)
        tputs(me, 1, ft_putchar);
}
```

---

## Exemples Complets

### Exemple 1: Shell Basique avec Readline
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>

extern char **environ;

void execute_command(char **args)
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        execve(args[0], args, environ);
        perror("execve");
        exit(127);
    }
    else if (pid > 0)
        waitpid(pid, &status, 0);
    else
        perror("fork");
}

int main(void)
{
    char *line;
    char *args[2];

    while (1)
    {
        line = readline("minishell> ");
        if (!line)
            break;

        if (*line)
        {
            add_history(line);
            args[0] = line;
            args[1] = NULL;
            execute_command(args);
        }
        free(line);
    }
    rl_clear_history();
    return (0);
}
```

### Exemple 2: Pipeline avec Fork et Pipe
```c
#include <unistd.h>
#include <sys/wait.h>

void execute_pipeline(char **cmd1, char **cmd2)
{
    int pipefd[2];
    pid_t pid1, pid2;

    pipe(pipefd);

    pid1 = fork();
    if (pid1 == 0)
    {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        execve(cmd1[0], cmd1, NULL);
        exit(1);
    }

    pid2 = fork();
    if (pid2 == 0)
    {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        execve(cmd2[0], cmd2, NULL);
        exit(1);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}
```

### Exemple 3: Gestion des Signaux
```c
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void sigint_handler(int sig)
{
    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

void setup_signal_handlers(void)
{
    struct sigaction sa;

    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGINT, &sa, NULL);
    signal(SIGQUIT, SIG_IGN);
}
```

---

## Notes Importantes

### Sécurité et Bonnes Pratiques
- Toujours vérifier les valeurs de retour des fonctions système
- Libérer la mémoire allouée avec `malloc`/`readline`
- Fermer tous les descripteurs de fichiers ouverts
- Gérer correctement les signaux dans un shell interactif
- Utiliser `access` avant `execve` pour vérifier les permissions

### Gestion des Erreurs
- Utiliser `perror` ou `strerror(errno)` pour afficher les erreurs
- Vérifier `errno` après les appels système qui échouent
- Retourner des codes d'erreur appropriés (0 = succès, autre = erreur)

### Performance
- Préférer `write` à `printf` pour les messages d'erreur
- Utiliser `dup2` pour les redirections plutôt que de fermer/ouvrir
- Fermer les descripteurs de fichiers inutilisés dans les processus enfants

### Portabilité
- Certaines fonctions peuvent avoir des comportements différents selon les systèmes
- Tester `isatty` pour savoir si le shell est interactif
- Utiliser `getenv("TERM")` pour les fonctionnalités termcap

---

## Compilation

Pour compiler avec readline:
```bash
gcc -o minishell main.c -lreadline
```

Pour compiler avec termcap:
```bash
gcc -o program main.c -ltermcap
```

Ou avec ncurses (plus moderne):
```bash
gcc -o program main.c -lncurses
```

---

**Auteur:** Documentation pour le projet Minishell (42)
**Date:** Février 2026
