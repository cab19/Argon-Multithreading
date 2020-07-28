/*
 * Project Multithreading demo
 * Description: Demonstration of leveraging RTOS multithreading on the Argon
 * Author: Chris Bounsall
 * Date:
 */


#include "Particle.h"

// Pin setup for LEDs
int redLED = D0; // variable for Red LED pin
int yellowLED = D4; // variable for Yellow LED pin
int greenLED = D7; // variable for Green LED pin

//Multithreading
SYSTEM_THREAD(ENABLED); //multi threading, system runs on its own thread
void preSetup(); // startup function to register mutex before setup is called
STARTUP(preSetup()); // mutex must be initialised before setup
void thread1Function(void *param); // declaration of thread1 function
void thread2Function(void *param); // declaration of thread2 function
Thread thread1; // thread 1
Thread thread2; // thread 2
os_mutex_t mutex; // mutex object

// variables
system_tick_t lastThreadTime = 0; // used to monitor system time for thread 1
int varDelay = 9000; // the delay to be applied to thread 1 in milliseconds (9 seconds)


// System setup function
void setup() {
    // SERIAL
    Serial.begin(9600); // Starts the serial communication

    // Cloud Function
    Particle.function("callThread2Function", callThread2Function); // allows realtime calling to thread 2

    // Multithreading
    thread1 = Thread("thread1", thread1Function); // instantiation of thread 1
    thread2 = Thread("thread2", thread2Function); // instantiation of thread 2

    // setup pins
    pinMode(redLED, OUTPUT);
    pinMode(yellowLED, OUTPUT);
    pinMode(greenLED, OUTPUT);
}

// System loop function
void loop() {
    // system thread
    flashLED(greenLED, 3); // flash green led 3 times
    delay(3000); // wait 3 seconds
}

// Function run before setup, therefore allowing the creation of the mutex before it's referenced...
void preSetup() {
	os_mutex_create(&mutex);  // Create the mutex
}

// Calls flashLED every X seconds determined by varDelay variable
void thread1Function(void *param) {
	while(true) {
        // do stuff
        flashLED(yellowLED, 5); // flash yellow led 5 times
        os_thread_delay_until(&lastThreadTime, varDelay);	// Delay X seconds between cycles
	}
	// You must not return from the thread function, once thread starts it should never end...
}

void thread2Function(void *param) {
	while(true) {
        os_mutex_lock(mutex); // lock mutex, code runs once
        WITH_LOCK(Serial) { // lock serial to thread
            Serial.println("Function 2 called");
        }
        flashLED(redLED, 1); // flash red led 1 time
	}
	// You must not return from the thread function, once thread starts it should never end...
}

// function triggered via cloud, unlocks mutex which triggers thread2Function
int callThread2Function(String arg) {
  Serial.println("MUTEX UNLOCK");
  os_mutex_unlock(mutex);
  return 1;
}

// Flash passed in LED x amount of times
void flashLED(int targetLED, int cycles){
    
    for(int x = 0; x < cycles; x++){
        digitalWrite(targetLED, 1); // turn on
        delay(500); // delay half a second
        digitalWrite(targetLED, 0); // turn off
        delay(500); // delay half a second
    }
}