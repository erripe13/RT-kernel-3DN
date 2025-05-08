## TP FreeRTOS

La plupart des questions sont liées à un commit du repo, dans la mesure du possible.

L'essentiel des commits détaillés sont sur la branche "pierre" mais il serait erroné de croire que l'individu du même nom a été le seul à travailler. https://github.com/erripe13/RT-kernel-3DN/commits/pierre

Note : à la fin du projet la branche a été mergée dans le main en même temps que l'ajout de la fonctionnalité de statistiques dans le shell.

### Objectif

L’objectif de ce TP sur cinq séances est de mettre en place quelques applications sous FreeRTOS en utilisant la carte STM32F746G-DISCO conçue autour du microcontrôleur STM32F746G.

---

### 0. (Re)prise en main

#### 0.1 Premiers pas

1. Où se situe le fichier `main.c` ?  
   **Réponse : Le fichier `main.c` se situe dans `/Core/Src`.**

2. À quoi servent les commentaires `/* USER CODE BEGIN */` et `/* USER CODE END */` ?  
   **Réponse : Ces commentaires indiquent les zones où l'on peut écrire du code sans que CubeMX ne les écrase lors de la régénération.**

3. Quels sont les paramètres à passer à `HAL_Delay()` et `HAL_GPIO_TogglePin()` ?  
   **Réponse : `HAL_Delay()` prend un `uint32_t` correspondant à une durée en millisecondes. `HAL_GPIO_TogglePin()` prend un port (ex: GPIOI) et un numéro de pin (ex: GPIO_PIN_1).**

4. Dans quel fichier les ports d’entrée/sorties sont-ils définis ?  
   **Réponse : Ils sont définis dans `main.h` à partir de la configuration du fichier `.ioc`.**

5. Écrivez un programme simple permettant de faire clignoter la LED (PI1).  
   **Commit : blink no rtos** – *vérif. prof. OK*

6. Modifiez le programme pour que la LED s’allume lorsque le bouton USER (PI11) est appuyé.  
   **Commit : blink no rtos** – *vérif. prof. OK*

---

### 1. FreeRTOS, tâches et sémaphores

#### 1.1 Tâche simple

1. Quels paramètres de FreeRTOS vous paraissent pertinents ? En quoi `configTOTAL_HEAP_SIZE` est-il important ?  
   **Réponse : `configMAX_PRIORITIES`, `configMINIMAL_STACK_SIZE`, `configTOTAL_HEAP_SIZE`. Ce dernier définit la mémoire allouée dynamiquement aux tâches, files, sémaphores, etc.**

2. Quel est le rôle de la macro `portTICK_PERIOD_MS` ?  
   **Réponse : Elle permet de convertir un délai exprimé en millisecondes vers des ticks système.**

3. Créez une tâche faisant clignoter la LED toutes les 100 ms avec un affichage série.  
   *vérif. prof. OK* — (*cf. commit associé à la tâche*)

#### 1.2 Sémaphores pour la synchronisation

4. Créez deux tâches `taskGive` et `taskTake` synchronisées via un sémaphore.  
   **Réponse : `taskGive` utilise `xSemaphoreGive()` pour libérer un sémaphore toutes les 100 ms. `taskTake` attend ce sémaphore avec `xSemaphoreTake()`. Chaque tâche affiche un message avant et après l’appel.**  
   *vérif. prof. OK*

5. Ajoutez une gestion d'erreur si `taskTake` ne reçoit pas le sémaphore sous 1 seconde.  
   **Commit : semaphore "watchdog"** – *vérif. prof. OK*

6. Ajoutez 100 ms au délai de `taskGive` à chaque itération et observez les conséquences.  
   *vérif. prof. OK*

7. Changez les priorités des tâches et expliquez l’impact.  
   **Réponse : Lorsque `taskGive` a une priorité trop basse, elle ne libère plus le sémaphore à temps. Cela provoque un reset logiciel. Si la priorité est suffisante, la tâche fonctionne normalement.**

#### 1.3 Notifications

8. Remplacez les sémaphores par des notifications de tâches.  
   **Commit : task notifications + queue for delay value** – *vérif. prof. OK*

#### 1.4 Queues

9. Modifiez `taskGive` pour envoyer la valeur d’un timer dans une queue. `taskTake` la lit et l’affiche.  
   *vérif. prof. OK* — (*cf. commit task notifications + queue for delay value*)

#### 1.5 Réentrance et exclusion mutuelle

10. Recopiez le code `task_bug()` fourni et observez les sorties.  
       vérif. prof. OK*

11. Expliquez le problème observé.  
       **Réponse : L'utilisation simultanée de `printf()` par plusieurs tâches entraîne des collisions car la fonction n’est pas réentrante.**

12. Proposez une solution avec un mutex.  
       **Réponse : Encadrer les appels à `printf()` avec `xSemaphoreTake()` / `xSemaphoreGive()` sur un sémaphore mutex créé avec `xSemaphoreCreateMutex()`.**



