#include "Nextion.h"  //incluir la libreria


//declaro las clases de los objetos que voy a utilizar
NexNumber largoPlaca = NexNumber(1, 6, "n0");
NexNumber anchoPlaca = NexNumber(1, 7, "n1");
NexNumber cantidadAgujeros = NexNumber(1, 8, "n2");
NexNumber diametroAgujeros = NexNumber(1, 9, "n3");
//NexText Bandera = NexText(4, 4, "t0");
NexNumber x1_hmi = NexNumber(2, 6, "n0");
NexNumber y1_hmi = NexNumber(2, 7, "n1");
NexNumber x2_hmi = NexNumber(2, 8, "n2");
NexNumber y2_hmi = NexNumber(2, 9, "n3");
NexNumber x3_hmi = NexNumber(2, 19, "n4");
NexNumber y3_hmi = NexNumber(2, 20, "n5");
NexNumber x4_hmi = NexNumber(2, 21, "n6");
NexNumber y4_hmi = NexNumber(2, 22, "n7");

NexNumber x5_hmi = NexNumber(4, 6, "n0");  
NexNumber y5_hmi = NexNumber(4, 7, "n1");
NexNumber x6_hmi = NexNumber(4, 8, "n2");
NexNumber y6_hmi = NexNumber(4, 9, "n3");
NexNumber x7_hmi = NexNumber(4, 20, "n4");
NexNumber y7_hmi = NexNumber(4, 21, "n5");
NexNumber x8_hmi = NexNumber(4, 22, "n6");
NexNumber y8_hmi = NexNumber(4, 23, "n7");

//NexText estadoSensor = NexText(7, 3, "n0");
//Declaracion de las clases de los botones

NexButton bguardar1 = NexButton(1, 11, "b1");
NexButton bguardar2 = NexButton(2, 28, "b3");
NexButton bguardar3 = NexButton(4, 28, "b3");
NexButton biniproceso = NexButton(5, 2, "b2");
NexButton bdetenerProceso = NexButton(7, 2, "b0");
NexButton brehacer = NexButton(8, 2, "b0");

NexPage pagina2 = NexPage(2,0,"page2");   //Coordenadas de los agujeros
NexPage pagina3 = NexPage(4,0,"page3");   //Coordenadas de los agujeros (2)
NexPage pagina4 = NexPage(5,0,"page4");   //Pantalla de proceso iniciado
NexPage pagina5 = NexPage(6,0,"page5");   //"Ingresar todos los datos correctamente"
NexPage pagina7 = NexPage(7,0,"page7");   //"Proceso iniciado"
NexPage pagina8 = NexPage(8,0,"page8");   //"Proceso terminado"
NexPage pagina10 = NexPage(10,0,"page10");//"Los valores sobrepasan el límite de la placa"

char buffer[100] = { 0 };

//----------------------------------------
//señal para RELE
int M1 = 2;  //Motor que hace subir el punzón
int M2 = 3;  //Motor que hace bajar el punzón
//SENSORES infrarojos
int sensorAlto = 4;
int sensorBajo = 7;
//driver1(x)
int PUL = 5;
int DIR = 6;
//driver2(y)
int PUL2 = 10;
int DIR2 = 11;
//Sensores de fin de carrera
int finCarrera1=8;  //Limite en X
int finCarrera2=9;  //Limite en Y

int LED = 13;

/*
Motor y
  HIGH -Y
  LOW +Y
Motor x
  HIGH -X
  LOW +X
*/

  
uint32_t ancho=0;
uint32_t largo=0;
uint32_t pasosRx=0;
uint32_t pasosRy=0;
uint32_t diametro=0;
uint32_t cantidad=0;          // numero de agujeros
uint32_t PX = 328;  //posicion x del punzon
uint32_t PY = 19;   //posicion y del punzon
long MPX=0;        //lo que el motor de pasos movera en x
long MPY=0;        //lo que el motor de pasos movera en y
uint32_t PR=800;         // pasos/revolucion del driver
uint32_t MMREVX=36;    // proporcion milimetros/revolucion en x para 800 SPP
uint32_t MMREVY=64; //proporcion mm/rev en y para 800 SPP
int i = 1;
uint32_t posiciones[8][2];//matriz para guardar los posiciones x,y de los 8 agujeros
bool inicioProceso = 0;  
int numAgujero=0;
long punzonx;
long punzony;
uint32_t pasos;
uint32_t pasos2;
bool interrupcion;
int distanciaSharp;
int xFinal=5;


