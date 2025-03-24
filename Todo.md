## Active
- [] Allow setting timezone
- [] Copy all libraries into project
- [] Confirm or centralize LittleFS intialization
- [] Implement backup and restore of LittleFS
- [] No json consistent central for bluetooth, debuggert and mqtt
- [] Remote debug disable telnet
- [] Custume remote debugger (expose print messages so bluetooth can implement)
- [] Implement remote deubugger terminal in bluetooth handler 
- [] Implement https
- [] Implement multiselect actions as files
- [] Implement actions as buttons on home page
- [] Implement time handler for timmed events
- [] Implement AesHandler
- [] Implement basic commands for nRF Toolbox for bluetooth only
- [] Document using nRF Toolbox with example commands
- [] Document EMQX setup
- [] Implement x,y and font size in GfxHandler
- [] Implement build_flags consistent with other projects follow #if rather than #ifdef -DENABLE_LIB=0/1
- [] Review code for blocking actions when not connected or missing default values after wiping preferences

### Bugs
- [] Nested folders
- [] Reboot loop if clock and settings are cleared

### Complete
- [x] GitHub pages action fix
- [x] Test working from dev branch
- [x] Implement template handler
- [x] Add wifi handler
- [x] Implement ConfigManager
- [x] Implement MqttHandler
- [x] Implement OTA
- [x] Implement secure MqttHandler with EMQX
- [x] Rather than action handler implment CommandHandler example tft hello led blue etc.
- [x] Implemente modal
- [x] Dynamic buttons
- [x] Implment task manager
- [x] Implement AES

## Bugs
Github pages not deploying from tag. Modified to deploy from action.