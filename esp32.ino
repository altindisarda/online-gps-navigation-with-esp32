#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#define TFT_CS   15
#define TFT_DC   2
#define TFT_RST  4 
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        String incoming = String(pCharacteristic->getValue().c_str());
        if (incoming.length() > 5) {
            tft.fillScreen(ILI9341_BLACK);
            
            // Veri: YON, MESAFE, SURE, TOPLAM, SOKAK
            int v1 = incoming.indexOf(',');
            int v2 = incoming.indexOf(',', v1 + 1);
            int v3 = incoming.indexOf(',', v2 + 1);
            int v4 = incoming.lastIndexOf(',');

            String yon = incoming.substring(0, v1);
            String mesafe = incoming.substring(v1 + 1, v2);
            String sure = incoming.substring(v2 + 1, v3);
            String toplam = incoming.substring(v3 + 1, v4);
            String sokak = incoming.substring(v4 + 1);

            // 1. ÜST ŞERİT (Mavi Arka Plan - Sokak Adı)
            tft.fillRect(0, 0, 320, 45, ILI9341_BLUE);
            tft.setTextColor(ILI9341_WHITE);
            tft.setTextSize(2);
            tft.setCursor(10, 15);
            tft.print(sokak);

            // 2. YÖN OKU (BÜYÜK)
            tft.setTextSize(14);
            tft.setCursor(170, 60);
            if(yon == "SAG") { tft.setTextColor(ILI9341_GREEN); tft.print(">"); }
            else if(yon == "SOL") { tft.setTextColor(ILI9341_ORANGE); tft.print("<"); }
            else { tft.setTextColor(ILI9341_CYAN); tft.print("^"); }

            // 3. MESAFE
            tft.setTextSize(5);
            tft.setTextColor(ILI9341_WHITE);
            tft.setCursor(10, 85);
            tft.print(mesafe);

            // 4. ALT BİLGİLER
            tft.drawFastHLine(0, 190, 320, ILI9341_WHITE);
            tft.setTextSize(2);
            tft.setTextColor(ILI9341_YELLOW);
            tft.setCursor(10, 210);
            tft.print("Kalan: " + sure);
            tft.setCursor(180, 210);
            tft.print(toplam);
        }
    }
};

void setup() {
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(ILI9341_BLACK);
    
    BLEDevice::init("MOTO_NAV_PRO");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pChar = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
    pChar->setCallbacks(new MyCallbacks());
    pService->start();
    BLEDevice::getAdvertising()->addServiceUUID(SERVICE_UUID);
    BLEDevice::getAdvertising()->start();
    
    tft.setCursor(20, 100);
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_CYAN);
    tft.print("Navigasyon Hazir...");
}
void loop() {}