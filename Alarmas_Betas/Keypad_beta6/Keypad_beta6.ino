#include <Password.h> //Incluimos la libreria Password
#include <Keypad.h> //Incluimos la libreria Keypad
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
Password password = Password("123456");  //Definimos el Password
Password puk = Password("654321");  //Definimos el puk
int dlugosc = 6;//Largo del Password
int estado = 0; //estados de la alarma desactivada 0/ activada 1
int pinpul = 0;
int buzzer = 10; //Creamos las Variables de salida
int z1 = 12 ;
int vz1 = 0;
int z2 = 13;
int vz2 = 0;
int bocina = 11;
int ilosc; //Numero de Clicks
int npasse; //Numeros de veces que se envio al pass a comprobar
const byte ROWS = 4; // Cuatro Filas
const byte COLS = 4; // Cuatro Columnas

// Definimos el Keymap
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = { 9, 8, 7, 6 }; // Conectar los keypads ROW1, ROW2, ROW3 y ROW4 a esos Pines de Arduino.
byte colPins[COLS] = { 5, 4, 3, 2, }; // Conectar los keypads COL1, COL2, COL3 y COL4 a esos Pines de Arduino.
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
void setup() {
  pinMode(buzzer, OUTPUT);
  pinMode(bocina, OUTPUT);
  pinMode(12, INPUT);
  pinMode(13, INPUT);
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("< ALARMA >");
  lcd.setCursor(3, 1);
  lcd.print("Bienvenido");
  Serial.begin(9600);
  keypad.addEventListener(keypadEvent);
}
void loop() {
  keypad.getKey();
alarmas();
}
void keypadEvent(KeypadEvent eKey) {
  switch (keypad.getState()) {
    /*case IDLE:
      delay(5000);
      alarmas();
      break;*/
    case PRESSED://Cada vez que se preciona una tecla suena el buzzer
      for (int i = 1; i <= 1; i++ ) {
        digitalWrite(buzzer, HIGH);
        delay(100);
        digitalWrite(buzzer, LOW);
        delay(50);
      }
      switch (eKey) {
        case '*'://al precionar * se envia a verificar la pass
          if (npasse >= 3) {
            checkpuk();
          }
          else {
            checkPassword();
          }
          break;
        case '#': //borrar la pass mientras se escribe
          if (npasse >= 3) {
            puk.reset();
            imppasslcd();
            ilosc = 0;
          }
          else {
            password.reset();
            imppasslcd();
            ilosc = 0;
          }
          break;
        case 'A'://activa vz1/desactiva vz1
          switch(vz1){
            case 0:
              vz1=1;lcdzonas();break;
            case 1:
              vz1=0;lcdzonas();break;
            }
          break;
        case 'B'://activa vz1/desactiva vz1
          switch(vz2){
            case 0:
              vz2=1;lcdzonas();break;
            case 1:
              vz2=0;lcdzonas();break;
            }
          break;
        default://si se preciona cualquier numero
          if (npasse >= 3) {
            ilosc ++;
            puk.append(eKey);
          }
          else {
            ilosc ++;
            password.append(eKey);
          }
      }
  }
  /////////////////////////MOSTRAR PASSWORD EN LCD//////////////////////////
  switch (ilosc) { //depende cuantas veces se preciono
    case 1:
      imppasslcd();
      lcd.setCursor(6,1);
      lcd.print("*_");
      break;
    case 2:
      imppasslcd();
      lcd.setCursor(6,1);
      lcd.print("**_");
      break;
    case 3:
      imppasslcd();
      lcd.setCursor(6,1);
      lcd.print("***_");
      break;
    case 4:
      imppasslcd();
      lcd.setCursor(6,1);
      lcd.print("****_");
      break;
    case 5:
      imppasslcd();
      lcd.setCursor(6,1);
      lcd.print("*****_");
      break;
    case 6:
      imppasslcd();
      lcd.setCursor(6,1);
      lcd.print("******");
      ilosc = 0;
      break;
  }
  ///////////////////////MOSTRAR PASSWORD EN LCD//////////////////////////
}
void checkPassword() {
  if (password.evaluate()) { //si la contraseña es correcta
    buzzerb();
    npasse = 0;//resetea al numero de veces que se envio la pass incorrectamente
    password.reset();//resetea el password para que no quede en memoria
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("<PIN CORRECTO>");//muestra que el pin es correcto
    delay(1000);//espera 2 segundo prendido
    alarma();
  }
  else{ //SI LA CONTRASEÑA ES INCORRECTA
    buzzerm();
    npasse ++;//suma uno al contador de contraseña incorrecta
    password.reset();
    switch (npasse) { //depende cuantas veces llego a ser incorrecta la clave
      case 1:
        passinc();//muestra el herror en el lcd
        lcd.print("Intento numero 1");
        delay(100);
        break;
      case 2:
        passinc();//muestra el herror en el lcd
        lcd.print("Intento numero 2");
        delay(100);
        break;
      case 3:
        passinc();//muestra el herror en el lcd
        lcd.print("Intento numero 3");
        delay(1000);
        lcd.clear();
        mostrarpuk();
        break;
    }
  }
}
void checkpuk() {
  if (puk.evaluate()) { //si el es correcta
    buzzerb();
    npasse = 0;//resetea al numero de veces que se envio la pass incorrectamente
    puk.reset();//resetea el password para que no quede en memoria
    password.reset();
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("<PUK CORRECTO>");//muestra que el puk es correcto  
    delay(1000);
    alarma();
  }
  else {
    buzzerm();
    puk.reset();
    password.reset();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("<PUK INCORRECTO>");
    lcd.setCursor(0, 1);
    lcd.print("Intente de nuevo");
  }
}
void buzzerm(){
  for (int i = 1; i <= 1; i++ ) { //SUENA EL BUZZER  1 VEZ
    digitalWrite(buzzer, HIGH);
    delay(400);
    digitalWrite(buzzer, LOW);
    delay(100);}
}
void buzzerb(){
  for (int i = 1; i <= 3 ; i++ ) { //hace sonar 3 veces el buzzer
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(50);}
}
void imppasslcd() {
  lcd.clear();
  lcd.setCursor(0,1);
  if (npasse >= 3) {lcd.print("Puk:");}
  else {lcd.print("Pin:");}
  if (npasse >= 3) {mostrarpuk();}
  else if(estado==1){lcdalarmas();}
  else if(pinpul==1){lcdalarmaa();}
  else if(pinpul==0){lcdalarmad();}
}
void lcdalarmaa() {
  lcd.setCursor(0, 0);
  lcd.print("ALARMA  ACTIVADA");
  lcd.setCursor(0, 1);
}
void lcdalarmad() {
  lcd.setCursor(0, 0);
  lcd.print("ALARMA  INACTIVA");
  lcd.setCursor(0, 1);
}
void lcdalarmas() {
  lcd.setCursor(0, 0);
  lcd.print("INTRUSO  CAPTADO");
  lcd.setCursor(0, 1);
}
void passinc() {
  lcd.setCursor(0, 0);
  lcd.print("<PIN INCORRECTO>");
  lcd.setCursor(0, 1);
}
void mostrarpuk(){
  lcd.setCursor(2, 0);
  lcd.print("INGRESE PUK");
  lcd.setCursor(0, 1);
}
void alarma() {
  pinpul++;
  switch(pinpul){
    case 1:
        lcdzonas();
        lcdalarmaa();
        break;
    case 2:
      lcdzonas();
      lcdalarmad();
      digitalWrite(bocina, LOW);
      pinpul=0;
      estado=0;
      break;  
  }
}
void alarmas(){
  if(vz1==1 && vz2==0){
    if(pinpul==1 && digitalRead(z1)==HIGH){
      digitalWrite(bocina, HIGH);
      lcdalarmas();
      estado=1;
    }
  }
  if(vz1==0 && vz2==1){
    if(pinpul==1 && digitalRead(z2)==HIGH){
      digitalWrite(bocina, HIGH);
      lcdalarmas();
      estado=1;
    }
  }
  if(vz1==1 && vz2==1){
    if(pinpul==1 && (digitalRead(z2)==HIGH || digitalRead(z1)==HIGH)){
      digitalWrite(bocina, HIGH);
      lcdalarmas();
      estado=1;
    }
  }
  else if(vz1==0 && vz2==0){
    if(pinpul==1 && (digitalRead(z2)==HIGH || digitalRead(z1)==HIGH)){
      digitalWrite(bocina, LOW);
      lcdalarmas();
      estado=1;
    }
  }
}
void lcdzonas(){
  if(vz1==0 && vz2==0){
    lcd.setCursor(0,1);
    lcd.print("Z1:Off -  Z2:OFF");
  }
  if(vz1==1 && vz2==0){
    lcd.setCursor(0,1);
    lcd.print("Z1:ON  -  Z2:OFF");
    Serial.println(vz1);
    Serial.println(vz2);
    }
  if(vz1==0 && vz2==1){
    lcd.setCursor(0,1);
    lcd.print("Z1:Off -  Z2:ON ");
    }
  if(vz1==1 && vz2==1){
    lcd.setCursor(0,1);
    lcd.print("Z1:ON  -  Z2:ON ");
    }
}

















