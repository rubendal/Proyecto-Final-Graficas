## Cosas que hacer

* Habrá naves enemigas que disparan. Las naves enemigos son modelos 3d con
iluminación y textura

* Las naves enemigas serán eliminadas al recibir 5 disparos. Usando
materiales se debe mostrar el nivel de daño que tiene la nave enemiga

* Se debe llevar un registro de los enemigos matados en forma de un score en
pantalla. El score debe ser en texto. (glutbitmapcharacter)

* El juego debe tener una pantalla de inicio donde se muestren los controles
básicos y se espere a que el usuario apriete una tecla para comenzar.


## Incompletos

* Se deben calcular colisiones entre la nave y los asteroides usando Bounding
Spheres. Si la nave choca con un asteroide el juego termina (Reemplazar exit() con pantalla de game over)

* Deben existir por lo menos 3 luces en el escenario. (Hay 1 luz)

* Los asteroides y enemigos deben aparecer de forma aleatoria y avanzarán
lentamente de derecha a izquierda. La dificultad del nivel debe ser
incremental (Faltan los enemigos)

* Al llegar al final del nivel se debe mostrar una pantalla de “Ganaste” (Se detecta el final del nivel con el tiempo transcurrido)

* Si la vida del jugador llega a 0 se mostrará una pantalla de “Game Over”. (falta pantalla de game over)

## Con errores/problemas

* Implementar Parallax con 3 imágenes de fondo usando transformaciones en
las coordenadas de textura. (Corregir imagenes para que se repita y no se note mucho)

## Terminados

* Los controles serán con WASD. Con “W” y “S” se moverá la nave hacia arriba
y abajo. Con “A” y “D” la nave se moverá hacia adelante y atrás

* La nave no se debe de salir de los límites de la pantalla

* Se debe utilizar los eventos de tecla apretada y tecla soltada para poder
apretar dos teclas al mismo tiempo y tener un movimiento continuo y fluido.

* La nave debe ser un modelo 3D en formato obj con texturas y materiales

* En cierto momento del juego aparecerán asteroides que el jugador debe
evitar. Los asteroides son modelos 3D con texturas y materiales

* El jugador puede disparar presionando la barra espaciadora

* Debe existir una barra de municiones en la UI. Cada que se dispara, la barra
de municiones disminuye.

* Deben aparecer en el escenario powerups que permiten rellenar la barra de
municiones. El jugador recolecta el powerup tocando el ícono con la nave

* Deben aparecer en el escenario powerups que te aumentan los puntos de
vida. El jugador los recolecta tocándolos

* Hay una barra de vida. La barra de vida disminuye cuando un enemigo te
dispara
