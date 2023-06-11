# can-to-go-system Software 
## System
- microcontroller: esp32
- framework: espidf

## System Architecture
```mermaid
  flowchart TB
    Starting[Starting]
    Configuration[Configuration];
    Validation[Validation]
    Listening[Listening];
    Starting --> Configuration --> Validation --> Listening;
```

## States
### Starting
```mermaid
sequenceDiagram
    participant FSM
    participant UART
    participant CAN
    participant DISPLAY
    participant LEDS

    FSM->>UART: start Task
    FSM->>CAN: start Task
    FSM->>DISPLAY: start Task
    FSM->>LEDS: start Task
```

### Configuration
```mermaid
sequenceDiagram
    participant FSM
    participant UART
    participant CAN
    participant DISPLAY
    participant LEDS

    FSM->>DISPLAY: get Baudrate
    DISPLAY-->>FSM: Baudrate 
    FSM->>CAN: set Baudrate
```

### Validation
```mermaid
sequenceDiagram
    participant FSM
    participant UART
    participant CAN
    participant DISPLAY
    participant LEDS

    FSM->>CAN: trigger validation state [Queue]
    CAN->>CAN: send validation message
    CAN-->>DISPLAY: result validation message [Queue]
    CAN-->>LEDS: result validation message [Queue]
    CAN-->>UART: result validation message [Queue]
    DISPLAY->>DISPLAY: show validation result
    LEDS->>LEDS: show validation result
    UART->>UART: show validation result
```

### Listening
```mermaid
sequenceDiagram
    participant FSM
    participant UART
    participant CAN
    participant DISPLAY
    participant PERIPHERIE

    FSM->>CAN: trigger listening state [Queue]
    CAN->>CAN: listen to messages
    CAN-->>DISPLAY: send message [Queue]
    CAN-->>LEDS: send message [Queue]
    CAN-->>UART: send message [Queue]
    DISPLAY->>DISPLAY: show message
    LEDS->>LEDS: show message blink
    UART->>UART: show message
```