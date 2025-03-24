#include "Globals.h"
#include "TemplateHandler.h"
#include "RemoteDebugHandler.h"
#include "ImprovHandler.h"
#include "GfxHandler.h"
#include "LedHandler.h"
#include "ButtonHandler.h"
#include "WifiHandler.h"
#include "WebHandler.h"
#include "BluetoothHandler.h"
#include "MqttHandler.h"
#include "OTAHandler.h"
#include "TimeHandler.h"
#include "CommandHandler.h"
#include "DeviceHandler.h"
#include "ScriptHandler.h"
#include "CronHandler.h"
#include "SystemMonitor.h"
#include "DownloadHandler.h"
#include "AesHandler.h"

void setup()
{
  TemplateHandler::init();
  RemoteDebugHandler::init();
  ScriptHandler::init();
  ConfigManager::init();
  BluetoothHandler::init();
  GfxHandler::init();
  WifiHandler::init();
  WebHandler::init();
  ImprovHandler::init();
  LedHandler::init();
  ButtonHandler::init();
  MqttHandler::init();
  OTAHandler::init();
  CommandHandler::init();
  DeviceHandler::init();
  CronHandler::init();
  SystemMonitor::init();
  DownloadHandler::init();
  AesHandler::init();
  //TimeHandler::init(settings.timezone.c_str());
  TimeHandler::init();
  // GfxHandler::printMessage(SOFTWARE_VERSION);
  // LedHandler::setDefaultBrightness(100);
  // LedHandler::setColorByName("Purple");
}

void loop()
{
  TemplateHandler::loop();
  RemoteDebugHandler::loop();
  ScriptHandler::loop();
  BluetoothHandler::loop();
  WifiHandler::loop();
  WebHandler::loop();
  ImprovHandler::loop();
  ButtonHandler::loop();
  MqttHandler::loop();
  OTAHandler::loop();
  DeviceHandler::loop();
  TimeHandler::loop();
}