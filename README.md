
# RayLibArcade

Emulateur logiciel en C / C++, basé sur le principe de M.A.M.E.  
L'objectif est de réaliser un émulateur software pour un ESP32-P4. Cependant, ce projet existe pour "simplifier" le développement afin de ne pas avoir à flasher le micro-controleur et aussi de pouvoir avancer sur le développement sans avoir l'ESP32-P4.  

J'ai choisi d'utiliser le framework RayLib car c'est un framework que j'ai déjà utilisé pour faire de la 3D. Au final, en pur gestion bitmap 2D, cela ne se révèle pas être un bon choix car j'ai un framerate complétement à la ramasse. La gestion du double buffer de RayLib est en "conflit" avec mon moteur graphique d'émulation. Pour tester c'est ok, mais cela ne sera pas utilisable en vrai sur ordinateur. Vu mon besoin, un SFML ou SDL aurait été un meilleur choix. Cela n'est pas bien grâve. J'adapterai peut-être un jour.  

Le projet "réel" est **[EspArcade](https://github.com/BorisFR/EspArcade)**. Il partage l'ensemble des fichiers sources de ce projet. Seul la partie compilation est différente.

Pour le moment, l'audio n'est pas du tout pris en compte. C'est une fonctionalité que je ne connais pas du tout, je n'ai jamais développé sur cette fonctionalité. Cela viendra plus tard quand j'aurai quelques jeux d'opérationnels.  

> <ins>Auteur :</ins> Boris  
<ins>Démarrage :</ins> Août 2025  

## Blog

- Je tourne en rond, je n'arrive pas à avoir une fonction générique pour décoder les graphismes.  

- En bidouillant, j'arrive à avoir les sprites opérationnels. Cependant cela ne me satisfait pas trop car cela semble trop en "dur" pour ce jeu et ne fonctionnera surement pas pour d'autres.  Je décide donc d'essayer sur un autre jeu Z80. Je choisis d'abord Crush Roller mais eu final, il y a une protection des ROM et je ne pas trop envie de m'y atteler dès à présent. Je bascule alors sur le jeu Frogger. Celui-ci utilise deux Z80, mais un des deux n'est utile que pour l'audio, cela devrait donc me suffir pour valider mon développement.  Bon, une petite adaptation est nécessaire pour les ROM car certaines ne sont pas utilisale tel quel pour avoir le graphisme. Une inversion des bit 0 et 1 sur la première rom graphique (et également pour la première ROM audio).  

- J'avance doucement en analysant d'autres émulateurs Pacman. J'arrive à faire fonctionner l'émulation Z80, le souci étant ue j'aimerai ne faire que du C++ mais quasi tout les émulateurs sont en pur C. L'autre souci est au niveau graphique : il faut décoder le stockage des tiles et sprites. Je m'inspire de ces autres émulateurs, cela commence à fonctionner, au moins pour les tiles de taille 8x8. Pour les sprites 16x16, c'est un peu tout mélangé.  

- J'enchaine sur un nouveau jeu avec un autre processeur. Les jeux sur le processuer I8080 en noir et blanc sont tout de mâme un peu trop ancien à mon goût. Je me contenterai de Space Invaders et Space Invaders Part II.  
Le nouveau jeu aura donc de la couleur est mon choix se porte sur Pacman qui fonctionne sur un processeur Z80.  

- Histoire de valider le début du code, j'implémente un jeu ressemblant : Space Invaders Part II. Quelques soucis dans le code mais au final, cela fonctionne. Je peaufine l'affichage afin d'implémenter le filtre de couleurs.

- Je démarre l'écriture du code avec un jeu simple à émuler. Mon choix se porte sur Space Invaders. 
Il n'y a pas de gestion de couleur, c'est juste un filtre transparent sur l'écran.En fonction de l'emplacement du pixel allumé, le joueur le verra au travers du filtre et verra donc la courleur.  
Le jeu allume et éteint les pixels, chaque pixel est un bit d'un octet.  
Découverte du jeu, il fonctionne avec un processeur I8080. J'ai rapidement un affichage qui fonctionne. Sauf qu'il n'y a pas les envahisseurs ! Il faut implémenter les interruptions au niveau de l'émulation du processeur. J'implemente dans la foulée une gestion minimale des inputs afin de pouvoir commencer à interagir avec le jeu. J'implémente une gestion d'écran bitmap avec une gestion du "dirty" afin d'optimiser l'affichage et obtenir un 60 images par secondes au minimum sur l'ESP32-P4.