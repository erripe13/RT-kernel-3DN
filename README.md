# RT-kernel-3DN
TP noyau temps réel 3DN ENSEA

## TP FreeRTOS
Chaque question est reliée à un commit du git.
### Objectif
L’objectif de ce TP sur cinq séances est de mettre en place quelques applications sous FreeRTOS en utilisant la carte NUCLEO-G431RB conçue autour du microcontrôleur STM32G431RBT6.

---

### Questions et réponses

#### 0. (Re)prise en main
1. Où se situe le fichier `main.c` ?  
   **Réponse : Le main se situe dans /core/source**

2. À quoi servent les commentaires indiquant `BEGIN` et `END` ?  
   **Réponse : Ces commentaires permettre connaitre les zones ou l'on peut écrire sans que cubeide ne le supprime lors de sa génération de code**

3. Quels sont les paramètres à passer à `HAL_Delay` et `HAL_GPIO_TogglePin` ?  
   **Réponse : HAL_Delay recoit un uint32_t et HAL_GPIO_TogglePin recoit le port et le pin du GPIO**

4. Dans quel fichier les ports d’entrée/sorties sont-ils définis ?  
   **Réponse : Le fichier .ioc permet de configurer les entrées sortie**

5. Écrivez un programme simple permettant de faire clignoter la LED.  
   **Commit : blink no rtos**

6. Modifiez le programme pour que la LED s’allume lorsque le bouton USER est appuyé.  
   **Commit : blink no rtos**

---

#### 1. FreeRTOS, tâches et sémaphores

##### 1.1 Tâche simple
1. Quels paramètres de FreeRTOS vous paraissent pertinents ? En quoi le paramètre `TOTAL_HEAP_SIZE` est-il important ?  
   **Réponse : MAX_PROPERTIES, MINIMAL_STACK_SIZE et TOTAL_HEAP_SIZE. TOTAL_HEAP_SIZE est crucial puisqu'il définit la mémoire dynamique(heap)**

2. Quel est le rôle de la macro `portTICK_PERIOD_MS` ?  
   **Réponse :La macro représente la durée d'un tick du système en ms.**

##### 1.2 Sémaphores pour la synchronisation
3. Expliquez le fonctionnement des tâches `taskGive` et `taskTake`.  
   **Réponse : taskGive donne un sémaphore. Elle utilise la fonction xSemaphoreGive() pour libérer le semaphore. taskTake à pour rôle de de prendre le sémaphore. Si le sémaphore n'est pas disponible la tache attend**

4. Ajoutez un mécanisme de gestion d’erreur lors de l’acquisition du sémaphore.  
   **Commit :  semaphore "watchdog"**

5. Expliquez les changements observés après modification des priorités.  
   **Réponse :En changeant les priorités on observe que si la priorité de la tache give est inferieure, alors la tache ne donne pas le sémaphore et le µc se reset. A contrario si la tache a une priorité égale à l'autre alors, le sémaphore est donné.**

##### 1.3 Notification
6. Modifiez le code pour utiliser des notifications de tâches à la place des sémaphores.  
   **Commit : task notifications + queue for delay value**

##### 1.4 Queues
7. Expliquez comment envoyer et recevoir des valeurs dans une queue.  
   **Réponse :etst**

##### 1.5 Réentrance et exclusion mutuelle
8. Expliquez le problème observé dans le code fourni et proposez une solution avec un sémaphore Mutex.  
   **Réponse : On ne peut pas utiliser 2 instances de printf en même temps sans la réentrance**

---

#### 2. On joue avec le Shell
1. Que se passe-t-il si l’on ne respecte pas les priorités décrites ?  
   **Réponse :On risque d'être plus prioritaire que les taches assignées au système**

2. Écrivez une fonction `led()` pour contrôler le clignotement de la LED.  
   **Réponse :Commit: building shell led command**

3. Écrivez une fonction `spam()` pour afficher du texte dans la liaison série.  
   **Réponse :Commit: added spam task functionnality to shell**

---

#### 3. Debug, gestion d’erreur et statistiques

##### 3.1 Gestion du tas
1. Quel est le nom de la zone réservée à l’allocation dynamique ?  
   **Réponse :La HEAP**

2. Est-ce géré par FreeRTOS ou la HAL ?  
   **Réponse :FreeRTOS**

3. Expliquez les relevés de mémoire RAM et Flash après modification de la taille du tas.  
   **Réponse :Au départ nous avions une utilisation de la RAM de 19kB et lors du dépassement de 15,360kB nous sommes en erreur car on a dépassé la taille de la HEAP. Le problème résolu après la modification de la taille de la HEAP**

##### 3.2 Gestion des piles
4. Expliquez l’intérêt des hooks dans FreeRTOS.  
   **Réponse :Les hooks sont des fonctions appelées automatiquement par FreeRTOS dans des situations spécifiques, comme l'inactivité (`IdleHook`), les dépassements de pile (`StackOverflowHook`), ou les échecs d'allocation mémoire (`MallocFailedHook`). Ils permettent de personnaliser le comportement du système en fonction des besoins.**

##### 3.3 Statistiques dans l’IDE
5. Comment afficher l’utilisation de la pile et du CPU dans STM32CubeIDE ?  
   **Réponse :**

##### 3.4 Affichage des statistiques dans le shell
6. Écrivez une fonction pour afficher les statistiques dans le terminal.  
   **Réponse :**

---

#### 4. Écriture d’un driver

##### 4.1 Interfacer l’ADXL345
1. Quelles sont les valeurs à mettre dans les registres `INT_ENABLE` et `POWER_CTL` pour démarrer la mesure ?  
   **Réponse :**

2. Expliquez comment lire et afficher les valeurs de l’accéléromètre.  
   **Réponse :**

##### 4.2 Driver SPI
3. Expliquez le fonctionnement des fonctions `drv_spi_init`, `drv_spi_write` et `drv_spi_read`.  
   **Réponse :**

4. Testez et validez le driver SPI.  
   **Réponse :**

---

**Note :** Complétez les réponses au fur et à mesure de votre progression dans le TP.

