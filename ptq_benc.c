/*

Encabezado HEX												
4D 54 68 64 						MThd                  									0-3 	
00 00 00 06 						chunk length											4-7
00 00 								format 0												8-9
00 01 								one track												10-11
00 60 								96 ticks per quarter-note								12-13



4D 54 72 6B 						MTrk													14-17
HASTA ACA ENCABEZADO_1

ACA TAMAÑO CHUNK
ww xx yy zz 						chunk length (a determinar)								18-21


ACA EMPIEZA ENCABEZADO 2
00 FF 58 04 04 02 18 08 			time signature		4/2^4 								22-29
00 FF 51 03 07 A1 20 				tempo		0x07 0xA1 0x20 = 5e5 esto hay q cambiar		30-36
1
acá arrancan las notas, por ejemplo
00 90 30 60                         delta_t; note on channel 0; nota 0x30; velocidad 60
note off:
00 80 30 40
00 FF 2F 00 						end of track


*/

unsigned char encabezado_1[]={0x4D, 0x54, 0x68, 0x64, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x01, 0x03, 0xC0,0x4D, 0x54, 0x72, 0x6B};
unsigned char tamano_chunk[]={0x00, 0x00, 0x09, 0x03}; 
unsigned char encabezado_2[]={0x00, 0xFF, 0x58, 0x04, 0x04, 0x02, 0x18, 0x08, 0x00, 0xFF, 0x51, 0x03};
unsigned char tempo_usec[3];
unsigned char notas[]={21, 23, 24, 26, 28, 29, 31, 33, 35, 36, 38, 40, 41, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, 60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79, 81, 83, 84, 86, 88, 89, 91, 93, 95, 96, 98, 100, 101, 103, 105, 107, 108};
unsigned char notas_on[2080];
unsigned char notas_off[208];
unsigned char fin_archivo[]={0x00, 0xFF, 0x2F, 0x00};

int microsegundos=1000000;
int terminar=0;
int intento=1;
int t_min_sin_falla=1000000;
int t_max_con_falla=0;

//#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{   
    printf("\nInicio de la prueba\n");
	printf("\nPara cancelar presione ctrl + c\n");
	//getch();
	while (terminar==0)
{	
    float t_milis=(float)microsegundos/10000;
    printf("tiempo entre notas: %4.2f milisegundos\n",t_milis);

    tempo_usec[2]=microsegundos;
    tempo_usec[1]=microsegundos>>8;
    tempo_usec[0]=microsegundos>>16;
	
//	printf("Hex 0: 0x%X Hex 1: 0x%X Hex 2: 0x%X\n ",tempo_usec[0],tempo_usec[1],tempo_usec[2]);
	

    int i=0; // posicion en el array de eventos midi
    int j=0; //posición en el array de notas
	while(i<2080)
	{
		if(i%4==0) {notas_on[i]=0x60;} // delay de 0x60 = 96
		else if (i%4==1) {notas_on[i]=0x90;} // note on canal 0
		else if (i%4==2) {
					       notas_on[i]=notas[j]; // copio las notas que voy a ir poniendo 
						   j++;                  // y avanzo un lugar
						   if (j==52) {j=0;}	 // si me paso dle array vuelvo					
						 }
	    else {notas_on[i]=127;}                // la velocidad de la nota es 127	
		i++;
	}
	int l; //posicion en el array de notes off
	j=51;   //reseteo la posicion en las notas
	while(l<208)
	{
		if(l%4==0) {notas_off[l]=0x60;} // delay de 0x60 = 96
		else if (l%4==1) {notas_off[l]=0x80;} // note off canal 0
		else if (l%4==2) {
					       notas_off[l]=notas[j]; // copio las notas que voy a ir poniendo 
						   j--;                  // y avanzo un lugar		
						 }
	    else {notas_off[l]=100;}                // la velocidad de apagado es 100 (arbitrario)
		l++;
	}
	// para calcular el tamaño tengo: 2080 de los note on, 208 de los notes off, 4 del fin de archivo, 3 de los microsegundos = 1255 caracteres, pero además tengo los 12 del encabezado 2 = 1267
	// 2307 en HEX es 04F3
	//int a= sizeof(tamano_chunk);
	//printf("%i",a);
	
	FILE * archivo;
	archivo = fopen("miditest.mid","w");
	
	for (int m=0; m<sizeof(encabezado_1); m++) { fprintf(archivo,"%c",encabezado_1[m]);	}
	for (int n=0; n<sizeof(tamano_chunk); n++) { fprintf(archivo,"%c",tamano_chunk[n]);	}
	for (int o=0; o<sizeof(encabezado_2); o++) { fprintf(archivo,"%c",encabezado_2[o]);	}
	for (int p=0; p<sizeof(tempo_usec); p++) { fprintf(archivo,"%c",tempo_usec[p]);	}
	for (int q=0; q<sizeof(notas_on); q++) {fprintf(archivo,"%c",notas_on[q]);}
	for (int r=0; r<sizeof(notas_off); r++) {fprintf(archivo,"%c",notas_off[r]);}
	for (int s=0; s<sizeof(fin_archivo); s++) {fprintf(archivo,"%c",fin_archivo[s]);}
	fclose(archivo);
	if (argc==1)
	{   char comando[128];
        char pianoteq[]="./Pianoteq";
		char opciones[]=" --headless --play-and-quit --midi miditest.mid";
		char comilla[]="\"";
	//	printf("la comilla es en caracteres %c y en nro %i\n",comilla[0],comilla[0]);
		strcpy(comando,comilla);
		strcat(comando,pianoteq);
		strcat(comando,comilla);
		strcat(comando,opciones);
		printf("el comando es: %s; tamaño comando: %i\n",comando,strlen(comando));
		system(comando);
	}
	
	
	 char respuesta[8];
	 
	 printf("Intento nro %i\n",intento);
	 printf("Fallo el audio? s/n, para terminar t\n");
	 scanf("%s",respuesta);
      
      
      if (respuesta[0]==121)
        { 
		   intento++;
		   t_max_con_falla=microsegundos;
		   microsegundos=0.5*(t_max_con_falla+t_min_sin_falla);
        }
      else if (respuesta[0]==110) 
       {      
         intento++;
         t_min_sin_falla=microsegundos;
         microsegundos=0.5*(t_max_con_falla+t_min_sin_falla);
       }
	 else if (respuesta[0]==116) 
	 {
		float t_final=(float)t_min_sin_falla/10000;
		printf("Iteraciones totales %i, delta tiempo final: %4.2f\n",intento,t_final);
		terminar++; 
		
		
	 }
	
}
	
	
	
	return 0;
}
