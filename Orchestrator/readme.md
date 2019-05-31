# Projet Soatcar v2.0
Le principe est d'articuler des "parts" écrits en Python, C++, C#, JavaScript autour d'un Etat partagé via un Memory Mapped File (MMF).  


# Orchestrateur
## Configuration
Sous forme de fichier ini :  
- Une section Main contenant des déclarations générales : le répertoire racine, la conduite automatique ou non, le hostname, ...  
- Une section Setup permettant de lancer des commandes avant le démarrage des parts
- Une section Teardown permettant de lancer des commandes après l'arrêt des parts  
- Une section par part pour définir la commande à lancer, dans quel répertoire et son fichier pid  

## Déroulement
### Start
Lancement avec le paramètre start  
L'orchestrateur vérifie les fichiers pids définis dans sa config et s'arrête si un process tourne avec un pid défini dans ces fichiers.  
L'orchestrateur lance les commandes de la section [Setup] : créer/recréer le fichier MMF, supprimer les fichiers pids restant, ...   
L'orchestrateur applique la section [Main] : positionner les flags SteeringAuto et ThrottleAuto comme indiqué et le stopFlag à false.  
L'orchestrateur suit les sections parts dans l'ordre du fichier en lançant les exec= depuis le répertoire dir=

### Status
Lancement avec le paramètre status  
L'orchestrateur lit les section part, récupère le nom du fichier pid, lit dedans le pid et vérifie si la part tourne ou non en vérifiant si un process existe avec ce pid.  

### Stop
Lancement avec le paramètre stop  
L'orchestrateur met le flag StopFlag à 1 et attend le temps configuré dans la section [Main] avant de vérifier les fichiers pids pour voir si les parts se sont bien arrêtées. Si ce n'est pas le cas, lance un kill sur les parts non arrêtées, voir un kill -9.  
Enfin il lance les commandes de la section [TearDown]



# Organisation du MMF
Le MMF n'est rien d'autre qu'un bytearray partagé. Autrement dit une zone mémoire sans structure, allant de l'octet 0 à l'octet N ou N est la taille de la zone mémoire partagée.  

## Les bases
Il faut donc structurer cette zone sachant que :  
- un booléen (autrement appelé flag) peut prendre un bit mais sera plus facile à gérer sur un octet  
- un entier prend 32 bits soit 4 octets  
- un long prend 64 bits soit 8 octets. Attention en JS on est fortement limité en taille d'int, les long n'existent pas, les unsigned non plus...  
- une chaine de caractère a une taille indéfinie : on devra mettre sa taille sur les premier 4 octets puis le texte lui même à la suite  
- une image a aussi une taille indéfinie : même principe que pour le texte  
- dans le cas de la chaine de caractères et de l'image, il faut prévoir suffisamment de place pour que toute image ou chaine tienne dans l'espace défini  
- ne pas utiliser de double ou float : ils ne sont pas forcément représentés de la même manière en mémoire par les différents langages 
- si possible éviter les négatifs : ce sera plus simple   
  
