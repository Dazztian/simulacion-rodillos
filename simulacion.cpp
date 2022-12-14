//============================================================================
// Name        : simulacion.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
using namespace std;


static double COTA_SUPERIOR_FDP1 = 0.32;
static double COTA_SUPERIOR_FDP2 = 0.18;
static double COTA_SUPERIOR_TIEMPO_ATENCION = 0.4;


int intervaloDeArribo1(void);
int validarHoraLaboral(int dt );
//nt obtenerDia(int t ):

int obtenerMinimoTC1(int cantidadDeMaquinasModelo1, std::vector<int> &tiempoComprometidoPorMaquinaModelo1);
int obtenerMinimoTC2(int cantidadDeMaquinasModelo2, std::vector<int> &tiempoComprometidoPorMaquinaModelo2);


double random(void);
int tiempoDeAtencion(void);

double fdpIA1(double x1);
double fdpIA2(double x);
double fdpTA(double x);

double randomENTRE(int a, int b);

int main(int argc, char** argv)
{
   //CONDICIONES INICIALES :
   int cantidadDeMaquinasModelo1 = 1;
   int cantidadDeMaquinasModelo2 = 2;

   int tiempo = 0;
   int tiempoFinal =  10000; // 40320; // 1 Mes
   int tiempoProximaLlegada = 0;
   int tiempoLimiteDeRechazo; // 6 horas
   int condicionTiempoRechazo = 16;  //alto
   int intervaloEntreArribos = 0;
   int menorTiempoComprometidoModelo1 = 0;
   int menorTiempoComprometidoModelo2 = 0;
   //int menorTiempoComprometido=0;

   std::vector<int> tiempoComprometidoPorMaquinaModelo1(10);
   std::vector<int> tiempoComprometidoPorMaquinaModelo2(10);

   int pedidosTotales = 0;
   int pedidosRechazados = 0;
   int pedidosAceptados = 0;

   int sumatoriaTiempoEspera = 0;
   double promedioTiempoDeEspera = 0;
   double porcentajePedidosRechazados = 0;

   //----------------------TEST ZONE-------------------------------------------------------------------------------------------------------------------------------
   /*printf("hola\n");
   printf("tiempo de atencion: %d  \n", tiempoDeAtencion() );
   for (int i = 0 ; i < cantidadDeMaquinasModelo1 ; i++)
   {
      printf("random: %f  \n", randomENTRE(10, 13 )  );
      tiempoComprometidoPorMaquinaModelo1[i] = 0;
      // printf("numero: %d \n", tiempoComprometidoPorMaquinaModelo1[i] );
   }*/

   //   for (int i = 0 ; i < 10 ; i++)
   // {
   //    printf("d --->  %d ", (int)randomENTRE(1,8) );
   // }
   //---------------------HASTA ACA BORRAR-----------------------------------------------------------------------------------------------------------------------

   for (int i=0; i<cantidadDeMaquinasModelo1; i++){
      tiempoComprometidoPorMaquinaModelo1[i] = 0;
      // printf("numero: %d \n", tiempoComprometidoPorMaquinaModelo1[i] );
   }

   for (int i=0; i<cantidadDeMaquinasModelo2; i++){
      tiempoComprometidoPorMaquinaModelo1[i] = 0;
      // printf("numero: %d \n", tiempoComprometidoPorMaquinaModelo1[i] );
   }

   tiempoProximaLlegada = intervaloDeArribo1();

   std::vector<int> sumatoriaTiempoOciosoPorMaquinaModelo1(15);
   std::vector<int> sumatoriaTiempoOciosoPorMaquinaModelo2(15);

   for (int i = 0 ; i < cantidadDeMaquinasModelo1 ; i++){
      sumatoriaTiempoOciosoPorMaquinaModelo1[i] = 0;
   }

   for (int i = 0 ; i < cantidadDeMaquinasModelo2 ; i++){
      sumatoriaTiempoOciosoPorMaquinaModelo2[i] = 0;
   }

   //EMPIEZA LA SIMULACION
   int minimoTC1;
   int minimoTC2;

   while (tiempo < tiempoFinal){
      tiempo = tiempoProximaLlegada;
      intervaloEntreArribos = 0;

      intervaloEntreArribos = intervaloDeArribo1();
      tiempoProximaLlegada = tiempo + intervaloEntreArribos;

      minimoTC1 = obtenerMinimoTC1(cantidadDeMaquinasModelo1, tiempoComprometidoPorMaquinaModelo1); //devuelve un entero
      menorTiempoComprometidoModelo1 = tiempoComprometidoPorMaquinaModelo1[minimoTC1];
      pedidosTotales = pedidosTotales + 1;

      if(cantidadDeMaquinasModelo2>=1){ //SI Hay AL MENOS UNA MAQUINA NUEVA de model MR-8
         minimoTC2 = obtenerMinimoTC2(cantidadDeMaquinasModelo2, tiempoComprometidoPorMaquinaModelo2);
         menorTiempoComprometidoModelo2 = tiempoComprometidoPorMaquinaModelo2[minimoTC2];
      }

      if(cantidadDeMaquinasModelo2>=1 && menorTiempoComprometidoModelo1 > menorTiempoComprometidoModelo2){
         if (tiempo >= menorTiempoComprometidoModelo2){
            tiempoComprometidoPorMaquinaModelo2[minimoTC2] =  tiempoComprometidoPorMaquinaModelo1[minimoTC2] + tiempoDeAtencion();
            sumatoriaTiempoOciosoPorMaquinaModelo2[minimoTC2] = sumatoriaTiempoOciosoPorMaquinaModelo1[minimoTC2] + (tiempo - menorTiempoComprometidoModelo2);
         }else{
            tiempoLimiteDeRechazo = tiempo + condicionTiempoRechazo;
            if (tiempoComprometidoPorMaquinaModelo2[minimoTC2] < tiempoLimiteDeRechazo){
               //SE ACEPTA EL PEDIDO PORQUE NO SUPERA EL TIEMPO DE ENTREGA LIMITE
               sumatoriaTiempoEspera = sumatoriaTiempoEspera + (tiempoComprometidoPorMaquinaModelo2[minimoTC2] - tiempo);
               tiempoComprometidoPorMaquinaModelo2[minimoTC2] = tiempoComprometidoPorMaquinaModelo2[minimoTC2] + tiempoDeAtencion();
               //tiempoComprometidoPorMaquinaModelo2.push_back(minimoTC2,tiempoComprometidoPorMaquinaModelo2[minimoTC2] + tiempoPromedioModelo2);
               pedidosAceptados = pedidosAceptados + 1;
            }else{
               pedidosRechazados = pedidosRechazados + 1;
            }
         }
      }else{
         if (tiempo >= menorTiempoComprometidoModelo1){
            tiempoComprometidoPorMaquinaModelo1[minimoTC1] =  tiempoComprometidoPorMaquinaModelo1[minimoTC1] + tiempoDeAtencion();
            sumatoriaTiempoOciosoPorMaquinaModelo1[minimoTC1] = sumatoriaTiempoOciosoPorMaquinaModelo1[minimoTC1] + (tiempo - menorTiempoComprometidoModelo1);
         }else{
            tiempoLimiteDeRechazo = tiempo + condicionTiempoRechazo;
            if (tiempoComprometidoPorMaquinaModelo1[minimoTC1] < tiempoLimiteDeRechazo){
               //SE ACEPTA EL PEDIDO PORQUE NO SUPERA EL TIEMPO DE ENTREGA LIMITE
               sumatoriaTiempoEspera = sumatoriaTiempoEspera + (tiempoComprometidoPorMaquinaModelo1[minimoTC1] - tiempo);
               tiempoComprometidoPorMaquinaModelo1[minimoTC1] = tiempoComprometidoPorMaquinaModelo1[minimoTC1] + tiempoDeAtencion();
               //tiempoComprometidoPorMaquinaModelo2.push_back(minimoTC2,tiempoComprometidoPorMaquinaModelo2[minimoTC2] + tiempoPromedioModelo2);
               pedidosAceptados = pedidosAceptados + 1;
            }else{
               pedidosRechazados = pedidosRechazados + 1;
            }
         }
      }
   }

   if(pedidosAceptados==0){
      printf("pedidosAceptados fault (es 0)  \n");
   }else{
      promedioTiempoDeEspera = sumatoriaTiempoEspera / pedidosAceptados;
   }

   porcentajePedidosRechazados = pedidosRechazados * 100 / pedidosTotales;

   std::wcout << L"----------------------------------------------------------" << std::endl;
   std::wcout << L"INFORMACION DE LA SIMULACION :" << std::endl;
   std::wcout << L"" << std::endl;
   std::wcout << L"CANTIDAD DE MAQUINAS MODELO 1: " << cantidadDeMaquinasModelo1 << std::endl;
   std::wcout << L"CANTIDAD DE MAQUINAS MODELO 2: " << cantidadDeMaquinasModelo2 << std::endl;

   std::wcout << L"PEDIDOS TOTALES : " << pedidosTotales << std::endl;
   std::wcout << L"PEDIDOS ACEPTADOS : " << pedidosAceptados << std::endl;
   std::wcout << L"PEDIDOS RECHAZADOS : " << pedidosRechazados << std::endl;
   std::wcout << L"PROMEDIO DE TIEMPO DE ESPERA : " << promedioTiempoDeEspera << std::endl;

   std::wcout << L"PORCENTAJE DE PEDIDOS RECHAZADOS : " << porcentajePedidosRechazados << L" %" << std::endl;

   for(int i=0; i<cantidadDeMaquinasModelo1; i++){
      std::wcout << L"PORCENTAJE DE TIEMPO OCIOSO DE LA MAQUINA " << i+1 << " MODELO 1: "<< (sumatoriaTiempoOciosoPorMaquinaModelo1[i]*100)/tiempo << L" %" << std::endl;
   }

   for(int i=0; i<cantidadDeMaquinasModelo2; i++){
      std::wcout << L"PORCENTAJE DE TIEMPO OCIOSO DE LA MAQUINA " << i+1 << " MODELO 2: "<< (sumatoriaTiempoOciosoPorMaquinaModelo2[i]*100)/tiempo << L" %" << std::endl;
   }
}

