#include <Password.h> //Incluimos la libreria Password
#include <Keypad.h> //Incluimos la libreria Keypad
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
Password password = Password("123456");  //Definimos el Password
Password puk = Password("654321");  //Definimos el puk
int dlugosc = 6;//Largo del Password
int buzzer = 10; //Creamos las Variables de salida
int ilosc; //Numero de Clicks
int npasse; //Numeros de veces que se envio al pass a comprobar
const byte ROWS = 4; // Cuatro Filas
const byte COLS = 4; // Cuatro Columnas

// Definimos el Keymap
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = { 9,8,7,6 };// Conectar los keypads ROW1, ROW2, ROW3 y ROW4 a esos Pines de Arduino.
byte colPins[COLS] = { 5,4,3,2, };// Conectar los keypads COL1, COL2, COL3 y COL4 a esos Pines de Arduino.
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
void setup(){
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("< ALARMA >");
  lcd.setCursor(3,1);
  lcd.print("Bienvenido");
  delay(3000);
  lcd.noBacklight();
  Serial.begin(9600);
  keypad.addEventListener(keypadEvent);  
  pinMode(buzzer, OUTPUT);
}
void loop(){ keypad.getKey();}
void keypadEvent(KeypadEvent eKey){
  switch (keypad.getState()){
    case PRESSED://Cada vez que se preciona una tecla suena el buzzer
      lcd.backlight();
      for(int i = 1; i <= 1; i++ ){digitalWrite(buzzer, HIGH); delay(100); digitalWrite(buzzer, LOW); delay(50);}
    switch (eKey){
      case '*'://al precionar * se envia a verificar la pass
        if(npasse >= 3){
          checkpuk();
        }
          else{
          checkPassword();
          }
        break;
      case '#': //borrar la pass mientras se escribe
        if(npasse >=3){puk.reset();mostrarpuk();ilosc = 0;}
        else{
          password.reset();
          imppasslcd(); 
          ilosc = 0;
        }
        break;
      default://si se preciona cualquier numero
        if(npasse >=3){puk.append(eKey);ilosc ++;}
        else{
        ilosc ++;
        password.append(eKey);
        }
    }
  }
/////////////////////////MOSTRAR PASSWORD EN LCD////////////////////////// 
  switch(ilosc){//depende cuantas veces se preciono
    case 1:
      imppasslcd();
      lcd.print("*_");
      break;
    case 2:
      imppasslcd();
      lcd.print("**_");
      break;
    case 3:
      imppasslcd();
      lcd.print("***_");
      break;
    case 4:
      imppasslcd();
      lcd.print("****_");
      break;
    case 5:
      imppasslcd();
      lcd.print("*****_");
      break;
    case 6:
      imppasslcd();
      lcd.print("******");
      ilosc = 0;
      break;
  }
///////////////////////MOSTRAR PASSWORD EN LCD//////////////////////////
}
void checkPassword(){
  if (password.evaluate()){//si la contraseña es correcta
    for(int i = 1; i <=3 ; i++ ){//hace sonar 3 veces el buzzer
      digitalWrite(buzzer, HIGH);  
      delay(100);            
      digitalWrite(buzzer, LOW);  
      delay(50);      
    }
    npasse = 0;//resetea al numero de veces que se envio la pass incorrectamente
    password.reset();//resetea el password para que no quede en memoria
    Serial.println("Correcto");//muestra que el pin es correcto en el serial
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("<PIN CORRECTO>");//muestra que el pin es correcto 
    delay(100);//espera 2 segundo prendido
    //GREGAR ACTIVAR O DESACTIVAR ALARMA
  }  
  else{ //SI LA CONTRASEÑA ES INCORRECTA
    npasse ++;//suma uno al contador de contraseña incorrecta
    password.reset();
    for(int i = 1; i <= 1; i++ ){//SUENA EL BUZZER  1 VEZ
      digitalWrite(buzzer, HIGH);  
      delay(400);            
      digitalWrite(buzzer, LOW);  
      delay(100);      
    }
  switch(npasse){//depende cuantas veces llego a ser incorrecta la clave
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
      mostrarpuk();
      break;
  }
  }
}

void mostrarpuk(){
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("INGRESE PUK");
  lcd.setCursor(1,1);
  lcd.print("PARA CONTINUAR");
  }
void checkpuk(){
   if (puk.evaluate()){//si el es correcta
    for(int i = 1; i <=3 ; i++ ){//hace sonar 3 veces el buzzer
      digitalWrite(buzzer, HIGH);  
      delay(100);            
      digitalWrite(buzzer, LOW);  
      delay(50);      
    }
    npasse = 0;//resetea al numero de veces que se envio la pass incorrectamente
    password.reset();//resetea el password para que no quede en memoria
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("<PUK CORRECTO>");//muestra que el puk es correcto 
    delay(100);//espera 2 segundo prendido
  }
  else{
    puk.reset();
    for(int i = 1; i <= 1; i++ ){//SUENA EL BUZZER  1 VEZ
      digitalWrite(buzzer, HIGH);  
      delay(400);            
      digitalWrite(buzzer, LOW);  
      delay(100);      
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("<PUK INCORRECTO>");
    lcd.setCursor(0,1);
    lcd.print("Intente de nuevo");
    }
}

void passinc(){//lcd muestra mensaje de contraseña incorrecta
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("<PIN INCORRECTO>");
  lcd.setCursor(0,1);
}
void imppasslcd(){
  if(npasse >= 3){  
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("INGRESE PUK");
  lcd.setCursor(0,1);
  }
  else{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("< INGRESAR PIN >");
  lcd.setCursor(0,1);
  }
}

