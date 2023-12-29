#include "arduinoFFT.h"

#define MIC_PIN  34
#define SAMPLES 128             //SAMPLES-pt FFT. Must be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 4800 //Ts = Based on Nyquist, must be 2 times the highest expected frequency.

unsigned int samplingPeriod;
unsigned long microSeconds;

double vReal[SAMPLES]; //create vector of size SAMPLES to hold real values
double vImag[SAMPLES]; //create vector of size SAMPLES to hold imaginary values

arduinoFFT FFT = arduinoFFT();


const int sampleWindow = 50;                              // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

unsigned int output;
int Decibels;


void setup()
{

  Serial.begin(115200); //Baud rate for the Serial Monitor
  samplingPeriod = round(1000000 * (1.0 / SAMPLING_FREQUENCY)); //Period in microseconds

}


void loop()
{

  /*----------------------------------------Sound Sensor FRQ---------------------------------------------------*/
  /*Sample SAMPLES times*/
  for (int i = 0; i < SAMPLES; i++)
  {
    microSeconds = micros();    //Returns the number of microseconds since the Arduino board began running the current script.

    vReal[i] = analogRead(MIC_PIN ); //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
    vImag[i] = 0; //Makes imaginary term 0 always

    /*remaining wait time between samples if necessary*/
    while (micros() < (microSeconds + samplingPeriod))
    {
      //do nothing
    }
  }

  /*Perform FFT on samples*/
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

  /*Find peak frequency and print peak*/
  double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);


//  Serial.print("Freq: ");
//  Serial.print(peak);


  /*--------------------------------------Sound Sensor db-----------------------------------------------------*/

  unsigned long start_time = millis(); 
  float PeakToPeak = 0;  
  unsigned int maximum_signal = 0;  //minimum value
  unsigned int minimum_signal = 4000;  //maximum value
  while (millis() - start_time < 50)
  {
    output = analogRead(MIC_PIN );  
    if (output < 4000)  
    {
      if (output > maximum_signal)
      {
        maximum_signal = output;  
      }
      else if (output < minimum_signal)
      {
        minimum_signal = output; 
      }
    }
  }
  PeakToPeak = maximum_signal-minimum_signal; 
  
  Decibels = map(PeakToPeak, 100, 3100, 32, 80); 

  
  Serial.print("dB: ");
  Serial.println(Decibels);

  
}
