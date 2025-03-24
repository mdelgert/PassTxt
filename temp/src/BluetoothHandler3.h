#pragma once

#ifdef ENABLE_BLUETOOTH_HANDLER

#include "Globals.h"
#include <NimBLEDevice.h>

class BluetoothHandler
{
private:
    static NimBLEServer* pServer;
    static NimBLECharacteristic* pTxCharacteristic;
    static NimBLECharacteristic* pRxCharacteristic;

    class RxCallback : public NimBLECharacteristicCallbacks {
        void onWrite(NimBLECharacteristic* pCharacteristic) override;
    };

public:
    static void init();
    static void loop();
};

#else

class BluetoothHandler {
public: // No-op implementation of BluetoothHandler
    static void init() {} // No-op
    static void loop() {} // No-op
};

#endif // ENABLE_BLUETOOTH_HANDLER