---

### 2. On joue avec le Shell

1. Terminez l’intégration du shell commencé en TD.  
   **Réponse : Le shell a été intégré avec gestion de commande via une file. Les priorités des interruptions ont été ajustées selon les consignes (USART1 priorité 5).**  
   *vérif. prof. OK*

2. Que se passe-t-il si l’on ne respecte pas les priorités décrites précédemment ?  
   **Réponse : Si l’USART1 a une priorité plus élevée que `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY`, elle ne pourra pas appeler les primitives FreeRTOS, ce qui casse le fonctionnement du shell.**

3. Écrivez une fonction `led()` permettant de faire clignoter la LED.  
   **Réponse : La fonction `led()` crée une tâche de clignotement, et utilise une queue pour transmettre la période de clignotement depuis le shell. Une valeur de 0 éteint la LED.**  
   **Commit : building shell led command** – *vérif. prof. OK*

4. Écrivez une fonction `spam()` qui affiche du texte dans la liaison série.  
   **Réponse : La fonction `spam()` crée une tâche affichant un message périodique. Les arguments permettent de définir le texte et le nombre d’occurrences.**  
   **Commit : added spam task functionnality to shell** – *vérif. prof. OK*

---

### 3. Debug, gestion d’erreur et statistiques

#### 3.1 Gestion du tas

1. Quel est le nom de la zone réservée à l’allocation dynamique ?  
   **Réponse : La heap.**

2. Est-ce géré par FreeRTOS ou par la HAL ?  
   **Réponse : Par FreeRTOS.**

3. Ajoutez de la gestion d’erreur sur toutes les fonctions critiques.  
   **Réponse : Les appels critiques (xTaskCreate, malloc...) sont vérifiés via `configASSERT` ou test de retour, et redirigent vers `Error_Handler()`.**  
   *vérif. prof. OK*

4. Notez la mémoire RAM et Flash utilisée (1re config).  
   **Réponse : 19 kB de RAM utilisés avant saturation de la heap.**

5. Créez des tâches jusqu’à provoquer une erreur d’allocation.  
   *vérif. prof. OK*

6. Notez la nouvelle utilisation mémoire.  
   **Réponse : On ne peut pas voir la différence dans le build analyzer statique car le remplissage a lieu à l'exécution.**

7. Augmentez la `TOTAL_HEAP_SIZE` et testez.  
   *vérif. prof. OK*

8. Notez la nouvelle utilisation mémoire.  
   **Réponse : Après augmentation, les tâches s’exécutent à nouveau.**

#### 3.2 Gestion des piles

1. Lisez la doc FreeRTOS sur les Stack Overflow.  
   *vérif. prof. OK*

2. Activez `CHECK_FOR_STACK_OVERFLOW` dans CubeMX.  
   *vérif. prof. OK*

3. Écrivez la fonction `vApplicationStackOverflowHook`.  
   **Réponse : Fonction écrite et utilisée pour afficher un message ou clignoter la LED en cas de dépassement de pile : tableau rempli itérativement avec un printf à chaque étape pour observer à quel moment on a un overflow.**

4. Forcez une tâche à dépasser sa pile pour tester.  
   *vérif. prof. OK*

5. Quels sont les autres hooks de FreeRTOS et leur utilité ?  
   **Réponse :**
   - `vApplicationIdleHook` : appelée quand aucune tâche n’est active.
   - `vApplicationMallocFailedHook` : appelée lors d’un échec d’allocation.
   - `vApplicationTickHook` : appelée à chaque tick, si activée.

#### 3.3 Statistiques dans l’IDE

1. Activez dans CubeMX :
   - `GENERATE_RUN_TIME_STATS`
   - `USE_TRACE_FACILITY`
   - `USE_STATS_FORMATTING_FUNCTIONS`  

2. Générez, compilez, lancez en debug.  

3. Affichez `FreeRTOS Task List` dans STM32CubeIDE.  

4. Lancez et mettez en pause pour visualiser les stats.  
   *vérif. prof. OK*

5. Activez le "Toggle Stack Checking" pour voir l'utilisation pile. 

6. Implémentez le timer :

​	**Réponse : Deux fonctions implémentées pour mesurer le temps CPU via un timer.**

1. Affichez sémaphores et queues dans le debug IDE.
   *vérif. prof. OK*
2. Créez deux tâches se partageant une queue ou un sémaphore.
3. Nommez-les avec `vQueueAddToRegistry`.
    *vérif. prof. KO*

#### 3.4 Affichage des statistiques dans le shell

1. Écrivez une fonction shell pour afficher les stats FreeRTOS.
    **Réponse : Utilisation de `vTaskGetRunTimeStats()` et `vTaskList()` dans une commande shell dédiée.**
    *vérif. prof. OK*
