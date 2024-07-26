#include <LiquidCrystal.h> 
LiquidCrystal lcd (8, 9, 4, 5, 6, 7);

#define right  0
#define left   1
#define up     2
#define down   3
#define select 4
#define none   5

#define encoder_switch 2
#define outputA 1
#define outputB 0

int aState, aLastState, Read; 
int comp = 0;
unsigned long encoder_delay = 0;

float lcd_key = 0, measured = 0.0, max = 0, min = 5, gain = 1, offset = 0, pot_reading = 0.0;
int option_counter = 0, f_tuning = 0, A_state, A_last_state, B_state;

int lcd_key_reading();
double Rotary_Encoder(float counter, float upper_limit, float lower_limit);

void setup() {
  pinMode(A0, INPUT); 
  pinMode(A1, INPUT); 
  lcd.begin(16, 2); 
  A_last_state = digitalRead(outputA);
}

void loop() {
  lcd_key = lcd_key_reading();
  pot_reading = analogRead(A1) * 5.0 / 1023;
  measured = (pot_reading + offset) * gain;
  A_state = digitalRead(outputA);
  B_state = digitalRead(outputB);

  if (lcd_key == right) {
    f_tuning++;
    delay(500);
    if (f_tuning > 1) {
      f_tuning = 0;
    }
  }

  if (lcd_key == select) {
    option_counter++;
    delay(200);
    lcd.clear();
    if (option_counter > 4) {
      option_counter = 0;
    }
  }

  switch (option_counter) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("select mode:");
      lcd.setCursor(0, 1);
      break;

    case 1:
      lcd.setCursor(0, 0);
      lcd.print("mode 1");
      lcd.setCursor(0, 1);
      lcd.print("measured=");
      lcd.print(measured, 3);
      break;

    case 2:
      gain = Rotary_Encoder(gain, 99.9, 0.01);
      if (lcd_key == up) {
        if (f_tuning == 0) {
          gain += 1;
        } else {
          gain += 0.01;
        }
        delay(200);
        if (gain > 99.9) {
          gain = 99.9;
        }
      }
      if (lcd_key == down) {
        if (f_tuning == 0) {
          gain -= 1;
        } else {
          gain -= 0.01;
        }
        delay(200);
        if (gain < 0.01) {
          gain = 0.01;
        }
      }
      lcd.setCursor(0, 0);
      lcd.print("mode 2 ");
      lcd.setCursor(0, 1);
      lcd.print("gain=");
      lcd.print(gain, 3);
      break;

    case 3:
      offset = Rotary_Encoder(offset, 2.5, -2.5);
      if (lcd_key == up) {
        if (f_tuning == 0) {
          offset += 0.25;
        } else {
          offset += 1;
        }
        delay(200);
        if (offset > 2.5) {
          offset = 2.5;
        }
      }
      if (lcd_key == down) {
        if (f_tuning == 0) {
          offset -= 0.25;
        } else {
          offset -= 1;
        }
        delay(200);
        if (offset < -2.5) {
          offset = -2.5;
        }
      }
      lcd.setCursor(0, 0);
      lcd.print("mode 3 ");
      lcd.setCursor(0, 1);
      lcd.print("offset=");
      lcd.print(offset, 3);
      break;

    case 4:
      if (measured > max) {
        max = measured;
      }
      if (measured < min) {
        min = measured;
      }
      if (lcd_key == left) {
        max = 0;
        min = 5;
      }
      lcd.setCursor(0, 0);
      lcd.print("mode 4");
      lcd.setCursor(0, 1);
      lcd.print("max=");
      lcd.print(max);
      lcd.print(" min=");
      lcd.print(min);
      break;
  }
}

int lcd_key_reading() {
  int Button_reading = analogRead(A0);

  if (Button_reading > 950) return none;
  if (Button_reading < 50) return right;
  if (Button_reading < 195) return up;
  if (Button_reading < 380) return down;
  if (Button_reading < 500) return left;
  if (Button_reading < 750) return select;
  return none;
}

double Rotary_Encoder(float counter, float upper_limit, float lower_limit) {
  float step;
  if ((millis() - encoder_delay) > 170) {
    Read = analogRead(encoder_switch);
    encoder_delay = millis();
  }
  if (Read < 50) {
    comp += 1;
  }
  if (comp == 4) {
    comp = 0;
  }
  Read = 100;
  switch (comp) {
    case 0:
      step = 0.01;
      break;
    case 1:
      step = 0.1;
      break;
    case 2:
      step = 1;
      break;
    case 3:
      step = 10;
      break;
  }

  aState = digitalRead(outputA);
  if (aState != aLastState) {
    if ((digitalRead(outputB) != aState) && (counter < upper_limit)) {
      counter += step;
    } else if (counter > lower_limit) {
      counter -= step;
    }
  }
  aLastState = aState;
  if (counter > upper_limit) {
    counter = upper_limit;
  }
  if (counter < lower_limit) {
    counter = lower_limit;
  }
  return counter;
}
