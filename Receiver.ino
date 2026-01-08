/*
   PROJE: Yangın ve Gaz Tespit Sistemi - ALICI (Receiver)
   DONANIM: 
     - LED (Pin 8)
     - 433MHz RF Alıcı (Data Pin 2)
   
   ÖZELLİKLER:
     - Gelen veriyi çözme
     - Ekrana Risk ve Ham Değer basma
     - Alarm veya Gaz Risk 2+ durumunda LED yakma
*/

#include <RH_ASK.h>
#include <SPI.h>

// --- RF AYARLARI ---
// Hız: 2000bps, RX: 2 (Alıcı Pini), TX: 12 (Boş)
RH_ASK rf_driver(2000, 2, 12);

const int LED_PIN = 8;

// --- VERİ PAKETİ (Verici ile AYNI olmalı) ---
struct VeriPaketi {
  uint8_t alarmDurumu;
  uint8_t alevRiski;
  uint8_t gazRiski;
  int alevDegeri;
  int gazDegeri;
};

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  if (!rf_driver.init()) Serial.println("RF Modulu Hatasi!");
  
  Serial.println("--- ALICI MONITORU BASLATILDI ---");
  Serial.println("Veri Bekleniyor...");
}

void loop() {
  VeriPaketi gelenPaket;
  uint8_t paketBoyutu = sizeof(gelenPaket);

  // Veri Geldi mi?
  if (rf_driver.recv((uint8_t *)&gelenPaket, &paketBoyutu)) {
    
    // --- EKRANA YAZDIRMA ---
    
    // 1. Durum
    if(gelenPaket.alarmDurumu == 1) {
       Serial.print("[TEHLIKE] ");
    } else {
       Serial.print("[GUVENLI] ");
    }
    
    // 2. Alev Bilgileri
    Serial.print("ALEV: ");
    Serial.print(gelenPaket.alevDegeri);
    Serial.print(" (Risk: ");
    Serial.print(gelenPaket.alevRiski);
    Serial.print(") | ");

    // 3. Gaz Bilgileri
    Serial.print("GAZ: ");
    Serial.print(gelenPaket.gazDegeri);
    Serial.print(" (Risk: ");
    Serial.print(gelenPaket.gazRiski);
    Serial.println(")");

    // --- LED KONTROLÜ ---
    // Alarm varsa VEYA Gaz Riski 2 ve üzeriyse LED YAN
    if (gelenPaket.alarmDurumu == 1 || gelenPaket.gazRiski >= 2) {
      digitalWrite(LED_PIN, HIGH);
    } else {
      digitalWrite(LED_PIN, LOW);
    }
  }
}
