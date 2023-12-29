int Buzzer =  13;
int up_led = 4;
int low_led = 6;
int relay = 12;
int sw_u = 3;
int sw_l = 7;


void setup()
{
  pinMode(Buzzer, OUTPUT);
  pinMode(up_led, OUTPUT);
  pinMode(low_led, OUTPUT);
  pinMode(relay, OUTPUT);

  pinMode(sw_l, INPUT_PULLUP);
  pinMode(sw_u, INPUT_PULLUP);

  digitalWrite(relay, HIGH);
}

void led_stat()
{
  if (digitalRead(sw_u) == HIGH)
  {
    digitalWrite(up_led, HIGH);
  }

  if (digitalRead(sw_u) == LOW)
  {
    digitalWrite(up_led, LOW);
  }

  if (digitalRead(sw_l) == HIGH)
  {
    digitalWrite(low_led, HIGH);
  }

  if (digitalRead(sw_l) == LOW)
  {
    digitalWrite(low_led, LOW);
  }
}

void(* resetFunc) (void) = 0;

void loop()
{
  int c = 0;
  led_stat();
  while (1)
  {

    led_stat();
    if ( digitalRead(sw_u) == LOW && digitalRead(sw_l) == LOW && c == 0)
    {

      delay(5000);

      if ( digitalRead(sw_u) == LOW && digitalRead(sw_l) == LOW && c == 0)
      {
        digitalWrite(Buzzer, HIGH);
        delay(10000);
        digitalWrite(Buzzer, LOW);

        delay(2000);

        digitalWrite(relay, LOW);

        c = 1;
      }
    }


    led_stat();
    if ( digitalRead(sw_u) == HIGH && digitalRead(sw_l) == HIGH && c == 1 )
    {
      delay(2000);
      if ( digitalRead(sw_u) == HIGH && digitalRead(sw_l) == HIGH && c == 1 )
      {
        for (int i = 0; i < 40; i++)
        {
          digitalWrite(Buzzer, HIGH);
          delay(250);
          digitalWrite(Buzzer, LOW);
          delay(250);
        }
        delay(100);
        digitalWrite(relay, HIGH);
        c = 0;
        resetFunc();
      }
    }

  }
}