int validarHoraLaboral(int tiempo){
   int DL = 0;
   if ((tiempo % 1440) >= 480 && (tiempo % 1440 <= 960)){
      DL = 1;
   }
   return DL;
}

int intervaloDeArribo1(){
   return (int)randomENTRE(2,6);
}

int intervaloDeArribo2(){
   return (int)randomENTRE(1,8);
}
/*-------------------FUNCION TEMPORALMENTE  MODIFICADA--------------------
int intervaloDeArribo1()
{
   while (true)
   {
      double random1 = random();
      double random2 = random();
      double x1 = 15 + 5 * random1; //15 es valor minimo de la fpd y 5 resta de valor maximo menos minimo (20-15)
      double y1 = COTA_SUPERIOR_FDP1 * random2;
      double referenciaParaValidarSiEsNumeroProbable = fdpIA1(x1);
      if (y1 <= referenciaParaValidarSiEsNumeroProbable)
      {
         return static_cast<int>(x1);
      }
   }
}

int intervaloDeArribo2(){
   while (true)
   {
      double random1 = random();
      double random2 = random();
      double x1 = 30 + 10 * random1; //15 es valor minimo de la fpd y 5 resta de valor maximo menos minimo (20-15)
      double y1 = COTA_SUPERIOR_FDP2 * random2;
      double referenciaParaValidarSiEsNumeroProbable = fdpIA2(x1);
      if (y1 <= referenciaParaValidarSiEsNumeroProbable)
      {
         return static_cast<int>(x1);
      }
   }
}*/

