int time_scale = 1;
bool setupCO = true;
void setTimer2PWM(byte chA, byte chB) //pins D11 and D3
{
  TCCR2A = 0b10100011; //OCA,OCB, fast pwm
  TCCR2B = 0b001; //no prescaler
  OCR2A = chA; //0..255
  OCR2B = chB;
}

float opt_voltage = 0;
byte opt_width = 240; //default reasonable value

void pwm_adjust()
//this function tries various PWM cycle widths and prints resulting
//voltage for each attempt, then selects best fitting one and
//this value is used in the program later
{
  float previous_v = 5.0; //voltage at previous attempt
  float raw2v = 5.0 / 1024.0;//coefficient to convert Arduino's 
  //analogRead result into voltage in volts
  for(int w = 0; w < 250; w++)
  {
    setTimer2PWM(0, w);
    float avg_v = 0;
    for(int x = 0; x < 100; x ++) //measure over about 100ms to ensure stable result
    {
      avg_v += analogRead(A1);
      delay(time_scale);
    }
    avg_v *= 0.01;
    avg_v *= raw2v;
    Serial .print("adjusting PWM w=");
    Serial.print(w);
    Serial.print(", V=");
    Serial.println(avg_v);
    if(avg_v < 3.6 && previous_v > 3.6) //we found optimal width
    {
      float dnew = 3.6 - avg_v; //now we need to find if current one
      float dprev = previous_v - 3.6;//or previous one is better
      if(dnew < dprev) //if new one is closer to 1.4 then return it
      {
        opt_voltage = avg_v;
        opt_width = w;
        return;
      }
      else //else return previous one
      {
        opt_voltage = previous_v;
        opt_width = w-1;
        return;
      }
    }
    previous_v = avg_v;
  }
}


byte phase = 0; //1 - high voltage, 0 - low voltage, we start from measuring
unsigned long prev_ms = 0; //milliseconds in previous cycle
unsigned long sec10 = 0; //this timer is updated 10 times per second,
//when it will overflow, program might freeze or behave incorrectly. 
//It will happen only after ~13 years of operation. Still, 
//if you'll ever use this code in industrial application,
//please take care of overflow problem 
unsigned long high_on = 0; //time when we started high temperature cycle
unsigned long low_on = 0; //time when we started low temperature cycle
unsigned long last_print = 0; //time when we last printed message in serial

float sens_val = 0; //current smoothed sensor value
float last_CO_ppm_measurement = 0; //CO concentration at the end of previous measurement cycle

void startMeasurementPhase()
{
  phase = 0;
  low_on = sec10;
  setTimer2PWM(0, opt_width);
}

void startHeatingPhase()
{
  phase = 1;
  high_on = sec10;
  setTimer2PWM(0, 255);
}

void setup() {

  pinMode(3, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  analogReference(DEFAULT);
  Serial.begin(9600);

}
unsigned long ms;
int dt;
void loop() 
{

  ms = millis();
  dt = ms - prev_ms;
  if (digitalRead(6) == HIGH) {    
    startCO();    
  }
  else {
    setupCO = true;}
}

void startCO() {
  if (setupCO)
  {
  setupCO = false;  
  pwm_adjust();

  Serial.print("PWM result: width ");
  Serial.print(opt_width);
  Serial.print(", voltage ");
  Serial.println(opt_voltage);
  Serial.println("Data output: raw A0 value, heating on/off (0.1 off 1000.1 on), CO ppm from last measurement cycle");
  
  startMeasurementPhase(); //start from measurement
  }
  if(dt > 100*time_scale || dt < 0) 
  {
    prev_ms = ms; //store previous cycle time
    sec10++; //increase 0.1s counter
  }
  if(phase == 1 && sec10 - high_on > 600) //60 seconds of heating ended?
    startMeasurementPhase();
  if(phase == 0 && sec10 - low_on > 900) //90 seconds of measurement ended?
  {
    last_CO_ppm_measurement = sens_val;
    Serial.print(last_CO_ppm_measurement);
    Serial.println(" ");
    startHeatingPhase();
  }

  float v = analogRead(A0); //reading value
  sens_val *= 0.999; //applying exponential averaging using formula
  sens_val += 0.001*v; // average = old_average*a + (1-a)*new_reading
  if(sec10 - last_print > 9) //print measurement result into serial 2 times per second
  {
    last_print = sec10;
    Serial.print(sens_val);
    Serial.print(" ");
    Serial.print(0.1 + phase*1000);
    Serial.print(" ");
    Serial.println(last_CO_ppm_measurement);
  }
}