#include "StateMachineLib.h"
#include "Keypad.h";
#include "LiquidCrystal.h";
int Contlcd = 0;
int ContClave = 0;
int cont = 0;
char miArray[10];
char clave[] = {"5555"};
char dig;
int currentState;
int TECLA;
int TECLA2;
int digito;
char digitoconv;
int ledGreen = 28;
int ledRed = 30;
int buzzer=26;
int tono = 1915;
///lcd
LiquidCrystal lcd(13, 12, 11, 10, 9, 8); //(RS, RW, E, D4,D5, D6, D7)

//// teclado

const byte FILAS = 4;     // define numero de filas
const byte COLUMNAS = 4;    // define numero de columnas
char keys[FILAS][COLUMNAS] = {    // define la distribucion de teclas
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pinesFilas[FILAS] = {7, 6, 5, 4}; // pines correspondientes a las filas
byte pinesColumnas[COLUMNAS] = {3, 2, 24, 22}; // pines correspondientes a las columnas

Keypad teclado = Keypad(makeKeymap(keys), pinesFilas, pinesColumnas, FILAS, COLUMNAS);  // crea objeto

enum State
{
  Espera = 0,
  Encendido = 1,
  Alerta = 2,

};

enum Input
{
  R = 0,
  T = 1,
  A = 2,
  Unknown = 3,
};

StateMachine stateMachine(4, 9);
Input input;

void setupStateMachine()
{

  stateMachine.AddTransition(Espera, Encendido, []() {
    return input == T;
  });

  stateMachine.AddTransition(Encendido, Espera, []() {
    return input == R;
  });
  stateMachine.AddTransition(Espera, Alerta, []() {
    return input == A;
  });

  stateMachine.AddTransition(Alerta, Espera, []() {
    return input == R;
  });

  stateMachine.SetOnEntering(Espera, outputA);
  stateMachine.SetOnEntering(Encendido, outputB);
  stateMachine.SetOnEntering(Alerta, outputC);

  /*
     stateMachine.SetOnLeaving(PosicionA, []() {Serial.println("Leaving A"); });
     stateMachine.SetOnLeaving(PosicionB, []() {Serial.println("Leaving B"); });
     stateMachine.SetOnLeaving(PosicionC, []() {Serial.println("Leaving C"); });
     stateMachine.SetOnLeaving(PosicionD, []() {Serial.println("Leaving D"); });
  */
}

void setup()
{
  Serial.begin(9600);
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledRed, LOW);
  //pinMode(16, OUTPUT);
  noTone(buzzer);
  digitalWrite(buzzer,HIGH);
  //pinMode(buzzer, OUTPUT);
  digitalWrite(15, HIGH);
  digitalWrite(14, HIGH);


  // lcd

  lcd.begin(16, 2);            // Inicia el LCD 16x02 (columnas, filas)
  lcd.setCursor(0, 0);         // Coloca el cursor en las coordenadas (0,0)
  lcd.print("Ingrese la clave"); // Escribe no LCD


  Serial.println("Starting State Machine...");
  setupStateMachine();
  Serial.println("Start Machine Started");

  stateMachine.SetState(Espera, false, true);
}

void loop()
{
  input = static_cast<Input>(readInput());

  stateMachine.Update();

}

int readInput()
{

  Input currentInput = Input::Unknown;

  lcd.setCursor(0, 1);

  //Se lee el dígito ingresado en el teclado matricial

  TECLA = teclado.waitForKey();
  if ( TECLA == '*') {
    lcd.print("                    ");
    lcd.setCursor(0, 1);
    cont = 0;

  }
  // read the incoming byte:

  else
  {

    Serial.println(TECLA);

    currentState = stateMachine.GetState();
    TECLA -= 48;
    Contlcd++;
    TECLA2 = TECLA + 48;
    digitoconv = (char)TECLA2;

    if (Contlcd == 1) {
      lcd.print(TECLA);
      delay(300);
      lcd.print("                    ");
      lcd.setCursor(0, 1);
      lcd.print("*_");
    }
    if (Contlcd == 2) {
      lcd.setCursor(1, 1);
      lcd.print(TECLA);
      delay(300);
      lcd.print("                    ");
      lcd.setCursor(0, 1);
      lcd.print("**_");
    }
    if (Contlcd == 3) {
      lcd.setCursor(2, 1);
      lcd.print(TECLA);
      delay(300);
      lcd.print("                    ");
      lcd.setCursor(0, 1);
      lcd.print("***_");
    }
    if (Contlcd == 4) {
      lcd.setCursor(3, 1);
      lcd.print(TECLA);
      delay(300);
      lcd.print("                    ");
      lcd.setCursor(0, 1);
      lcd.print("****");
    }

    miArray[cont++] = digitoconv;
    Serial.flush();

    if (cont == 4) {
      lcd.print("                    ");
      lcd.setCursor(0, 1);
      if (strcmp (miArray, clave) != 0) {
        Serial.println("error clave");
        lcd.print("clave incorrecta");
        digitalWrite(ledRed, HIGH);
        delay(2000);
        digitalWrite(ledRed, LOW);
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        ContClave ++;
      }
      digitalWrite(buzzer, HIGH);
      if (strcmp (miArray, clave) == 0) {
        noTone(buzzer);
       digitalWrite(buzzer, HIGH);
        //tone(buzzer,tono);
        ContClave = 0;
        digitalWrite(ledGreen, HIGH);
        delay(2000);
        digitalWrite(ledGreen, LOW);
        currentInput = Input:: T;

      }
      if ((currentState == State::Espera) && (ContClave >= 3))
      {
        tone(buzzer, tono,1000);
        //digitalWrite(16, HIGH);
        currentInput = Input:: A;
      }
      if ((currentState == State::Alerta) && (strcmp (miArray, clave) == 0))
      {

        lcd.print("clave correcta");
        delay(2000);
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        currentInput = Input:: R;

        delay(4000);
      }
      if ((currentState == State::Encendido) && (strcmp (miArray, "0987") == 0))
      {
        ContClave = 0;
        currentInput = Input:: R;
      }
      cont = 0;
      Contlcd = 0;
    }

  }

  return currentInput;

}

void outputA()
{
  Serial.println("Espera   encendido   alerta   ");
  Serial.println("X                              ");
  Serial.println();
}

void outputB()
{
  Serial.println("Espera   encendido   alerta   ");
  Serial.println("              X                ");
  Serial.println();
}

void outputC()
{
  Serial.println("Espera   encendido   alerta   ");
  Serial.println("                          X    ");
  Serial.println();
}
