#include <Wire.h>                            // Communication I2C
#include <FastLED.h>                         // Contrôle des LED WS2812
#include <Adafruit_GFX.h>                    // Librairie graphique générique
#include <Adafruit_SSD1306.h>                // Pour l'écran OLED

// ==== LED WS2812 ====
#define LED_PIN     4                        // Broche de données des LED
#define NUM_LEDS    21                       // Nombre total de LED dans la bande
#define BRIGHTNESS  32                       // Luminosité
#define LED_TYPE    WS2812                   // Type de LED utilisé
#define COLOR_ORDER GRB                      // Ordre des couleurs
CRGB leds[NUM_LEDS];                         // Tableau des LED

// ==== OLED ====
#define SCREEN_WIDTH 128                     // Largeur de l'écran OLED
#define SCREEN_HEIGHT 64                     // Hauteur de l'écran OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);  // Initialisation écran

// ==== PIR ====
#define PIR_POWER A2                         // Alimentation du capteur PIR
#define PIR_SIGNAL A3                        // Sortie du signal de détection

// ==== BUZZER ====
#define BUZZER_PIN 5                         // Broche du buzzer

// === MINUTEUR ===
unsigned long startTime = 0;                 // Temps de début de présence
unsigned long lastMotionTime = 0;            // Dernière détection de mouvement
bool isTiming = false;                       // Statut du chronomètre

// === Compteur d'entrées ===
int totalEntries = 0;                        // Compteur de personnes entrées
bool wasOccupied = false;                    // Pour éviter les doubles comptages

// === Avertissement ===
bool warningPlayed = false;                  // Pour éviter de rejouer l’alerte

// === Économie d'énergie ===
bool screenOn = true;                        // L’écran est actif ou non
unsigned long lastDisplayUpdate = 0;         // Dernière mise à jour de l’écran
const unsigned long displayTimeout = 10000;  // Temps avant extinction écran

// === Fonctions ===

// Définit la couleur des LED (valeurs de 0 à 255 pour chaque couleur)
void setColor(int redValue, int blueValue, int greenValue) {
  fill_solid(leds, NUM_LEDS, CRGB(greenValue, redValue, blueValue));
  FastLED.show();
}

// Mélodie jouée quand la cabine devient libre
void playMelodyFree() {
  int melody[] = { 523, 659 };
  for (int i = 0; i < 2; i++) {
    tone(BUZZER_PIN, melody[i], 150);
    delay(200);
    noTone(BUZZER_PIN);
  }
}

// Mélodie d’avertissement si la personne reste trop longtemps
void playMelodyWarning() {
  int melody[] = { 392, 523, 392 };
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, melody[i], 200);
    delay(250);
    noTone(BUZZER_PIN);
  }
}

void setup() {
  Serial.begin(115200);                      // Démarrage communication série

  // Configuration du capteur PIR
  pinMode(PIR_POWER, OUTPUT);
  digitalWrite(PIR_POWER, HIGH);             // Alimentation du capteur PIR
  pinMode(PIR_SIGNAL, INPUT);                // Lecture du signal
  pinMode(BUZZER_PIN, OUTPUT);               // Buzzer en sortie

  // Initialisation de la bande LED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  setColor(BRIGHTNESS, 0, 0);                // Vert = libre

  // Initialisation de l’écran OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Erreur OLED");
    while (true); // Boucle infinie si écran non détecté
  }

  // Affichage initial
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Cabine prete");
  display.display();
  lastDisplayUpdate = millis();              // Temps de dernière mise à jour
}

void loop() {
  bool motion = digitalRead(PIR_SIGNAL);     // Lire si mouvement détecté
  unsigned long currentTime = millis();      // Temps actuel

  if (motion) {
    lastMotionTime = currentTime;

    if (!isTiming) {                         // Début d'une nouvelle occupation
      isTiming = true;
      startTime = currentTime;
      warningPlayed = false;

      if (!wasOccupied) {                    // Nouvelle entrée détectée
        totalEntries++;
        Serial.print("Nouvelle entree ! Total : ");
        Serial.println(totalEntries);
      }

      wasOccupied = true;
    }

    unsigned long timeSpent = (currentTime - startTime);

    if (timeSpent > 20000 && !warningPlayed) { // Après 20 secondes : alerte
      setColor(BRIGHTNESS, BRIGHTNESS, 0);     // Jaune
      playMelodyWarning();
      warningPlayed = true;
    } else if (timeSpent <= 20000) {
      setColor(0, 0, BRIGHTNESS);              // Rouge pendant l'occupation
    }

    // Mise à jour de l’affichage OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Cabine OCCUPEE");
    display.setCursor(0, 20);
    display.println("Temps actif:");
    display.print(timeSpent / 1000);
    display.println(" sec");
    display.setCursor(0, 50);
    display.print("Entrees: ");
    display.print(totalEntries);
    display.display();
    screenOn = true;
    lastDisplayUpdate = currentTime;
  }
  else {
    if (isTiming && (currentTime - lastMotionTime > 5000)) {
      // Si plus de mouvement pendant de plus de 5 secondes, on considère la cabine libre
      isTiming = false;
      wasOccupied = false;
      warningPlayed = false;

      setColor(BRIGHTNESS, 0, 0);             // Vert
      playMelodyFree();                       // Petite mélodie

      // Affichage état libre
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Cabine LIBRE");
      display.setCursor(0, 50);
      display.print("Entrees: ");
      display.print(totalEntries);
      display.display();
      screenOn = true;
      lastDisplayUpdate = currentTime;
    }
  }

  // Éteindre l’écran si inactif pendant un certain temps = Economie d'énergie
  if (screenOn && (currentTime - lastDisplayUpdate > displayTimeout)) {
    display.clearDisplay();
    display.display();
    screenOn = false;
  }

  delay(200); //  pause pour éviter de charger la boucle
}