#include "Button_Manager.h"
#include "Buzzer_Manager.h"
#include "Timer_Manager.h"
#include "LCD_Manager.h"
#include <EEPROM.h>

void initButtons() {
  pinMode(BUTTON_SET, INPUT_PULLUP);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
}

bool readButton(uint8_t pin) {
  return digitalRead(pin) == LOW;
}


void waitForSetButton() {
  while (digitalRead(BUTTON_SET) == LOW) {
    delay(10);
  }

  while (digitalRead(BUTTON_SET) == HIGH) {
    delay(10);
  }

  while (digitalRead(BUTTON_SET) == LOW) {
    delay(10);
  }
}


void handleMainMenuButtons(bool currentSetBtn, bool currentUpBtn, bool currentDownBtn) {
  static bool lastUpBtn = false;
  static bool lastDownBtn = false;
  static bool lastSetBtn = false;

  if (currentSetBtn && !lastSetBtn) {
    if (!editingTime) {
      editingTime = true;
      currentItem = HOURS;
    } else {
      if (currentItem == ON_OFF) {
        editingTime = false;
        if (onState) {
          origHours = hours;
          origMinutes = minutes;
          origSeconds = seconds;
          startCountdown();
        }
      } else {
        currentItem = (MainMenuItem)((int)currentItem + 1);
      }
    }
    buzzShortNonBlocking();
  }

  if (editingTime) {
    if (currentUpBtn && !lastUpBtn) {
      switch (currentItem) {
        case HOURS: hours = (hours + 1) % 24; break;
        case MINUTES: minutes = (minutes + 1) % 60; break;
        case SECONDS: seconds = (seconds + 1) % 60; break;
        case ON_OFF: onState = !onState; break;
      }
      buzzShortNonBlocking();
    }

    if (currentDownBtn && !lastDownBtn) {
      switch (currentItem) {
        case HOURS: hours = (hours - 1 + 24) % 24; break;
        case MINUTES: minutes = (minutes - 1 + 60) % 60; break;
        case SECONDS: seconds = (seconds - 1 + 60) % 60; break;
        case ON_OFF: onState = !onState; break;
      }
      buzzShortNonBlocking();
    }
  }

  lastUpBtn = currentUpBtn;
  lastDownBtn = currentDownBtn;
  lastSetBtn = currentSetBtn;
}

void handleSettingsButtons(bool currentSetBtn, bool currentUpBtn, bool currentDownBtn) {
  static bool lastUpBtn = false;
  static bool lastDownBtn = false;
  static bool lastSetBtn = false;

  if (currentSetBtn && !lastSetBtn) {
    if (!settingsEditing) {
      settingsEditing = true;
    } else {
      settingsEditing = false;

      if (setpointTemp != lastSavedSetpoint) {
        EEPROM.put(0, setpointTemp);
        lastSavedSetpoint = setpointTemp;
      }

      previousAValue = currentAValue;
      currentAValue = aValue;

      if (previousAValue == 10 && currentAValue == 21) {
          specialScreenActive = true;
          lcd.clear();
          lcd.setCursor(4, 0);
          lcd.print("Hello");
          lcd.setCursor(0, 1);
          lcd.print(">");
          lcd.setCursor(1, 1);
          lcd.print("Back");
          waitForSetButton();
          specialScreenActive = false;
          lcd.clear();
      } else if (previousAValue == 11 && currentAValue == 21) {
          specialScreenActive = true;
          lcd.clear();
          lcd.setCursor(4, 0);
          lcd.print("ChemTech");
          lcd.setCursor(0, 1);
          lcd.print(">");
          lcd.setCursor(1, 1);
          lcd.print("Back");
          waitForSetButton();
          specialScreenActive = false;
          lcd.clear();
      } else if (previousAValue == 12 && currentAValue == 21) {
          specialScreenActive = true;
          lcd.clear();
          lcd.setCursor(4, 0);
          lcd.print("Welcome");
          lcd.setCursor(0, 1);
          lcd.print(">");
          lcd.setCursor(1, 1);
          lcd.print("Back");
          waitForSetButton();
          specialScreenActive = false;
          lcd.clear();
      } else if (previousAValue == 13 && currentAValue == 21) {
          specialScreenActive = true;
          lcd.clear();
          lcd.setCursor(4, 0);
          lcd.print("Naeem");
          lcd.setCursor(0, 1);
          lcd.print(">");
          lcd.setCursor(1, 1);
          lcd.print("Back");
          waitForSetButton();
          specialScreenActive = false;
          lcd.clear();
      } else if (previousAValue == 14 && currentAValue == 21) {
          specialScreenActive = true;
          lcd.clear();
          lcd.setCursor(4, 0);
          lcd.print("Hiii");
          lcd.setCursor(0, 1);
          lcd.print(">");
          lcd.setCursor(1, 1);
          lcd.print("Back");
          waitForSetButton();
          specialScreenActive = false;
          lcd.clear();
      }
    }
    buzzShortNonBlocking();
  }

  if (!settingsEditing && currentSetBtn && !lastSetBtn) {
    settingsCurrentItem = (SettingsMenuItem)((settingsCurrentItem + 1) % 3);
    buzzShortNonBlocking();
  }

  if (settingsEditing) {
    if (currentUpBtn && !lastUpBtn) {
      switch (settingsCurrentItem) {
        case SETPOINT: 
          setpointTemp = constrain(setpointTemp + 0.5, 0.0, 100.0);
          break;
        case MODE: 
          onState = !onState;
          break;
        case A_VALUE:
          aValue = constrain(aValue + 1, 1, 100);
          break;
      }
      buzzShortNonBlocking();
    }

    if (currentDownBtn && !lastDownBtn) {
      switch (settingsCurrentItem) {
        case SETPOINT: 
          setpointTemp = constrain(setpointTemp - 0.5, 0.0, 100.0);
          break;
        case MODE: 
          onState = !onState;
          break;
        case A_VALUE:
          aValue = constrain(aValue - 1, 1, 100);
          break;
      }
      buzzShortNonBlocking();
    }
  }

  lastUpBtn = currentUpBtn;
  lastDownBtn = currentDownBtn;
  lastSetBtn = currentSetBtn;
}

void updateButtons() {
  static unsigned long setPressTime = 0;
  bool currentSetBtn = readButton(BUTTON_SET);
  bool currentUpBtn = readButton(BUTTON_UP);
  bool currentDownBtn = readButton(BUTTON_DOWN);

  if (currentSetBtn && setPressTime == 0) {
    setPressTime = millis();
  } else if (!currentSetBtn && setPressTime > 0) {
    if (millis() - setPressTime >= longPressDuration) {
      screenMode = (screenMode == MAIN_MENU) ? SETTINGS_MENU : MAIN_MENU;
      settingsEditing = false;
      editingTime = false;
      lcd.clear();
    }
    setPressTime = 0;
  }

  if (screenMode == MAIN_MENU) {
    handleMainMenuButtons(currentSetBtn, currentUpBtn, currentDownBtn);
  } else if (screenMode == SETTINGS_MENU) {
    handleSettingsButtons(currentSetBtn, currentUpBtn, currentDownBtn);
  }
}