int tiempoDeAtencion(){
   return (int)randomENTRE(20,28);
}

/*      -------------TIEMPO DE ATENCION MODIFICADO TEMPORALMENTE------------------
int tiempoDeAtencion()
{
   while (true)
   {
      double random1 = random();
      double random2 = random();
      double x1 = 75 + 10 * random1; //15 es valor minimo de la fpd y 5 resta de valor maximo menos minimo (20-15)
      double y1 = COTA_SUPERIOR_TIEMPO_ATENCION * random2;
      double referenciaParaValidarSiEsNumeroProbable = fdpTA(x1);
      if (y1 <= referenciaParaValidarSiEsNumeroProbable)
      {
         return static_cast<int>(x1);
      }
   }
}*/

double random(void){
   return rand();
}

double randomENTRE(int a, int b){
   //N??mero aleatorios entre 250 y 420:    num=250+rand()%(421-250);
   //De forma general es  variable = limite_inferior + rand() % (limite_superior +1 - limite_inferior) ;
   double numero = a +rand()%(b-a);
   return numero;
}


double fdpIA1(double x1){
   double gama = 14.794;
   double beta = 1.611;
   double alfa = 2.778;
   double alfaMenosUno = alfa - 1;
   double k = 0.505;
   return (pow(((x1 - gama) / beta),alfaMenosUno) * alfa * k) / (pow((pow(((x1 - gama) / beta),alfa)) + 1,k + 1) * beta);
}



double fdpIA2(double x){
   double alfa = 36.576;
   double beta = 1.898;
   double pi = 3.14;
   return pow((pow(((x - alfa) / beta),2) + 1) * beta * pi,-1);
}

double fdpTA(double x){
   double alfa = 21.707;
   double beta = 0.862;
   double pi = 3.14;
   return pow((pow(((x - alfa) / beta),2) + 1) * beta * pi,-1);
}

int obtenerMinimoTC1(int cantidadDeMaquinasModelo1, std::vector<int> &tiempoComprometidoPorMaquinaModelo1){
   int minimo = 0;
   for (int i = 1 ; i < cantidadDeMaquinasModelo1 ; i++){
      if (tiempoComprometidoPorMaquinaModelo1[i] < tiempoComprometidoPorMaquinaModelo1[minimo]){
         minimo = i;
      }
   }
   return minimo;
}


int obtenerMinimoTC2(int cantidadDeMaquinasModelo2, std::vector<int> &tiempoComprometidoPorMaquinaModelo2){
   int minimo = 0;
   for (int i = 1 ; i < cantidadDeMaquinasModelo2 ; i++){
      if (tiempoComprometidoPorMaquinaModelo2[i] < tiempoComprometidoPorMaquinaModelo2[minimo]){
         minimo = i;
      }
   }
   return minimo;
}
