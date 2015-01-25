#include "PPMOutput.h"

PPMOutput* ppmOutput = 0;

PPMOutput::PPMOutput(int ppmOutputPin) :
  _ppmOutputPin(ppmOutputPin),
  _signalUp(false),
  _currentSignal(-1)
{
  ppmOutput = this;
  
  for (int i = 0; i < 16; ++i) {
    if (i % 2 == 0) {
      // Every even signal is real channel signal
      this->_inputSignals[i] = Signal(HIGH, SIGNAL_SHORTEST);
    } else {
      // Every odd signal is pause between channels
      this->_inputSignals[i] = Signal(LOW, SIGNAL_SPACER);
    }
  }
  this->_inputSignals[15].duration = PPM_FRAME_LENGTH - (8 * SIGNAL_SHORTEST + 7 * SIGNAL_SPACER);

  this->applyInputSignals();
}

void PPMOutput::initialize()
{
  pinMode(_ppmOutputPin, OUTPUT);
  
  TCCR1A = 0;
  TCCR1B = 0;
  
  // CTC mode (CTC = Clear Timer on Compare)
  // NORMAL MODE
  TCCR1A |= (0 << WGM11) | (0 << WGM10);
  TCCR1B |= (0 << WGM13) | (0 << WGM12);
  
  // Set prescaling = 8, this will give us 16Mhz / 8 = 2Mhz clock speed
  // i.e. each tick will be 1 / 2000000 seconds long = 0.5 microseconds
  // giving us 65536 * 0.5 microseconds = 32.768 milliseconds as the longest 
  // possible signal length.
  TCCR1B |= (0 << CS12) | (1 << CS11) | (0 << CS10);
  
  // Enable output compare A and B match interrupt
  TIMSK1 |= (1 << OCIE1A) | (1 << OCIE1B);

  // B match marks the end of the current frame
  OCR1B = PPM_FRAME_LENGTH * 2;
  
  // Initialize counter to zero.
  TCNT1 = 0;
  
  // finally, enable global interrupts
  interrupts();
}

void PPMOutput::applyInputSignals()
{
  memcpy(this->_signals, this->_inputSignals, 16 * sizeof(Signal)); 
}

Signal PPMOutput::nextSignal()
{
  if (this->_currentSignal >= 15) {
    return this->_signals[15]; 
  }

  return this->_signals[this->_currentSignal++];
}

void PPMOutput::interrupt(boolean restart)
{
  if (restart) { 
    TCNT1 = 0;
    OCR1A = 0;
    this->applyInputSignals();
    this->_currentSignal = 0;
  }
  
  Signal signalToSend = this->nextSignal();
  // duration is in microseconds and each step is 0.5 microseconds, so we'll multiply by two
  int counterValue = signalToSend.duration * 2;
  OCR1A += counterValue;
  
  digitalWrite(this->_ppmOutputPin, signalToSend.state);
}

// Reads next frame from receiver to _inputSignals, keeping in mind that only the even indexed
// items are the real channel values and odd numbered are the spaces.
// Item at index 15 is a special case as that is the separator between two consecutive PPM
// frames.
//
// This should be called from the main loop of the program, not inside interrupt handler.
//
void PPMOutput::outputPPM(long* channels, int numChannels) 
{
  // Disable interrupts as we don't want interrupt processing to kick in here as that would
  // potentially cause incomplete _inputSignals to be copied to _signals and PPM frames could
  // go out of sync.
  noInterrupts();
  
  // Total sum of all pulses, note that this will not include pauses between pulses, so that 
  // needs to be counted in separately.
  long totalTime = 0;
  for (int i = 0; i < 8; ++i) {
    long duration = SIGNAL_SHORTEST;
    if (i < numChannels) {
      duration = SIGNAL_SHORTEST + channels[i] * (SIGNAL_LONGEST - SIGNAL_SHORTEST) / MAX_RX_THROW;
      duration = max(min(duration, SIGNAL_LONGEST), SIGNAL_SHORTEST);
    }
    this->_inputSignals[i*2].duration = duration;
    totalTime += duration;
  }
  this->_inputSignals[15].duration = PPM_FRAME_LENGTH - (totalTime + 7 * SIGNAL_SPACER);
  
  interrupts();
}

ISR(TIMER1_COMPA_vect)
{
  ppmOutput->interrupt(false);
}

ISR(TIMER1_COMPB_vect)
{
  ppmOutput->interrupt(true);
}
