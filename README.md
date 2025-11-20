# uned-pp
PRÁCTICA DE PROCESAMIENTO PARALELO
 CURSO ACADÉMICO 2025-2026

ENUNCIADO

 La práctica consiste en desarrollar los programas necesarios para solucionar 4 ejercicios similares a 
los que suelen aparecer en las pruebas presenciales.

 EJERCICIO 1
 
 Teniendo inicialmente N procesos estáticos, se debe escribir un programa en MPI donde los 
procesos realicen comunicaciones en anillo. Los procesos de rango par se comunicarán únicamente 
entre ellos en sentido ascendente del rango y los procesos de rango impar se comunicarán en sentido 
descendente. Iniciarán la comunicación los procesos de menor rango en su MPI_COMM_WORLD.

 EJERCICIO 2
 
 Considere 2 procesos estáticos iniciales. Cada uno de estos procesos creará su propio hijo dinámico 
de manera independiente. A su vez, cada uno de estos hijos dinámicos creará también su propio hijo 
dinámico (nieto del estático inicial). De este modo se tendrá un entorno de comunicación en el que 
los padres podrán comunicarse entre ellos, además de cada padre de forma independiente con su 
hijo, y cada hijo con el nieto correspondiente. Gestione los comunicadores existentes para crear los 
siguientes comunicadores: ----

* Un intracomunicador que contenga al estático de rango 0 y al hijo del estático de rango 1. 
* Un intracomunicador que contenta al estático de rango 1 y al hijo del estático de rango 0. 
* Un intracomunicador que contenga únicamente a los dos hijos. 
* Un intracomunicador que contenga únicamente a los dos nietos.
  
Compruebe el correcto funcionamiento de cada uno de estos intracomunicadores realizando 
operaciones de comunicación punto a punto entre los dos procesos que componen cada 
comunicador. Escriba todo el desarrollo en un único programa.

 EJERCICIO 3
 
 Un proceso estático inicial (padre) creará de forma dinámica P procesos hijos. Se desea que estos P procesos hijos calculen la integral de una determinada función f(x) en el intervalo [a,b] por el método de Simpson 1/3. Este método consiste en dividir el intervalo comprendido entre los puntos 
a y b en m porciones regulares de anchura h (siendo m par), de tal manera que m·h = b-a. La integral se calcula de manera aproximada por un polinomio de segundo orden de la siguiente 
manera:

 b
 ∫
 a
 m/2
 f (x)dx≈h
 3
 ∑
 j=1
 {f (x2j−2
 )+4 f (x2j−1
 )+f (x2j
 )}

 
 donde xi = a+ih, para i = 0..m  (x₀ = a y xm = b) 

 
Escriba un programa en MPI que calcule la integral para las funciones:

- f(x) = x en el intervalo [0,10] (solución : https://www.wolframalpha.com/input?i2d=true&i=Integrate%5Bx%2C%7Bx%2C0%2C10%7D%5D&lang=es)
- f(x) = sin(x) en el intervalo [0,π] (solución: https://www.wolframalpha.com/input?i2d=true&i=Integrate%5Bsin%5C%2840%29x%5C%2841%29%2C%7Bx%2C0%2C%CF%80%7D%5D&lang=es)

 utilizando un número de procesos P que no sea divisor de m. teniendo en cuenta que los datos para el cálculo de la integral (parámetros P, a, b, y m) los debe pedir el padre por entrada estándar. El padre no participará en el cálculo de la integral.

  EJERCICIO 4
 
 Escribir un programa en MPI que genere una imagen correspondiente al Conjunto de Mandelbrot (https://es.wikipedia.org/wiki/Conjunto_de_Mandelbrot). La imagen debe tener un tamaño de 
1000x1000 pixels. El conjunto se debe calcular, como mínimo, para 10, 25 y 50 iteraciones por pixel, debiendo entregar en cada caso la imagen correspondiente.
Posteriormente, se debe realizar un análisis de rendimiento del algoritmo desarrollado para un numero variable de procesos involucrados en el cálculo. El análisis del rendimiento deberá incluir una comparativa del tiempo de ejecución y de la memoria utilizada por cada proceso. Con los resultados del análisis se debe proponer alguna posible optimización al código desarrollado. 

También se valorará la implementación de dichas optimizaciones y su comparación con los resultados previos
