#ifndef EMBEDDED_FILES_H
#define EMBEDDED_FILES_H

#include <Arduino.h>

// Declarations for embedded files
extern const uint8_t styles_css_start[] asm("_binary_web_styles_css_start");
extern const uint8_t styles_css_end[] asm("_binary_web_styles_css_end");
extern const uint8_t device_html_start[] asm("_binary_web_device_html_start");
extern const uint8_t device_html_end[] asm("_binary_web_device_html_end");
extern const uint8_t settings_js_start[] asm("_binary_web_settings_js_start");
extern const uint8_t settings_js_end[] asm("_binary_web_settings_js_end");
extern const uint8_t nav_js_start[] asm("_binary_web_nav_js_start");
extern const uint8_t nav_js_end[] asm("_binary_web_nav_js_end");
extern const uint8_t index_html_start[] asm("_binary_web_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_web_index_html_end");
extern const uint8_t settings_html_start[] asm("_binary_web_settings_html_start");
extern const uint8_t settings_html_end[] asm("_binary_web_settings_html_end");
extern const uint8_t nav_html_start[] asm("_binary_web_nav_html_start");
extern const uint8_t nav_html_end[] asm("_binary_web_nav_html_end");

#endif // EMBEDDED_FILES_H
