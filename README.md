# Squatstopper

est un système intelligent de gestion d’occupation pour cabines d’essayage, toilettes publiques ou tout autre espace clos. Il utilise un capteur PIR, un écran OLED, une bande LED WS2812 et un buzzer pour indiquer en temps réel l’état de la cabine. Le système envoie également les données d’usage (temps d’occupation et nombre d’entrées) vers un webhook Zapier via WiFi.

Ce projet est parfait pour prévenir les abus de temps dans les cabines, améliorer la rotation des utilisateurs et obtenir des statistiques utiles sur l’usage.




![Uploading FF36DF97-D0C8-469F-B5E6-8BAAECC64A95.jpeg…]()
![C186F1CE-C60D-478F-B71A-85365C7DCEF8](https://github.com/user-attachments/assets/b0fd9550-8d1a-4013-aac5-8fb7434f4ba9)







⚙️ Composants utilisés
	•	🔌 Microcontroleur Arduino UNICA
	•	📷 Capteur PIR
	•	🧠 Écran OLED I2C 128x64
	•	🔊 Buzzer actif
	•	🔌 Breadboard + câbles Dupont

🧠 Fonctionnalités
	•	🔎 Détection de mouvement : grâce au capteur PIR
	•	⏱️ Minuteur intelligent : calcule le temps passé dans la cabine
	•	📊 Compteur d’entrées : incrémenté uniquement en cas de présence prolongée
	•	🌈 Signal visuel :
	•	Vert = libre
	•	Rouge = occupé (moins de 20 sec)
	•	Bleu + buzzer = squatt détecté (plus de 20 sec)
	•	💤 Économie d’énergie : extinction automatique de l’écran OLED après inactivité
 
🛠️ Schéma de câblage
	•	PIR :
	•	VCC → A2
	•	OUT → A3
	•	GND → GND
	•	OLED :
	•	VCC → V_IN
	•	GND → GND
	•	SCL → A5
	•	SDA → A4
	•	Buzzer :
	•		•	→ D5
	•		•	→ GND
	•	LED WS2812 :
	•	DIN → D4
	•	GND → GND
	•	5V → V_IN
