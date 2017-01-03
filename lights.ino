#include <LowPower.h>
#include <Adafruit_MCP23017.h>

class SimpleMcpExpander
{
public:
  SimpleMcpExpander(byte addr)
    : m_address(addr)
    , m_current(0xFF)
    , m_previous(0xFF)
    , m_outputs(0)
  {}

  void begin() {
    mcp.begin(m_address);
    mcp.writeRegister(MCP23017_GPPUA, 0xFF);  // pullups on A
    mcp.writeRegister(MCP23017_IODIRB, 0);    // outputs on B
    Serial.print(F("Init, address "));
    Serial.println(m_address);
    Serial.flush();
  }

  void scanInputs() {
    m_previous = m_current;
    m_current = mcp.readGPIO(0);
  }

  void setOutputs(byte value) {
    m_outputs = value;
    mcp.writeRegister(MCP23017_GPIOB, value);
  }

  inline byte currentInputs() const {
    return m_current;
  }

  inline byte previousInputs() const {
    return m_previous;
  }

  inline byte address() const {
    return m_address;
  }

  inline byte outputs() const {
    return m_outputs;
  }

private:
  SimpleMcpExpander(SimpleMcpExpander&) = delete;
  Adafruit_MCP23017 mcp;
  byte m_address;
  byte m_current;
  byte m_previous;
  byte m_outputs;
};


// mklink-ed to Arduino's Library and called HomeLightingProject
#include <Rules.h>
#include <PinMonitor.h>
#include <RuleInterpreter.h>

static SimpleMcpExpander* expanders[8] = {
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
};

class RelayActionExecutor
{
public:
  void onTick() {
    // TODO maintain timers
  }

  void toggle(byte port, byte pin) {
    for (auto& expander : expanders) {
      if (!expander) {
        break;
      }
      if (expander->address() == port) {
        Serial.print(F("Toggling "));
        Serial.print(port);
        Serial.print(' ');
        Serial.println(pin);
        expander->setOutputs(expander->outputs() ^ (1 << pin));
        return;
      }
    }
  }

  void timerReset(byte port, byte pin) {}
  void allOff() {}

  void perform(byte action) {
    switch (static_cast<Actions>(action >> SHIFT_ACTION)) {
    case Toggle:
      toggle(7 & (action >> SHIFT_PORT), action & 7);
      break;

    case TimerReset:
      timerReset(7 & (action >> SHIFT_PORT), action & 7);
      break;

    case AllOff:
      allOff();
      break;
    }
  }
};

static RelayActionExecutor executor;
static RuleInterpreter ruleInterpreter(rules, rulesSize);
static const byte expanderCount = 2;
static PinMonitor pinMonitor(8 * expanderCount);

void setup()
{
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Serial.begin(9600);
  while (!Serial);

  static SimpleMcpExpander expander1(0);
  static SimpleMcpExpander expander4(4);
  expanders[0] = &expander1;
  expanders[1] = &expander4;

  Serial.println(F("Booting up"));
  for (auto& expander : expanders) {
    if (!expander) {
      break;
    }

    Serial.print(F("SimpleExpander: "));
    Serial.print(expander->address());
    Serial.print(' ');
    Serial.print(expander->currentInputs());
    Serial.print(' ');
    Serial.print(expander->previousInputs());
    Serial.print(' ');
    Serial.print(expander->outputs());
    Serial.print(' ');
    Serial.println();

    expander->begin();
  }

  Serial.println(F("Boot complete"));
    digitalWrite(13, HIGH);
}

void scanInputs()
{
  for (auto& expander : expanders) {
    if (!expander) {
      break;
    }

    expander->scanInputs();
    const uint8_t diff = expander->previousInputs() ^ expander->currentInputs();
    uint8_t action = 0;
    for (uint8_t pin = 0; pin<8; ++pin) {
      if (1 & (diff >> pin)) {
        if (1 & (expander->currentInputs() >> pin)) {
          action = pinMonitor.onPinChange(expander->address(), pin, High, ruleInterpreter);
        }
        else {
          action = pinMonitor.onPinChange(expander->address(), pin, Low, ruleInterpreter);
        }
        if (action) {
          executor.perform(action);
        }
      }
    }
  }
}

void tick()
{
  scanInputs();
  uint8_t action = pinMonitor.onTick(ruleInterpreter);
  if (action) {
    executor.perform(action);
  }
  executor.onTick();
}

void loop()
{
    tick();
    if (Serial.available() > 2) {
        int cmd  = Serial.read();
        int port = Serial.read() - '0';
        int pin  = Serial.read() - '0';

        String log(F("Command "));
        log += (char)cmd;
        log += ':';
        log += port;
        log += ',';
        log += pin;
        Serial.println(log);
    if (port >= 0 && pin >=0 && pin <= 7 && port <= 7) {
            switch (cmd) {
                case 't': case 'T':
                    executor.perform(port << 3 | pin);
                break;

                case 'r': case 'R':
          executor.perform(1 << 6 | port << 3 | pin);
                break;
            }
        }
    }
    LowPower.idle(SLEEP_60MS, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_ON, TWI_OFF);
}

