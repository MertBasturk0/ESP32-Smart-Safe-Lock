#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// ---------------- Pin tanimlari ----------------
#define YESIL_LED   2
#define KIRMIZI_LED 1
#define BUZZER      45
#define LCD_SDA     9
#define LCD_SCL     10

// Buzzer tipi: aktif buzzer ise true, pasif buzzer ise false yapin
#define BUZZER_AKTIF false

// ---------------- Ayarlar ----------------
#define KASA_SIFRESI  "12345"   // kasa sifresi
#define TOPLAM_HAK    3     // yanlis girme hakki
#define GERI_SAYIM    10    // patlama geri sayimi (saniye)

// ---------------- Keypad ----------------
const byte SATIR = 4;
const byte SUTUN = 4;
char tuslar[SATIR][SUTUN] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
// Keypad pin 1-4 = sutunlar (ters sira), pin 5-8 = satirlar (ters sira)
byte satirPinleri[SATIR] = {46, 3, 8, 18};
byte sutunPinleri[SUTUN] = {17, 16, 15, 7};
Keypad keypad = Keypad(makeKeymap(tuslar), satirPinleri, sutunPinleri, SATIR, SUTUN);

// ---------------- LCD ----------------
LiquidCrystal_I2C *lcd = nullptr;

// ---------------- Durum ----------------
String sifre = "";
String girilen = "";
int haneSayisi = 0;
int kalanHak = TOPLAM_HAK;

// ---------------- Yardimci fonksiyonlar ----------------
void buzzerAc(int frekans = 2000) {
  if (BUZZER_AKTIF) digitalWrite(BUZZER, HIGH);
  else tone(BUZZER, frekans);
}

void buzzerKapat() {
  if (BUZZER_AKTIF) digitalWrite(BUZZER, LOW);
  else noTone(BUZZER);
}

void bip(int sure, int frekans = 2000) {
  buzzerAc(frekans);
  delay(sure);
  buzzerKapat();
}

void lcdYaz(const String &satir1, const String &satir2) {
  lcd->clear();
  lcd->setCursor(0, 0);
  lcd->print(satir1);
  lcd->setCursor(0, 1);
  lcd->print(satir2);
}

void sifreAyarla() {
  sifre = KASA_SIFRESI;
  haneSayisi = sifre.length();
}

void sifreEkraniGoster() {
  girilen = "";
  lcdYaz("Sifre gir:" + String(haneSayisi) + "hane", "");
}

void girilenGoster() {
  lcd->setCursor(0, 1);
  lcd->print("                ");
  lcd->setCursor(0, 1);
  for (unsigned int i = 0; i < girilen.length(); i++) lcd->print('*');
}

void kasaPatlat() {
  digitalWrite(YESIL_LED, LOW);
  for (int s = GERI_SAYIM; s > 0; s--) {
    lcdYaz("KASA PATLAYACAK!", String(s) + " saniye kaldi");
    digitalWrite(KIRMIZI_LED, HIGH);
    bip(200, 2500);
    delay(300);
    digitalWrite(KIRMIZI_LED, LOW);
    delay(500);
  }
  lcdYaz("   !!! BOOM !!!", " KASA PATLADI!");
  digitalWrite(KIRMIZI_LED, HIGH);
  buzzerAc(1000);
  delay(3000);
  buzzerKapat();
  lcdYaz(" KASA PATLADI!", "Reset'e basin");
  while (true) {           // sistem kilitlenir, reset gerekir
    digitalWrite(KIRMIZI_LED, !digitalRead(KIRMIZI_LED));
    delay(500);
  }
}

void sifreKontrol() {
  Serial.printf("Girilen: %s  Dogru: %s\n", girilen.c_str(), sifre.c_str());
  if (girilen == sifre) {
    digitalWrite(KIRMIZI_LED, LOW);
    digitalWrite(YESIL_LED, HIGH);
    lcdYaz("  KASA ACILDI!", "Kilit icin: #");
    bip(100, 2000); delay(80); bip(100, 2500); delay(80); bip(200, 3000);

    // '#' basilana kadar acik kalir, sonra tekrar kilitlenir
    while (keypad.getKey() != '#') delay(10);
    digitalWrite(YESIL_LED, LOW);
    kalanHak = TOPLAM_HAK;
    sifreAyarla();
    sifreEkraniGoster();
    return;
  }

  // Yanlis sifre
  kalanHak--;
  digitalWrite(KIRMIZI_LED, HIGH);
  if (kalanHak <= 0) {
    lcdYaz(" YANLIS SIFRE!", "Hakkiniz bitti!");
    bip(1000, 1000);
    kasaPatlat();
  }
  lcdYaz(" YANLIS SIFRE!", "Kalan hak: " + String(kalanHak));
  bip(1000, 1000);
  delay(1000);
  digitalWrite(KIRMIZI_LED, LOW);
  sifreEkraniGoster();
}

// ---------------- Setup / Loop ----------------
void setup() {
  Serial.begin(115200);

  pinMode(YESIL_LED, OUTPUT);
  pinMode(KIRMIZI_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(YESIL_LED, LOW);
  digitalWrite(KIRMIZI_LED, LOW);
  buzzerKapat();

  // LCD adresini otomatik bul (genelde 0x27 veya 0x3F)
  Wire.begin(LCD_SDA, LCD_SCL);
  uint8_t adres = 0x27;
  for (uint8_t a : {0x27, 0x3F}) {
    Wire.beginTransmission(a);
    if (Wire.endTransmission() == 0) { adres = a; break; }
  }
  Serial.printf("LCD adresi: 0x%02X\n", adres);
  lcd = new LiquidCrystal_I2C(adres, 16, 2);
  lcd->init();
  lcd->backlight();

  lcdYaz(" ELEKTRONIK KASA", "  Hosgeldiniz");
  // LED testi: acilista iki LED de 1 sn yanar
  Serial.println("LED testi: yesil + kirmizi yaniyor");
  digitalWrite(YESIL_LED, HIGH);
  digitalWrite(KIRMIZI_LED, HIGH);
  delay(1500);
  digitalWrite(YESIL_LED, LOW);
  digitalWrite(KIRMIZI_LED, LOW);

  sifreAyarla();
  sifreEkraniGoster();
}

void loop() {
  char tus = keypad.getKey();
  if (!tus) return;

  Serial.printf("Basilan tus: %c\n", tus);
  bip(40, 3000);  // tus sesi

  if (tus >= '0' && tus <= '9') {
    girilen += tus;
    girilenGoster();
    if ((int)girilen.length() == haneSayisi) {
      delay(300);
      sifreKontrol();
    }
  } else if (tus == '*') {        // son haneyi sil
    if (girilen.length() > 0) girilen.remove(girilen.length() - 1);
    girilenGoster();
  } else if (tus == 'C') {        // tumunu temizle
    girilen = "";
    girilenGoster();
  }
}