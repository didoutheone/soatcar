Projet Soatcar v2.0  
Le principe est d'articuler des "parts" écrits en Python, C++, C#, JavaScript autour d'un Etat partagé via un Memory Mapped File (MMF).  


# Orchestrateur
## Configuration
Sous forme de ficier ini :  
- Une section Main contenant des déclarations générales : le répertoire racine, la conduite automatique ou non  
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
L'orchestrateur met le flag StopFlag à 1 et attend le temps configuré dans la section [Main] avant de vérifier les fichiers pids pour voir si les parts se sont bien arrêtées. Si ce n'est pas le cas, lance un kill sur les parts non arrêtées, voir un kill -9.  
Enfin il lance les commandes de la section [TearDown]



# Organisation du MMF
Le MMF n'est rien d'autre qu'un bytearray partagé. Autrement dit une zone mémoire sans structure, allant de l'octet 0 à l'octet N ou N est la taille de la zone mémoire partagée.  

## Les bases
Il faut donc structurer cette zone sachant que :  
- un booléen (autrement appelé flag) peut prendre un bit mais sera plus facile à gérer sur un octet  
- un entier prend 32 bits soit 4 octets  
- un long prend 64 bits soit 8 octets  
- une chaine de caractère a une taille indéfinie : on devra mettre sa taille sur les premier 4 octets puis le texte lui même à la suite  
- une image a aussi une taille indéfinie : même principe que pour le texte  
- dans le cas de la chaine de caractères et de l'image, il faut prévoir suffisamment de place pour que toute image ou chaine tienne dans l'espace défini  
- ne pas utiliser de double ou float : ils ne sont pas forcément représentés de la même manière en mémoire par les différents langages 
- si possible éviter les négatifs : ce sera plus simple   
  
## Les champs requis
- un flag STOP demandant à toutes les parts de quitter  
- un flag ThrottleAuto : est ce que la vitesse est gérée par un process de décision (ThrottleAuto=true) ou par manuellement via le joystick (ThrottleAuto=false)  
- un flag SteeringAuto  : est ce que la direction est gérée par un process de décision (SteeringAuto=true) ou par manuellement via le joystick (SteeringAuto=false)  

- un flag camera ready  
- un entier camera loop rate en Hz  
- un flag autoPilot ready  
- un entier autoPilot loop rate en Hz  
- un flag joystick ready  
- un entier joystick loop rate en Hz  
- un flag pwm/actuator ready  
- un entier pwm/actuator loop rate en Hz  
- un flag ultrasonic sensor ready  
- un entier ultrasonic loop rate en Hz  
- un flag webserver ready  
- un entier webserver loop rate en Hz  

- un entier décrivant la vitesse demandée sur une echelle de 0 à 100000 (0 : à fond en arrière, 50 000 : au repos, 100 000 : à fond en avant)  
- un entier limitant la vitesse maximale de 0 à 100%  
- un entier décrivant la direction demandée sur une echelle de 0 à 100000 par exemple (0 : à fond à gauche, 50 000 : tout droit, 100 000 : à fond à droite)  
- un entier distance ultrason avant (en millimetre. On a un seul capteur frontal ?)
- un entier pour la temperature CPU x 1000 (en millidegré donc)  
- un entier charge CPU (pourcentage CPU utilisé)  
- un entier load average x1000 (indicateur de charge du serveur - voir top)  
- un entier charge mémoire (pourcentage de RAM utilisé)  
- un entier charge disque (pourcentage de disque utilisé)  

- une zone image non traitée, provenant directement de la caméra   
- un long indiquant le numéro de l'image pour indiquer s'il y a une nouvelle image à traiter  

Peut être :  
- une zone image traitée ? Résultat d'openCV pouvant être utilisé par un algo tel que celui de Renaick plutot que l'image originale ?  
  
Autre ?  

## Organisation
```
    # 3Mo to share : 1 Mo = 1 048 576 bytes for flags and int values, 1MB for raw image, 1MB for filtered image
    # Configuration flags and values
    CONST_STOP_FLAG						= 0		# byte
    CONST_THROTTLE_AUTO					= 10	# byte
    CONST_THROTTLE_MAX_PCT				= 15	# int
    CONST_STEERING_AUTO					= 20	# byte
    
    # Parts ready and loop rates
    CONST_CAMERA_READY					= 30	# byte
    CONST_CAMERA_LOOP_RATE				= 40	# int
    CONST_AUTO_PILOT_READY				= 50	# byte
    CONST_AUTO_PILOT_LOOP_RATE			= 60	# int
    CONST_JOYSTICK_READY				= 70	# byte
    CONST_JOYSTICK_LOOP_RATE			= 80	# int
    CONST_ACTUATOR_READY				= 90	# byte
    CONST_ACTUATOR_LOOP_RATE			= 100	# int
    CONST_ULTRASONIC_READY				= 110	# byte
    CONST_ULTRASONIC_LOOP_RATE			= 120	# int
    CONST_WEBSERVER_READY				= 130	# byte
    CONST_WEBSERVER_LOOP_RATE			= 140	# int

    # Throttle and steering required values between 0 and 100000
    CONST_THROTTLE_VALUE				= 150	# int
    CONST_STEERING_VALUE				= 160	# int
	
    # state variables
    CONST_ULTRASONIC_DISTANCE			= 170	# int
    CONST_CPU_TEMP						= 180	# int
    CONST_CPU_LOAD						= 190	# int
    CONST_LOAD_AVERAGE					= 200	# int
    CONST_MEMORY_LOAD					= 210	# int
    CONST_DISK_LOAD						= 220	# int

    # Images
    CONST_CAMERA_IMAGE					= 1048576 # int for the size followed by the image itself
    CONST_CAMERA_TREATED_IMAGE			= 2097152 # int for the size followed by the image itself
```

