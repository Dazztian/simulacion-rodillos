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


static /*constexpr*/ double COTA_SUPERIOR_FDP1 = 0.32;
static /*constexpr*/ double COTA_SUPERIOR_FDP2 = 0.18;
static /*constexpr*/ double COTA_SUPERIOR_TIEMPO_ATENCION = 0.4;


int intervaloDeArribo1(void);
int validarHoraLaboral(int dt);
//nt obtenerDia(int t ):

int obtenerMinimoTC1(int cantidadDeMaquinasModelo1, std::vector<int> &tiempoComprometidoPorMaquinaModelo1);
int obtenerMinimoTC2(int cantidadDeMaquinasModelo2, std::vector<int> &tiempoComprometidoPorMaquinaModelo2);


int random(void);
int tiempoDeAtencion(void);

double fdpIA1(double x1);
double fdpIA2(double x);
double fdpTA(double x);

double randomENTRE(int a, int b);

int main(int argc, char** argv)
{
   //CONDICIONES INICIALES :
   int cantidadDeMaquinasModelo1 = 1;
   int cantidadDeMaquinasModelo2 = 1;

   int tiempo = 0;
   int tiempoFinal =  10; // 40320; // 1 Mes
   int tiempoProximaLlegada = 0;
   int tiempoLimiteDeRechazo = 72; // 6 horas
   int condicionTiempoRechazo = 360;
   int intervaloEntreArribos = 0;
   int menorTiempoComprometidoModelo1 = 0;
   int menorTiempoComprometidoModelo2 = 0;
   // int menorTiempoComprometido=0;
   // int tiempoPromedioModelo2 = 65;

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
   //---------------------HASTA ACA BORRAR-----------------------------------------------------------------------------------------------------------------------

   for (int i = 0 ; i < cantidadDeMaquinasModelo1 ; i++){
      tiempoComprometidoPorMaquinaModelo1[i] = 0;
      // printf("numero: %d \n", tiempoComprometidoPorMaquinaModelo1[i] );
   }

   for (int i = 0 ; i < cantidadDeMaquinasModelo2 ; i++){
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
      minimoTC1 = obtenerMinimoTC1(cantidadDeMaquinasModelo1, tiempoComprometidoPorMaquinaModelo1);  //debuelve un entero

      if(cantidadDeMaquinasModelo2>=1){ //SI Hay AL MENOS UNA MAQUINA NUEVA de model MR-8
         minimoTC2 = obtenerMinimoTC2(cantidadDeMaquinasModelo2, tiempoComprometidoPorMaquinaModelo2);
      }

      menorTiempoComprometidoModelo1 = tiempoComprometidoPorMaquinaModelo1[minimoTC1];
      menorTiempoComprometidoModelo2 = tiempoComprometidoPorMaquinaModelo2[minimoTC2];

      pedidosTotales = pedidosTotales + 1;

      if (menorTiempoComprometidoModelo1<=menorTiempoComprometidoModelo2){
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
      }else{
         if (tiempo >= menorTiempoComprometidoModelo2){
            tiempoComprometidoPorMaquinaModelo2[minimoTC2] =  tiempoComprometidoPorMaquinaModelo1[minimoTC2] + tiempoDeAtencion();
            sumatoriaTiempoOciosoPorMaquinaModelo2[minimoTC2] = sumatoriaTiempoOciosoPorMaquinaModelo1[minimoTC2] + (tiempo - menorTiempoComprometidoModelo2);
         }else{
            //tiempoLimiteDeRechazo = tiempo + condicionTiempoRechazo;
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
      }
   }

   if(pedidosTotales==0){
      printf("pedidosTotales fault (es 0)  \n");
   }else{
      promedioTiempoDeEspera = sumatoriaTiempoEspera / pedidosTotales;
   }

   porcentajePedidosRechazados = pedidosRechazados * 100 / pedidosTotales;

   std::wcout << L"----------------------------------------------------------" << std::endl;
   std::wcout << L"INFORMACION DE LA SIMULACION :" << std::endl;
   std::wcout << L"" << std::endl;
   std::wcout << L"CANTIDAD DE MAQUINAS MODELO 1: " << cantidadDeMaquinasModelo1 << std::endl;

   std::wcout << L"PEDIDOS TOTALES : " << pedidosTotales << std::endl;
   std::wcout << L"PEDIDOS ACEPTADOS : " << pedidosAceptados << std::endl;
   std::wcout << L"PEDIDOS RECHAZADOS : " << pedidosRechazados << std::endl;
   std::wcout << L"PROMEDIO DE TIEMPO DE ESPERA : " << promedioTiempoDeEspera << std::endl;

   std::wcout << L"PORCENTAJE DE PEDIDOS RECHAZADOS : " << porcentajePedidosRechazados << L" %" << std::endl;

}

int validarHoraLaboral(int tiempo){
   int DL = 0;

   if ((tiempo % 1440) >= 480 && (tiempo % 1440 <= 960)){
      DL = 1;
   }

   return DL;
}


int intervaloDeArribo1(){
   double x1;
   while (true){
      int random1 = random();
      int random2 = random();
      x1 = random1 % 5 + 15; //15 es valor minimo de la fpd y 5 resta de valor maximo menos minimo (20-15)
      double y1 = COTA_SUPERIOR_FDP1 * random2;
      double referenciaParaValidarSiEsNumeroProbable = fdpIA1(x1);
      if (y1 <= referenciaParaValidarSiEsNumeroProbable){
         break;
      }
   }
   return static_cast<int>(x1);
}

int random(void){
   return rand();
}

double randomENTRE(int a, int b){
   //Número aleatorios entre 250 y 420:    num=250+rand()%(421-250);
   // De forma general es  variable = limite_inferior + rand() % (limite_superior +1 - limite_inferior) ;
   double numero = a + rand()%(b-a);
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

int intervaloDeArribo2(){
   double x1;
   while (true){
      int random1 = random();
      int random2 = random();
      x1 = random1 % 10 + 30; //15 es valor minimo de la fpd y 5 resta de valor maximo menos minimo (20-15)
      double y1 = COTA_SUPERIOR_FDP2 * random2;
      double referenciaParaValidarSiEsNumeroProbable = fdpIA2(x1);
      if (y1 <= referenciaParaValidarSiEsNumeroProbable){
         break;
      }
   }
   return static_cast<int>(x1);
}

double fdpIA2(double x){
   double alfa = 36.576;
   double beta = 1.898;
   double pi = 3.14;
   return pow((pow(((x - alfa) / beta),2) + 1) * beta * pi,-1);
}

int tiempoDeAtencion(){
   double x1;
   while (true){
      int random1 = random();
      int random2 = random();
      x1 = 10 % random1 + 75; //15 es valor minimo de la fpd y 5 resta de valor maximo menos minimo (20-15)
      double y1 = COTA_SUPERIOR_TIEMPO_ATENCION * random2;
      double referenciaParaValidarSiEsNumeroProbable = fdpTA(x1);
      if (y1 <= referenciaParaValidarSiEsNumeroProbable){
         break;
      }
   }
   return static_cast<int>(x1);
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
      if (tiempoComprometidoPorMaquinaModelo2[i] < tiempoComprometidoPorMaquinaModelo2[minimo])
      {
         minimo = i;
      }
   }
   return minimo;
}
