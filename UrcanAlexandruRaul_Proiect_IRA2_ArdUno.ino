#include <Servo.h> // Include biblioteca Servo

// Definirea servo-urilor
Servo servoX; // Motorul servo pentru coordonata X
Servo servoY; // Motorul servo pentru coordonata Y

// Variabile pentru stocarea pozitiilor servomotoarelor
int posX = 90; // Pozitia initiala pentru servoX (mijloc)
int posY = 90; // Pozitia initiala pentru servoY (mijloc)

// Limitele de miscare pentru servomotoare
const int minServoPos = 0; // Pozitia minima servo
const int maxServoPos = 180; // Pozitia maxima servo

// Variabile pentru controlul temporizarii
unsigned long previousMillis = 0;
const long interval = 112; // Interval de timp la care se face achizitia datelor (pozitia fetei)

// Pozitii tinta pentru servomotoare
int targetX = posX; // Pozitia tinta servoX
int targetY = posY; // Pozitia tinta servoY

// Constanta regulatorului proportional (P)
const float Kp = 0.16; // Factor de amplificare proportional

// Constante pentru componentele integrala (I) si derivata (D)
const float Ki = 0.01; // Coeficientul pentru integrare
const float Kd = 0.05; // Coeficientul pentru derivare

// Variabile pentru componentele integrala si derivata
int integralX = 0; // Integrala erorii X
int integralY = 0; // Integrala erorii Y
int lastErrorX = 0; // Ultima eroare X
int lastErrorY = 0; // Ultima eroare Y

void setup() {
  servoX.attach(9); // Pinul de control pentru motorul servo X
  servoY.attach(11); // Pinul de control pentru motorul servo Y

  Serial.begin(9600); // Configureaza comunicatia seriala (rata Baud)
}

void loop() {
  // Citirea datelor prin portul serial
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n'); // Citeste o linie de la portul serial

    int commaIndex = data.indexOf(','); // Cauta indexul virgulei
    if (commaIndex != -1) {
      String x_str = data.substring(0, commaIndex); // Extrage coordonata X
      String y_str = data.substring(commaIndex + 1); // Extrage coordonata Y

      int x = x_str.toInt(); // Converteste coordonata X la un numar intreg
      int y = y_str.toInt(); // Converteste coordonata Y la un numar intreg

      // Mapeaza coordonatele fetei la un interval pentru servo
      targetX = map(x, 0, 640, maxServoPos, minServoPos); // Inversam maparea pe axa X (camera vede in oglinda fata de realitate)
      targetY = map(y, 0, 480, minServoPos, maxServoPos); // 480 este inalțimea tipică pentru un frame video
    }
  }

  // Verifica daca a trecut intervalul de actualizare
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Calculeaza eroarea
    int errorX = targetX - posX; // Eroarea pentru coordonata X
    int errorY = targetY - posY; // Eroarea pentru coordonata Y

    // Calculeaza ajustarea folosind regulatorul PID
    int adjustmentX = Kp * errorX + Ki * integralX + Kd * (errorX - lastErrorX); // Ajustarea pentru servoX
    int adjustmentY = Kp * errorY + Ki * integralY + Kd * (errorY - lastErrorY); // Ajustarea pentru servoY

    // Actualizeaza variabilele pentru componentele I si D
    integralX += errorX; // Integrarea erorii X
    integralY += errorY; // Integrarea erorii Y
    lastErrorX = errorX; // Ultima eroare X
    lastErrorY = errorY; // Ultima eroare Y

    // Ajusteaza pozitiile curente servo
    posX += adjustmentX; // Ajusteaza pozitia servoX
    posY += adjustmentY; // Ajusteaza pozitia servoY

    // Ajusteaza pozitiile servo si asigura ca sunt in limitele permise (Asemenea unui bloc de saturatie) 
    posX = constrain(posX, minServoPos, maxServoPos);
    posY = constrain(posY, minServoPos, maxServoPos);

    // Aplica noile pozitii
    servoX.write(posX);
    servoY.write(posY);

    // Printeaza pozitiile pentru debugging (optional)
    Serial.print("X: ");
    Serial.print(posX);
    Serial.print(" Y: ");
    Serial.println(posY);
  }
}
