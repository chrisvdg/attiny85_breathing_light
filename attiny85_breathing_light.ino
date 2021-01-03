
#include <avr/sleep.h>
#include <avr/wdt.h>

int LEDPIN = 0;
float smoothness_pts = 1000;
float gamma = 0.14; 
float beta = 0.5;

void setup() {
  // generate randomness seed
  randomSeed(analogRead(0));
  // turn off ADC to save energy
  ADCSRA = 0;
  
  setupIO();
}

void setupIO() {
  pinMode(LEDPIN, OUTPUT);
}

void loop() {
  for (int i=0;i<smoothness_pts;i++){
    float pwm_val = 255.0*(exp(-(pow(((i/smoothness_pts)-beta)/gamma,2.0))/2.0));
    analogWrite(LEDPIN,int(pwm_val));
    pwmSleep();
  }

  deepSleep(random(10,1000));
}

// sleep for 16ms with PWM still enabled
void pwmSleep(){
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();

  sleep_bod_disable();
  noInterrupts();

  // clear various "reset" flags
  MCUSR = 0;  // allow changes, disable reset
  WDTCR = bit (WDCE) | bit(WDE); // set interrupt mode and an interval
  WDTCR = bit (WDIE);
  wdt_reset();

  interrupts();
  
  sleep_cpu();
  sleep_disable();

  setupIO();
}

// Deep sleep for count * 8 seconds
void deepSleep(int count) {
  for(int i = 0; i < count; i++) {
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
  
    sleep_bod_disable();
    noInterrupts();
  
    // clear various "reset" flags
    MCUSR = 0;  // allow changes, disable reset
    WDTCR = bit (WDCE) | bit(WDE); // set interrupt mode and an interval
    WDTCR = bit (WDIE) | bit(WDP3) | bit(WDP0); // set the timer (counts) after which to signal an interrupt
    wdt_reset();
  
    interrupts();

  
    sleep_cpu();

    sleep_disable();
  }

  setupIO();
}


ISR (WDT_vect) {
  wdt_disable();
}