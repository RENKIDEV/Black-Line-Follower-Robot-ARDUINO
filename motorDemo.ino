#include "CarMotors.h" // Bibliothèque pour contrôler les moteurs du robot
#include "FastLED.h"   // Bibliothèque pour gérer les LEDs RGB

CarMotors car; // Objet représentant les moteurs de la voiture
int intersectionCount = 0; // Compteur du nombre d'intersections 

// define pour les broches des capteurs de ligne
#define LEFT_SENSOR_PIN A2
#define CENTER_SENSOR_PIN A1
#define RIGHT_SENSOR_PIN A0

// Define pour le pin de la led et le nombre de leds
#define PIN_RGBLED 4
#define NUM_LEDS 1

CRGB leds[NUM_LEDS]; // Tableau avec couleur led

// Seuil de détection des capteurs de ligne
#define THRESHOLD 628

// Vitesse de base et vitesse pour les virages
#define BASE_SPEED 100
#define TURN_SPEED 80

// Seuil addition des capteurs permet la detection d'intersections
int allsensor = 2350; 

// Indicateur pour savoir si le robot est actuellement à une intersection
bool isAtIntersection = false;

void setup() {
  Serial.begin(9600); // Permet d'initialiser la com serie
  car.init(BASE_SPEED); // Init de la vitesse 
  pinMode(LEFT_SENSOR_PIN, INPUT);   // Configurer le capteur gauche comme entrée
  pinMode(CENTER_SENSOR_PIN, INPUT); // Configurer le capteur central comme entrée
  pinMode(RIGHT_SENSOR_PIN, INPUT);  // Configurer le capteur droit comme entrée
  FastLED.addLeds<NEOPIXEL, PIN_RGBLED>(leds, NUM_LEDS); // Initialisation de la led
  FastLED.setBrightness(100); // Réglage de la luminosité de la led
  leds[0] = CRGB(255, 0, 0); // Rouge
  FastLED.show(); // Appliquer la couleur sur la led
}

void loop() {
  // Variables pour gérer une séquence d'initialisation avant le démarrage
  static unsigned long startTime = 0; // Temps de début
  static bool hasStarted = false; // Indique si le robot a démarré

  // demarage du robot, sequences leds
  if (!hasStarted) {
    if (startTime == 0) {
      startTime = millis(); // Enregistrer le moment ou le programme commence
      leds[0] = CRGB(255, 0, 0); // Rouge pour indiquer que le robot est au repos
      FastLED.show();
    }

    unsigned long currentTime = millis() - startTime; // Temps écoulé depuis le début

    if (currentTime >= 1000 && currentTime < 2000) {
      leds[0] = CRGB(255, 125, 0); // Orange pour signaler un état intermèdiaire
      FastLED.show();
    } else if (currentTime >= 2000) {
      leds[0] = CRGB(0, 255, 0); // Vert pour signaler que le robot est prêt à partir
      FastLED.show();
      hasStarted = true; // Marquer que le robot a demarrer
    }
    return; // Arreter ici pour laisser la sequence d'initialisation se dérouler
  }

  // Lecture des valeurs des capteurs de ligne
  int leftSensor = analogRead(LEFT_SENSOR_PIN); // Valeur du capteur gauche
  int centerSensor = analogRead(CENTER_SENSOR_PIN); // Valeur du capteur central
  int rightSensor = analogRead(RIGHT_SENSOR_PIN); // Valeur du capteur droit

  // Controle du mouvement en fonction des capteurs
  if (centerSensor > THRESHOLD) {
    car.setSpeed(BASE_SPEED); // Avancer en ligne droite
    car.goForward();
  } else if (leftSensor > THRESHOLD) {
    car.setSpeed(TURN_SPEED); // Tourner à gauche
    car.turnLeft();
  } else if (rightSensor > THRESHOLD) {
    car.setSpeed(TURN_SPEED); // Tourner à droite
    car.turnRight();
  } else {
    car.stop(); // S'arreter si aucun capteur n'est actif
  }

  // Détection d'une intersection
  if (allsensor < leftSensor + centerSensor + rightSensor && 
      leftSensor > THRESHOLD && centerSensor > THRESHOLD && rightSensor > THRESHOLD) {
    if (!isAtIntersection) { // Vérifier si c'est une nouvelle intersection
      isAtIntersection = true; // Marquer que nous sommes a une intersection
      intersectionCount++; // Incrémenter le compteur d'intersections
      Serial.println(intersectionCount); // Afficher le compteur sur le moniteur série

      // Changer la couleur des LEDs selon le nombre d'intersections
      if (intersectionCount < 3) {
        leds[0] = CRGB(0, 255, 0); // Vert
        FastLED.show();
      } else if (intersectionCount < 5) {
        leds[0] = CRGB(0, 0, 255); // Bleu
        FastLED.show();
      } else if (intersectionCount < 7) {
        leds[0] = CRGB(255, 128, 0); // Orange
        FastLED.show();
      }

      // Si le compteur atteint 7, reculer légèrement et arrêter
      if (intersectionCount >= 7) {
        car.setSpeed(50); // Vitesse lente
        car.goBackward(); // Reculer légèrement
        delay(250);        
        car.stop(); // Arrêt complet
        leds[0] = CRGB(255, 255, 0); // Jaune pour end
        FastLED.show();
        while (true) {} //Stopper completement
      }
    }
  } else {
    isAtIntersection = false; // Reset
  }

  delay(10);
}

void updateLEDColor(int step) {
  // Tableau de couleurs leds
  static const CRGB colors[] = {
    CRGB(255, 0, 0),   // Rouge
    CRGB(0, 255, 0),   // Vert
    CRGB(0, 0, 255),   // Bleu
    CRGB(255, 125, 0), // Orange
    CRGB(255, 0, 255)  // Violet
  };
  // Mise à jour de la LED avec la couleur appropriée
  leds[0] = colors[step % 5]; // 
  FastLED.show(); // Appliquer la couleur
}
