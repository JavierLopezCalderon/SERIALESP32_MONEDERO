#define relay 32

int MensajeMonitorSerial2;
String MensajeString;
double MontoAcumulado;
double Dinero_Acumulado;
String Dinero_acumulado_String;
int Tarifa = 5;
double Monedas[]={1,2,5,10,20,0.5,0.2,0.1};
int Ingresos_Usuarios;

void setup()
{
  Serial.begin(9600); 
  Serial2.begin(9600); 
  pinMode(relay, OUTPUT);
}

void IdentificacionDeMonedas()
{
     switch (MensajeMonitorSerial2){
         case 193://C1
         MontoAcumulado = MontoAcumulado + Monedas[0];
         Dinero_Acumulado = Dinero_Acumulado + Monedas[0];
         break;
         case 194://C2
         MontoAcumulado = MontoAcumulado + Monedas[1];
         Dinero_Acumulado = Dinero_Acumulado + Monedas[1];
         break;
         case 197://C5
         MontoAcumulado = MontoAcumulado + Monedas[2];
         Dinero_Acumulado = Dinero_Acumulado + Monedas[2];
         break;
         case 202://C10
         MontoAcumulado = MontoAcumulado + Monedas[3];
         Dinero_Acumulado = Dinero_Acumulado + Monedas[3];
         break;
         case 203://C20
         MontoAcumulado = MontoAcumulado + Monedas[4];
         Dinero_Acumulado = Dinero_Acumulado + Monedas[4];
         break;
         case 204://C0.5
         MontoAcumulado = MontoAcumulado + Monedas[5];
         Dinero_Acumulado = Dinero_Acumulado + Monedas[5];
         break;
         case 205://C0.2
         MontoAcumulado = MontoAcumulado + Monedas[6];
         Dinero_Acumulado = Dinero_Acumulado + Monedas[6];
         break;
         case 206://C0.1
         MontoAcumulado = MontoAcumulado + Monedas[7];
         Dinero_Acumulado = Dinero_Acumulado + Monedas[7];
         break;
     }
     Dinero_acumulado_String = Dinero_Acumulado;
     
}

void AccinamientoRelevador(){
  if(MontoAcumulado >= Tarifa){
      Ingresos_Usuarios++;
      
      digitalWrite(relay, HIGH);
      MontoAcumulado = 0;
      delay(4000); 
      digitalWrite(relay, LOW);
  }else{
      digitalWrite(relay, LOW);
  }
}


void loop()
{
  if (Serial2.available())
  {       
    MensajeMonitorSerial2 = Serial2.read();
    //MensajeString = String(MensajeMonitorSerial2, HEX);
    Serial.print("Mensaje: ");
    Serial.println(MensajeMonitorSerial2, HEX);
    IdentificacionDeMonedas();
    Serial.print("Acumulado: "); 
    Serial.println(MontoAcumulado);
    AccinamientoRelevador();
  } 
}