/*pinpul++;
  while(pinpul=1){ //si la alarma estaba activada y se ingresa el codigo
    lcdalarmaa();
    if(digitalRead(z1)==HIGH){
      lcdalarmas();
      delay(2000);
      digitalWrite(bocina, HIGH);
      pinpul=1;
      }
  if(keypad.getState()==PRESSED){
    break;
  }
  
  }
  while(pinpul=2){//si la alarma estaba sonando y se ingresa el codigo
    lcdalarmad();//se desactiva
    pinpul = 0; //las veces que se puso el pin pasa a 0
  }
  */





 /* switch (estado) {
    case 0://que hace cuando la alarma esta desactivada
      digitalWrite(bocina, LOW);
      break;
    case 1://que hace cuando la alarma esta activada
      if(digitalRead(z1)==HIGH){
        estado = 2;
      }
      else{
        digitalWrite(bocina, LOW);
      }
      //EL ESTADO SE TIENE QUE PONER EN 2 CUANDO SE ACTIVA EL SENSOR
      break;
    case 2://que hace si la larma esta sonando
      digitalWrite(bocina, HIGH);
      pinpul = 2; //para cuando se ingrese el codigo correcto
      lcdalarmas();
      break;
  }
}
*/







/*void lcdalarma() {
  switch (estado) {
    case 0: //si la alarma estaba desactivada y se ingresa el codigo
      lcdalarmaa(); //se activa
      break;
    case 1: //si la alarma estaba activada o sonando y se ingresa el codigo
      lcdalarmad();//se desactiva
      break;
  }
}*/
