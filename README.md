
# RayLibArcade

Emulateur logiciel en C / C++, basé sur le principe de M.A.M.E.  
L'objectif est de réaliser un émulateur software pour un ESP32-P4. Cependant, ce projet existe pour "simplifier" le développement afin de ne pas avoir à flasher le micro-controleur et aussi de pouvoir avancer sur le développement sans avoir l'ESP32-P4.  

J'ai choisi d'utiliser le framework RayLib car c'est un framework que j'ai déjà utilisé pour faire de la 3D. ~~Au final, en pur gestion bitmap 2D, cela ne se révèle pas être un bon choix car j'ai un framerate complétement à la ramasse quand je change la résolution du jeu en doublant les dimensions. C'est ok dans la résolution de base des jeux. La gestion du double buffer de RayLib est en "conflit" avec mon moteur graphique d'émulation. Pour tester c'est ok, mais cela ne sera pas utilisable en vrai sur ordinateur. Vu mon besoin, un SFML ou SDL aurait été un meilleur choix. Cela n'est pas bien grâve. J'adapterai peut-être un jour.~~  

Le projet "réel" est **[EspArcade](https://github.com/BorisFR/EspArcade)**. Il partage l'ensemble des fichiers sources de ce projet. Seul la partie compilation est différente.

Pour le moment, l'audio n'est pas du tout pris en compte. C'est une fonctionalité que je ne connais pas du tout, je n'ai jamais développé sur cette fonctionalité. Cela viendra plus tard quand j'aurai quelques jeux d'opérationnels.  

> <ins>Auteur :</ins> Boris  
<ins>Démarrage :</ins> Août 2025  

## Jeux  

J'utilise actuellement les ROMs de la version de M.A.M.E. 0.279. Il faut de-zipper les fichiers dans le nom de dossier correspondant.  
- ~~Crush Roller (set 1) : crush.zip~~ (abandon pour le moment)  
- Frogger : frogger.zip  
- Galaxian (Namco set 1) : galaxian.zip  
- Pac-Man (Midway) : pacman.zip  
- Pengo (World, not encrypted, rev A) : pengo.zip (ne fonctionne pas du tout)
- Space Invaders / Space Invaders M : invaders.zip  
- Space Invaders Part II : invadpt2.zip  

<img src="sdcard/ss/invaders.png" alt="Space Invaders" width="150" /> <img src="sdcard/ss/invadpt2.png" alt="Space Invaders Part II" width="150" /> <img src="sdcard/ss/pacman.png" alt="Pacman" width="150" /> <img src="sdcard/ss/galaxian.png" alt="Galaxian" width="150" /> <img src="sdcard/ss/frogger.png" alt="Frogger" width="150" /> <img src="sdcard/ss/pengo.png" alt="Pengo" width="150" />  

## Remerciements

- La MAMEdev Team : [https://docs.mamedev.org/](https://docs.mamedev.org/)  
- Harbaum pour son Galagino - ESP32 Arcade Emulator : [https://github.com/harbaum/galagino](https://github.com/harbaum/galagino)  
- Sandroid75 pour son émulateur PAC : [https://github.com/Sandroid75/pac](https://github.com/Sandroid75/pac)  
- Justin-Credible pour son émulateur Pacman en C# : [https://github.com/Justin-Credible/pac-man-emulator](https://github.com/Justin-Credible/pac-man-emulator)  
- tommojphillips pour son émulateur Space Invaders : [https://github.com/tommojphillips/Space-Invaders](https://github.com/tommojphillips/Space-Invaders)  
- camelliya pour son émulateur Space Invaders : [https://github.com/camelliya/pacman](https://github.com/camelliya/pacman)  
- JoseLGF pour son émulateur Space Invaders : [https://github.com/JoseLGF/SpaceIvaders](https://github.com/JoseLGF/SpaceIvaders)  
- Jean-Marc Harvengt pour son teensyMame : [https://github.com/Jean-MarcHarvengt/teensyMAME](https://github.com/Jean-MarcHarvengt/teensyMAME)  
- bitbank2 pour son décodeur de fichier JPEG : [https://github.com/bitbank2/JPEGDEC](https://github.com/bitbank2/JPEGDEC)  

## Blog

<details>
<summary>Octobre 2025</summary>  

* Menu : J'ai commencé l'implémentation d'un menu pour choisir le jeu. J'ai également implémenté le fait de pouvoir quitter un jeu et d'en démarrer un autre. J'ai dû revoir légèrement l'allocation et libération de mémoire. La gestion du tactile/souris permet maintenant de détecter le "click" et le "scroll". Mon programme permet donc maintenant de démarrer sur le menu, lancer l'exécution un jeu, de le quitter pour revenir au menu, puis de démarrer l'exécution d'un autre jeu.
* Background & RayLib : l'implémentation de la transparence est maintenant normalement terminé, au moins pour les jeux fonctionnels actuellement. J'ai redessiné un background sur base de marquee et de contour d'écran pour chacun. Concernant l'utilisation de RayLib, la fenêtre est maintenant redimensionable et je fais en sorte que le ratio d'écran soit maintenu. Ce ratio est basé sur l'implémentation finale pour l'ESP32-P4 avec un écran de 800x1280. J'ai également implémenté la prise de screenshot pour alimenter cette page, l'ajout de crédit se fait via la touche '5' et le démarrage d'une partie par la touche '1'.  
* Dans la continuité de Galaxian, sur le même PCB (carte mère) il y a le jeu Pengo qui m'intéresse. C'est parti pour l'implémenter. Nouveauté : rom cryptée. J'implémente un truc par copier/coller du source de M.A.M.E... et c'est un échec. Le nom de la ROM est "not encrypted". J'essai avec ou sans décodage de la ROM mais sans succès... J'ai implémenté un peu plus de la logique de M.A.M.E. lors de la prise en compte de chaque jeu, particulièrement sur le chargement des ROM et des fonctions associées.  
* Galaxian & background : L'implémentation de Galaxian fût relativement facile. Elle est presque terminé, il reste à développer le champs d'étoiles qui défile. Pour "background", l'objectif est d'ajouter une image de fond plein écran derrière l'écran du jeu. J'ai commencé en utilisant l'excellente bibliothèque PNGdec de BitBank2. Manifestement, j'ai un beau souci d'intégration avec mon code. Après de nombreuses heures, j'ai utilisé son autre bibliothèque JPEGdec : succès ! J'ai réalisé rapidement quelques fonds d'écran pour chacun des jeux implémentés, ajouté un positionnement vertical des écrans et ça roule. Il faut encore affiner le graphisme de ces fonds d'écran, mais pour le moment, j'en suis satisfait et je peux passer à la suite. Il faut encore que j'implémente le redessin de ce fond d'écran quand il y a une couleur transparente sur l'écran.  
* Point de situation : le texte "credit" n'apparait pas dans le jeu Pacman, le nombre de crédits, lui, apparaît bien. Bizarre... ⁉️ Concernant Frogger, toujours le souci des couleurs, le nombre de vies qui n'apparaît pas, le jeu est en mode vie infini, gros travail sur le scrolling à faire, ... Bref, du boulot pour Frogger 😁 Et c'est à ce moment que je me rend compte que Frogger n'est pas sur le PCB de Pacman mais de Galaxian. Je me dis que pour avoir un Frogger opérationnel, je dois passer d'abord par l'implémentation de Galaxian. C'est parti!  
* J'ai trouvé le souci des inputs pour Pacman : un bug dans la gestion de l'interruption car j'écrasais la valeur du port 0, comme je le faisais avec Space Invaders. La logique est ici différente. J'ai maintenant corrigé le souci avec RayLib et les premiers jeux (sans limite de fps) fonctionne à plus de 1000 fps...  
* Changement de l'émulateur CpuI8085 pour i8080.Semble ok pour Space Invaders mais ne fonctionne pas pour Space Invaders Part II. Je teste en version ESP...  Bon, au final j'avais introduit un bug dans la gestion du "dirty screen". Comme cela ne fonctionnait pas uniquement avec les jeux i8080, j'avais mis en cause l'émulation du processeur... j'en ai donc implémenté un second. Ce second émulateur 'i8080' ne fonctionne pas avec Space Invaders Part II. Je suis donc revenu sur le premier 'i8085' et j'ai continué à creuser jusqu'à trouvé le bug. Cela ne se voyait pas dans les autres jeux car le 'dirty screen' n'est pas implenté pour ces jeux. Tout est à nouveau opérationnel et synchro en les deux versions😊.
* Début de l'implémentation du scrolling pour le jeu Frogger. Il semble que ce premier code soit opérationnel comme il le devrait. J'en suis surpris, il faut que je creuse un peu plus. J'ai un doute sur un potentiel décalage d'une ligne vers le bas.  
J'ai refait une synchro avec EspArcade. L'émulation I8080 ne fonctionne plus, l'ESP32-P4 fait un reboot à cause d'un morceau de code de ce processeur 😒  
* Le souci des INPUT_PORT est réglé : je n'avais pas implémenté toutes les touches (input du player 2 par exemple, ou encore le COIN2, COIN3", START P2, ...). Maintenant que c'est implémenté, ~~le souci de joystick de Pacman est réglé et~~ je sais enfin démarrer une  partie de FROGGER et même commencer à jouer. Bon, j'ai un souci avec les couleurs des sprites (pas des tiles) et je n'ai pas implémenté le scrolling. Il y a également un souci avec le nombre de vies : elles n'apparaissent pas et j'en ai un nombre infini. Pour le moment je n'ai aucune idée du pourquoi de ce phénomène.
* J'ai une première version des INPUT_PORT opérationnelle. Je me plonge dans le décodage graphique et c'est enfin un succès. J'ai les sprites de Pacman et de Frogger ! Space Invaders et sa suite sont toujours fonctionnels. Il y a cependant un "truc" avec les INPUT_PORT car au démarrage d'une partie de Pacman, le joystick ne fonctionne pas comme il devrait, je dois faire un mouvement dans toutes les directions pour que cela fonctionne correctement. Avec Frogger, je ne sais même pas simuler un COIN1. Le jeu Pacman est maintenant pleinement opérationnel ! J'ai commencé l'implémentation du clipping car les jeux positionnent des sprites sur l'écran dans des zones qui ne sont au final pas affichés.    
* Je tourne en rond, je n'arrive pas à avoir une fonction générique pour décoder les graphismes. Je plonge dans le code source de M.A.M.E. version 0.37xxx (genre pure C) et sur la toute dernière (C / C++ orienté objet). Je commence à intégrer la logique. J'en extrait du code que j'intègre dans le mien. Je commence par les macros permettant de charger les ROMS. Cela me permet d'avoir un gestionnaire de mémoire. J'intègre ensuite la partie qui gère les INPUT. Je revois complétement l'architecture logicielle de mon programme. Le tout nécessite la réécriture de nombreux morceaux de code initial. Le programme devient plus "générique". Pour intégrer un jeu, il faut un fichier de définitions (jeu.h) qui sont majoritairement copiées depuis Mame lui-même et un fichier de codes (jeu.c) qui implémente les spécificités pour le jeu. On retrouve la logique d'implémentation de Mame car je m'en inspire énormément.  
</details>

<details>
<summary>Septembre 2025</summary>  

* En "bidouillant", j'arrive à avoir les sprites opérationnels. Cependant cela ne me satisfait pas trop car cela semble trop en "dur" pour ce jeu et ne fonctionnera surement pas pour d'autres.  Je décide donc d'essayer sur un autre jeu Z80. Je choisis d'abord Crush Roller mais eu final, il y a une protection des ROM et je ne pas trop envie de m'y atteler dès à présent. Je bascule alors sur le jeu Frogger. Celui-ci utilise deux Z80, mais un des deux n'est utile que pour l'audio : cela devrait donc me suffir pour valider mon développement.  Bon, une petite adaptation est nécessaire pour les ROM car certaines ne sont pas utilisale tel quel pour avoir le graphisme. Une inversion des bit 0 et 1 a lieu sur la première rom graphique et également pour la première ROM audio.  
* J'avance doucement en analysant d'autres émulateurs Pacman. J'arrive à faire fonctionner l'émulation Z80, le souci étant ue j'aimerai ne faire que du C++ mais quasi tout les émulateurs sont en pur C. L'autre souci est au niveau graphique : il faut décoder le stockage des tiles et sprites. Je m'inspire de ces autres émulateurs, cela commence à fonctionner, au moins pour les tiles de taille 8x8. Pour les sprites 16x16, c'est un peu tout mélangé. J'ai ma fonction qui extrait les couleurs et également les palettes de couleurs. Je suis un peu perdu avec la notion de "pen" de Mame.  
* J'enchaine sur un nouveau jeu avec un autre processeur. Les jeux sur le processeur I8080 en noir et blanc sont tout de même un peu trop ancien à mon goût. Je me contenterai de Space Invaders et Space Invaders Part II.  
Le nouveau jeu aura donc de la couleur est mon choix se porte sur Pacman qui fonctionne sur un processeur Z80.  
* Histoire de valider le début du code, j'implémente un jeu ressemblant : Space Invaders Part II. Quelques soucis dans le code mais au final, cela fonctionne. Je peaufine l'affichage afin d'implémenter le filtre de couleurs.
</details>

<details>
<summary>Août 2025</summary>  

* Je démarre l'écriture du code avec un jeu simple à émuler. Mon choix se porte sur Space Invaders.  
* Il n'y a pas de gestion de couleur, c'est juste un filtre transparent sur l'écran.En fonction de l'emplacement du pixel allumé, le joueur le verra au travers du filtre et verra donc la couleur.  
Le jeu allume et éteint les pixels, chaque pixel est un bit d'un octet.  
Découverte du jeu, il fonctionne avec un processeur I8080. J'ai rapidement un affichage qui fonctionne. Sauf qu'il n'y a pas les envahisseurs ! Il faut implémenter les interruptions au niveau de l'émulation du processeur. J'implemente dans la foulée une gestion minimale des inputs afin de pouvoir commencer à interagir avec le jeu. J'implémente une gestion d'écran bitmap avec une gestion du "dirty" afin d'optimiser l'affichage et obtenir un 60 images par secondes au minimum sur l'ESP32-P4.  
</details>
