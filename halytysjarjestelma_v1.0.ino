#include <Keypad.h>
#include <LiquidCrystal.h>

unsigned long aloitusAika = 0;
unsigned long viive = 5000;

int PIRsensor = 13;
int PIRvalue = 0;

String userInput;
String salasana = "0000";

int flag;
int flag2;

int count = 5;

int cursorColumn = 0;

//keypadin setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {12, 11, 10, 9};
byte colPins[COLS] = {8, 7, 6, 5};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


//LCD-näytön setup
const int rs = 2, rw = 3, enable = 4, d4 = A0, d5 = A1, d6 = A2, d7 = A3;
LiquidCrystal lcd(rs, rw, enable, d4, d5, d6, d7);


void setup() {
  lcd.begin(16, 2); //lcd-näytön käyttöliittymän käynnistys
  Serial.begin(9600);

  lcd.print("   Tervetuloa");
  delay(2000);
  lcd.clear();
  pinMode(24, OUTPUT);
  digitalWrite(24, LOW);
  pinMode(A0, OUTPUT);
  digitalWrite(A0, HIGH);
  pinMode(A1, OUTPUT);
  digitalWrite(A1, HIGH);
  pinMode(A2, OUTPUT);
  digitalWrite(A2, HIGH);
  pinMode(A3, OUTPUT);
}

void loop() {
  lcd.clear();
  lcd.print("A: Halytys");
  lcd.setCursor(0, 1);
  lcd.print("B: Salasana");
  char key = keypad.waitForKey();
  switch (key) //valikkotoiminnot
  {
    case 'A': //hälytys päälle ja pois
      halytysAktivointi();
      break;
    case 'B': //salasanan vaihto
      vaihdaSalasana();
      break;
  }
}

//salasanojen vertailufunktio
void checkSalasana(String pwd) {
  while (userInput != salasana) { //verrataan userInput-muuttujan sisältöä salasana-muuttujan sisältöön; 
    lcd.clear();
    lcd.print("Salasana?");
    lcd.setCursor(0, 1);
    userInput = kysySalasana(); //asetetaan userInput-muuttujaan kysySalasana-funktion palautusarvo
    if (userInput != salasana) { //jos väärin, niin kehotetaan yrittämään uudestaan
      lcd.clear();
      lcd.print("  Yrita");
      lcd.setCursor(0, 1);
      lcd.print("     uudelleen");
      delay(1500);
    }
  }
  userInput = ""; //nollataan userInput-muuttuja, jotta ei aiheuta bugeja
  lcd.clear();
  lcd.print("  Salasana");
  lcd.setCursor(0, 1);
  lcd.print("        oikein");
  delay(1500);
  if (flag2 == 1) { //jos liikettä havaittu ja salasana oikein, mennään tänne
    halytysDeaktivointi(); 
  }
}

//salasanan vaihtamisfunktio
void vaihdaSalasana() {
  lcd.clear();
  lcd.print("  Salasanan");
  lcd.setCursor(0, 1);
  lcd.print("        vaihto");
  delay(2000);
  checkSalasana(salasana); //ensin kysytään vanha salasana
  lcd.clear();
  lcd.print("Uusi salasana?");
  lcd.setCursor(0, 1);
  salasana = kysySalasana(); //jos oikein niin asetetaan salasanaksi kysySalasana-funktion palautusarvo
  lcd.clear();
  lcd.print("  Salasana");
  lcd.setCursor(0, 1);
  lcd.print("    vaihdettu!");
  delay(2000);
}

//salasanan kysymisfunktio
//luetaan keypadia ja palautetaan salasana
String kysySalasana()
{
  String num;
  char key = keypad.getKey();
  while (key != '#')
  {
    switch (key)
    {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        lcd.print("*");
        num = num + key;
        break;

      case '*':
        num = "";
        lcd.clear();
        lcd.print("Salasana?");
        lcd.setCursor(0, 1);
        break;
    }
    key = keypad.getKey();

    if (millis() - aloitusAika > viive && PIRvalue == 1 && flag == 0) { //jos aikaraja ylittyy ja PIRvalue == 1, suoritetaan halytysTriggered
      halytysTriggered();
    }
  }
  return num;

}

//hälytyksen aktivointifunktio
void halytysAktivointi()
{
  digitalWrite(24, LOW); //käytetään wifi-moduuli pois päältä
  
  while (count > 0) { //countdown hälytyksen aktivoimiseksi
    lcd.clear();
    lcd.print("Lahtolaskenta: ");
    lcd.setCursor(0, 1);
    lcd.print(count);
    delay(1000);
    count--;
  }
  count = 5;
  lcd.clear();
  lcd.print("  Halytys");
  lcd.setCursor(0, 1);
  lcd.print("     aktivoitu");
  while (PIRvalue == 0) {
    PIRvalue = digitalRead(PIRsensor); //luetaan sensorin arvoa niin kauan, että arvo ei ole 0
  }
  if (PIRvalue == 1) { //kun sensorin arvo 1, suoritetaan seuraavat toimenpiteet
    lcd.clear();
    lcd.print("Liiketta        ");
    lcd.setCursor(0, 1);
    lcd.print("       havaittu!");
    delay(2000);
    aloitusAika = millis();
    flag2 = 1;
    checkSalasana(salasana); //kysytään salasana hälytyksen purkamiseksi

  }
}

//hälytyksen laukaisufunktio
void halytysTriggered() {
  digitalWrite(24, HIGH); //wifi-moduuli päälle
  tone(22, 100); //summeri päälle
  flag = 1;
  lcd.clear();
  lcd.print("    HALYTYS!");
  delay(2000);
  lcd.clear();
  lcd.print("Salasana?");
  lcd.setCursor(0, 1);
}

//hälytyksen deaktivointifunktio
void halytysDeaktivointi() {
  noTone(22); //summeri pois päältä
  flag = 0;
  flag2 = 0;
  PIRvalue = 0;
  lcd.clear();
  lcd.print("  Halytys");
  lcd.setCursor(0, 1);
  lcd.print("   deaktivoitu");
  delay(2000);
}