## Les champs requis
Il s'en ajoute chaque jour, liés aux diverses parts et aux usages.    
Le plus simple est de séparer le MMF en zones de 10Ko, une zone "générale" et ensuite une zone par part (c'est à dire une zone de 10Ko où une part spécifique peut écrire et les autres lire).    
On se met alors d'accord sur les premiers champs et on en ajoute selon la nécessité avant de les officialiser.   
  
Découpage par 10Ko :   
- 0 à 10Ko : les champs généraux (Stop flag, conduite autonome, vitesse autonome, max de vitesse...) voir de la configuration si besoin est.   
- 10Ko à 20Ko : Le joystick   
- 20Ko à 30Ko : Le PWM   
- 30Ko à 40Ko : Les Sensors système du Pi (CPU, RAM, Disque...)   
- 40Ko à 50Ko : Les Sensors physiques (Ultrasonic...)   
- 50Ko à 60Ko : La part de decision/d'auto-pilote par réseau de neurone, openCV (attention l'image traitée est à la position 2Mo) ou autre   
- 60Ko à 70Ko : La caméra (attention l'image elle même est à la position 1Mo)   
- 70Ko à 80Ko : Le serveur web de remote monitoring 
- 80Ko à 90Ko : La gestion des LEDs  
-  ...
- 1000Ko à 2000Ko : l'image brute prise par la caméra   
- 2000Ko à 3000Ko : l'image traitée   


## Organisation

```python
    # 3Mo to share : 1 Mo = 1 048 576 bytes for flags and int values splitted by part, 1MB for raw image, 1MB for filtered image
    
    # Common fields
    CONST_STOP_FLAG						= 0		# byte - Si cette valeur passe à true (1), toutes les parts doivent s'arrêter
    CONST_IS_STEERING_AUTO				= 10	# byte - La voiture gère t'elle la direction en mode autonome ?
    CONST_IS_THROTTLE_AUTO				= 20	# byte - La voiture gère t'elle la vitesse en mode autonome ?
    CONST_MAX_THROTTLE_LIMIT			= 30	# int - Valeur maximale de throttle admise dans la plage [0 - 10000]
    CONST_IS_CONSTANT_THROTTLE_ACTIVE	= 40	# byte - La vitesse est-elle fixée / constante ?
    CONST_CONSTANT_THROTTLE_VALUE		= 50	# int - Fixer une valeur de vitesse constante dans la plage [0 - 10000]
    CONST_HOSTNAME						= 60	# string - hostname servant d'identifiant de la soatcar / l'ordi qui héberge le code (64 characters max) donc faire commencer le suivant à minimum 130)
    
    # Joystick
    CONST_IS_JOYSTICK_READY				= 10000	# byte - La part Joystick est-elle prête ?
    CONST_JOYSTICK_LOOP_RATE			= 10010	# int - Fréquence en Hz de la boucle de traitement des inputs du joystick
    CONST_JOYSTICK_STEERING_VALUE       = 10020	# int - Valeur de steering en entrée manuelle dans la plage [0 - 10000]
    CONST_JOYSTICK_THROTTLE_VALUE		= 10030	# int - Valeur de throttle en entrée manuelle dans la plage [0 - 10000]
    
    # PWM
    CONST_IS_ACTUATOR_READY				= 20000	# byte - La part Actuator/PWM est-elle prête ?
    CONST_ACTUATOR_LOOP_RATE			= 20010	# int - Fréquence en Hz de la boucle de traitement des Actuator/PWM
    CONST_STEERING_OUTPUT				= 20020	# int - La valeur de steering réellement utilisée pour le PWM de direction dans la plage [0 - 10000]
    CONST_THROTTLE_OUTPUT				= 20030	# int - La valeur de throttle réellement utilisée pour le PWM de vitesse dans la plage [0 - 10000]
    
    # System sensors
    CONST_IS_SYSTEM_SENSOR_READY		= 30000	# byte - La part System Sensor (CPU load, RAM, ...) est-elle prête ?
    CONST_SYSTEM_SENSOR_LOOP_RATE		= 30010	# int - Fréquence en Hz de la boucle de traitement des Sensors système
    CONST_CPU_TEMP						= 30020	# int - Température du CPU
    CONST_CPU_LOAD						= 30030	# int - Pourcentage de charge du CPU
    CONST_RAM_LOAD						= 30040	# int - Pourcentage de charge de la RAM
    CONST_DISK_LOAD						= 30050	# int - Pourcentage d'utilisation de l'espace disque
    CONST_LOAD_AVERAGE					= 30060	# int - Load average x 100 : cf top
    
    # Physical sensors
    CONST_IS_PHYSICAL_SENSOR_READY		= 40000	# byte - La part physical Sensor (Ultrasons, ...) est-elle prête ?
    CONST_PHYSICAL_SENSOR_LOOP_RATE		= 40010	# int - Fréquence en Hz de la boucle de traitement des Sensors physiques
    CONST_ULTRASONIC_DISTANCE			= 40020	# int - Distance en mm détectée par le détecteur à ultrasons
    
    # Auto pilot
    CONST_IS_AUTOPILOT_READY			= 50000	# byte - La part d'auto pilote est-elle prête ?
    CONST_AUTOPILOT_LOOP_RATE			= 50010	# int - Fréquence en Hz de la boucle d'auto pilotage
    CONST_AUTO_STEERING_VALUE			= 50020	# int - Valeur de steering en mode automatique dans la plage [0 - 10000]
    CONST_AUTO_THROTTLE_VALUE			= 50030	# int - Valeur de throttle en mode automatique dans la plage [0 - 10000]
    
    # Camera
    CONST_IS_CAMERA_READY				= 60000	# byte - La part caméra est-elle prête ?
    CONST_CAMERA_LOOP_RATE				= 60010	# int - Fréquence en Hz de la boucle de récupération d'images par la caméra
    CONST_IS_RECORDING					= 60020	# byte - Booléen indiquant si la voiture est en train d'enregistrer des immages
    CONST_RECORDING_PREFIX				= 60030	# string - Chaine de caractères qui sera le préfixe des images enregistrées
    CONST_RECORDING_FOLDER				= 60040	# string - Chaine de caractère indiquant le répertoire local dans lequel enregistrer les images
    CONST_RECORDING_IMAGE_INDEX			= 60050	# int - Entier indiquant le numéro de l'image enregistrée, au sein de l'enregistrement en cours
    
    # Remote monitoring web server
    CONST_IS_REMOTE_MONITORING_READY	= 70000	# byte - La part remote monitoring est-elle prête ?
    CONST_REMOTE_MONITORING_LOOP_RATE	= 70010	# int - Fréquence en Hz de la boucle de récupération d'images par la caméra
    
    # Led management
    CONST_IS_LED_MANAGER_READY			= 80000	# byte - La part de gestion des LEDs est-elle prête ?
    CONST_LED_MANAGER_LOOP_RATE			= 80010	# int - Fréquence en Hz de la boucle de gestion des LEDs
    
    # Images
    CONST_CAMERA_IMAGE_NO				= 1000000	# int - Image index
    CONST_CAMERA_IMAGE					= 1000010	# int - Raw image
    CONST_TREATED_IMAGE_NO				= 2000000	# int - Treated image index
    CONST_TREATED_IMAGE					= 2000010	# int - Treated image
```