//lista de eventos tactiles
NexTouch *nex_listen_list[] = {

  &bguardar1,
  &bguardar2,
  &bguardar3,
  &biniproceso,
  &bdetenerProceso,
  &brehacer,

  NULL
};
//----------------------------------------

//la funcion a llamar cuando presione el boton sumar
void parpadeo();
void prepararMaq();
int leerSharp();
void guardar1(void *ptr);
void guardar2(void *ptr);
void guardar3(void *ptr);
void iniproceso(void *ptr);
void stopProceso(void *ptr);
void rehacer(void *ptr);
void estados();


void setup() {
  nexInit();

  pinMode(PUL, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(PUL2, OUTPUT);
  pinMode(DIR2, OUTPUT);
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(finCarrera1,INPUT);
  pinMode(finCarrera2,INPUT);
  pinMode(sensorAlto, INPUT);
  pinMode(sensorBajo, INPUT);
  //pinMode(sharp, INPUT);
  pinMode(LED, OUTPUT);


  //digitalWrite(ENA, HIGH);
  digitalWrite(DIR,LOW);
  digitalWrite(DIR2,LOW);
   // Llama a la funcion cuando se presiona el boton

  bguardar1.attachPush(guardar1,&bguardar1);
  bguardar2.attachPush(guardar2,&bguardar2);
  bguardar3.attachPush(guardar3,&bguardar3);
  biniproceso.attachPush(iniproceso,&biniproceso);
  bdetenerProceso.attachPush(stopProceso,&bdetenerProceso);
  brehacer.attachPush(rehacer,&brehacer);  //Revisar funcionalidad

  digitalWrite(LED,LOW);
  digitalWrite(M2,HIGH);  //Inicialmente ambos desactivados en funcion de relé de 2 canales
  digitalWrite(M1,HIGH);
  //dbSerialPrintln("setup completado");
  //Serial.begin(9600);

  inicioProceso=0;
  interrupcion=0;
}


//Este código busca dar los estados de los 4 sensores para comprobar el correcto funcionamiento en conjunto de cada uno

void loop() {


  while(1){
    interrupcion=0;
    while(inicioProceso==0){         
      nexLoop(nex_listen_list);
    } 
  

    long dyrecorrida=0;
    long limiteY=50;

      //PREGUNTAR SI SE QUIERE HACER OTRAPLACA IGUAL
  
    int	b = true;//Bandera que indica un cambio en el orden de los datos, inicialmente Verdadera
    int ln = 7;
    uint32_t tempx, tempy;

    //BUCLE QUE SE EJECUTA MIENTRAS OCURRA UN CAMBIO DE POSICION EN LOS DATOS
    while(b){
      b=false;//Desactivando la bandera de cambio de datos
      //Recorreriendo todo el array
      for(int i=0; i < ln; i++){
        // Comparando el dato siguiente con el actual i
        if(posiciones[i+1][0]>posiciones[i][0]){ 
          b=true;//Hay un cambio de posicion
          tempx = posiciones[i][0];//Guardando el valor actual
          tempy = posiciones[i][1];
          //Intercambiando la posicion de los datos
          posiciones[i][0] = posiciones[i+1][0];
          posiciones[i+1][0] = tempx;
          posiciones[i][1] = posiciones[i+1][1];
          posiciones[i+1][1] = tempy;
        }
      }
    }
    //FIN DEL BUCLE DE ORDENAMIENTO

    //MOVIMENTO DE LOS MOTORES A LA POSICIÓN DE COLOCACIÓN DE PLACA
    //Motor en X
    MPX = PX - largo;  //desplazamiento en x(distancia del punto de referencia hasta el nuevo SC)
    pasos=(MPX*PR)/MMREVX;  //cálculo para determinar los pasos 
    for(int a=0;a<pasos;a++){
      digitalWrite(PUL,HIGH);
      delayMicroseconds(800);
      digitalWrite(PUL,LOW);
      delayMicroseconds(800);
    }

    delay(500);

    //Motor en y
    digitalWrite(DIR2,LOW);
    MPY = ancho - PY;
    dyrecorrida=MPY;
      
    pasos2=(MPY*PR)/MMREVY;  //cálculo para determinar los pasos
    if(dyrecorrida<limiteY){
      for(int b=0;b<pasos2;b++){
      digitalWrite(PUL2,HIGH);
      delayMicroseconds(1000);
      digitalWrite(PUL2,LOW);
      delayMicroseconds(1000);
      }
    }

    //INICIO DE ITERACION PARA LA POSICION EN LOS AGUJEROS
      MPX=0;
      MPY=0;
      punzonx=largo;
      punzony=ancho;
      for(numAgujero=0;numAgujero<cantidad;numAgujero++){    
        //movimiento en x
          MPX=punzonx-posiciones[numAgujero][0];
          punzonx=posiciones[numAgujero][0];
          pasos=(MPX*PR)/MMREVX;
          for(int a=0;a<pasos;a++){
            digitalWrite(PUL,HIGH);
            delayMicroseconds(800);// velocidad de 1.28 segundos por revolucion
            digitalWrite(PUL,LOW);
            delayMicroseconds(800);
          }
          delay(100);

        //movimiento en y
          MPY=punzony-posiciones[numAgujero][1];
          punzony=posiciones[numAgujero][1];
          dyrecorrida=dyrecorrida+MPY;
          if(MPY<0){
            MPY=abs(MPY);
            digitalWrite(DIR2,LOW);
          }
          else if(MPY>0){digitalWrite(DIR2,HIGH);}
          
          //Motor en y
          pasos2=(MPY*PR)/MMREVY;
          if((dyrecorrida<limiteY)&&(MPY!=0)){
            for(int a=0;a<pasos2;a++){
            digitalWrite(PUL2,HIGH);
            delayMicroseconds(1000);
            digitalWrite(PUL2,LOW);
            delayMicroseconds(1000);
            }
          }

          //Movimiento del punzón
          if (digitalRead(sensorBajo)==1){
          digitalWrite(M2,LOW); //el punzon baja                                                                                
            while(digitalRead(sensorBajo)==1){
              nexLoop(nex_listen_list);
              if(interrupcion==1){break;}
            }
            digitalWrite(M2,HIGH);// el punzon se detiene
          }
          if(interrupcion==1){break;}
          delay(100);

          if (digitalRead(sensorAlto)==0){
            digitalWrite(M1,LOW); //punzon sube
            while(digitalRead(sensorAlto)==0){
              nexLoop(nex_listen_list);
              if(interrupcion==1){break;}
            }
          }
            digitalWrite(M1,HIGH);   //...el punzón se detiene  
            if(interrupcion==1){break;}

            delay(100); //tiempo para que el punzón suba un poco más y el sensor superior no tenga estados imprecisos
            
      }

      //TERMINO DE LA ITERACION DE POSICIÓN Y PERFORADO

      //COMANDOS QUE BOTAN LA PLACA
      MPX=punzonx;
      pasos=(MPX*PR)/MMREVX;
      for(int a=0;a<pasos;a++){
        digitalWrite(PUL,HIGH);
        delayMicroseconds(800);// velocidad de 1.28 segundos por revolucion
        digitalWrite(PUL,LOW);
        delayMicroseconds(800);
      }

     
      inicioProceso=0;
      numAgujero=0;
      pagina8.show();     
       
  }
}
//FIN DEL VOID LOOP


//-----------------INICIO DE LAS FUNCIONES-----------------
void parpadeo(){
  digitalWrite(LED,HIGH);
  delay(300);
  digitalWrite(LED,LOW);
  delay(300);
}

//---------------------------------------------------------
void prepararMaq(){
  //Se pone el punzón en el punto alto
  if(digitalRead(sensorAlto)==0){
    digitalWrite(M1,LOW); //punzon sube
    while(digitalRead(sensorAlto)==0){
      nexLoop(nex_listen_list);
      if(interrupcion==1){
        digitalWrite(M1,HIGH); //punzon se detiene
        interrupcion=0;
        break;
      }      //sale del while cuando el sensor Alto detecta al punzon   
    }
    digitalWrite(M1,HIGH); //punzon se detiene
  }

  //Se pone el soporte en el limite inferior de Y
  if(digitalRead(finCarrera2)==0){
    digitalWrite(DIR2,HIGH);  //-Y
    while(digitalRead(finCarrera2)==0){
      digitalWrite(PUL2,HIGH);
      delayMicroseconds(800);
      digitalWrite(PUL2,LOW);
      delayMicroseconds(800);
    }
  }

  //Se pone el soporte en el limite izquierdo de X
  if(digitalRead(finCarrera1)==0){
    digitalWrite(DIR,HIGH);  //-X
    while(digitalRead(finCarrera1)==0){
      digitalWrite(PUL,HIGH);
      delayMicroseconds(600);
      digitalWrite(PUL,LOW);
      delayMicroseconds(600);
    }
    digitalWrite(DIR,LOW);  //+X
  }

}
//---------------------------------------------------------
/*
int leerSharp(){
  int n=15;
  long suma=0;
  for(int i=0;i<n;i++)
  {
    suma=suma+analogRead(sharp);
  }
  int prom=suma/n;
  int distancia= prom*15/562;
  return(distancia);
}
*/
//---------------------------------------------------------
void guardar1(void *ptr){
  largoPlaca.getValue(&largo);
  anchoPlaca.getValue(&ancho);
  cantidadAgujeros.getValue(&cantidad);
  diametroAgujeros.getValue(&diametro);  
   
  if((largo>10)&&(ancho>10)&&(cantidad>0)&&(diametro>1)){
   parpadeo();
   pagina2.show();
  }
}
//---------------------------------------------------------
void guardar2(void *ptr){
  x1_hmi.getValue(&posiciones[0][0]);
  y1_hmi.getValue(&posiciones[0][1]);
  x2_hmi.getValue(&posiciones[1][0]);
  y2_hmi.getValue(&posiciones[1][1]);
  x3_hmi.getValue(&posiciones[2][0]);
  y3_hmi.getValue(&posiciones[2][1]);
  x4_hmi.getValue(&posiciones[3][0]);
  y4_hmi.getValue(&posiciones[3][1]);
 
  if(posiciones[0][0]>largo || posiciones[1][0]>largo || posiciones[2][0]>largo || posiciones[3][0]>largo || posiciones[0][1]>ancho || posiciones[1][1]>ancho || posiciones[2][1]>ancho || posiciones[3][1]>ancho){
    pagina10.show();
  } else if(cantidad>4){
    pagina3.show();
  }else{
    prepararMaq();
    pagina4.show();
  }
}
//---------------------------------------------------------
void guardar3(void *ptr){
  x5_hmi.getValue(&posiciones[4][0]);
  y5_hmi.getValue(&posiciones[4][1]);
  x6_hmi.getValue(&posiciones[5][0]);
  y6_hmi.getValue(&posiciones[5][1]);
  x7_hmi.getValue(&posiciones[6][0]);
  y7_hmi.getValue(&posiciones[6][1]);
  x8_hmi.getValue(&posiciones[7][0]);
  y8_hmi.getValue(&posiciones[7][1]);
  
  if(posiciones[4][0]>largo || posiciones[5][0]>largo || posiciones[6][0]>largo || posiciones[7][0]>largo || posiciones[4][1]>ancho || posiciones[5][1]>ancho || posiciones[6][1]>ancho || posiciones[7][1]>ancho){
    pagina10.show();
  } else {
    prepararMaq();
    pagina4.show();
  }
}
//---------------------------------------------------------
void iniproceso(void *ptr) { 
  inicioProceso=1;
  numAgujero=0;
  //parpadeo();
    
  pagina7.show();
}
//---------------------------------------------------------
void stopProceso(void *ptr){
  interrupcion=1;
  inicioProceso=0;
  digitalWrite(M1,HIGH);
  digitalWrite(M2,HIGH);  
  pagina8.show();
}
//---------------------------------------------------------
void rehacer(void *ptr){
  for(int a=0;a<4;a++){parpadeo();}
  prepararMaq();
  pagina4.show();
}
//---------------------------------------------------------
void estados(){
  if(digitalRead(sensorAlto)==0){
    dbSerialPrint("SENSOR ALTO: Detecta    ");
  }else{
    dbSerialPrint("SENSOR ALTO: No detecta    ");
  }

  if(digitalRead(sensorBajo)==8){
    dbSerialPrint("SENSOR BAJO: Detecta (");
    dbSerialPrint(distancia_p);
    dbSerialPrint(")    ");
  }else{
    dbSerialPrint("SENSOR BAJO: No detecta (");
    dbSerialPrint(distancia_p);
    dbSerialPrint(")    ");
  }

  if(digitalRead(finCarrera1)==1){
    dbSerialPrint("SENSOR X: Detecta    ");
  }else{
    dbSerialPrint("SENSOR X: No detecta    ");
  }

  if(digitalRead(finCarrera2)==1){
    dbSerialPrintln("SENSOR Y: Detecta    ");
  }else{
    dbSerialPrintln("SENSOR Y: No detecta    ");
  }

}







