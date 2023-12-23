# can-to-go-system Software 
## System
- microcontroller: esp32
- framework: espidf

## System Architecture
```mermaid
  flowchart TB
    Starting[Starting]
    Configuration[Configuration];
    Operation[Operation]
    Starting --> Configuration --> Operation
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
    participant REST_API

    FSM->>FSM: set Starting state
    FSM->>UART: start Task
    FSM->>DISPLAY: start Task
    FSM->>LEDS: start Task
    FSM->>REST_API: start Task
```

### Configuration
```mermaid
sequenceDiagram
    participant FSM
    participant UART
    participant CAN
    participant DISPLAY
    participant LEDS
    participant REST_API

    FSM->>FSM: set Configuration state
    UART->>UART: send logs
    FSM->>DISPLAY: get Baudrate
    DISPLAY-->>FSM: Baudrate 

    FSM->>CAN: start CAN with configuration
```

### Operation
```mermaid
sequenceDiagram
    participant FSM
    participant UART
    participant CAN
    participant DISPLAY
    participant LEDS
    participant REST_API

    FSM->>FSM: set Operation state
    UART->>UART: send logs
    loop
        CAN->>CAN: listen to messages
        CAN->>CAN: test CAN bus
        CAN-->>LEDS: can_repository trigger led for CAN bus test result [led_status_queue]
        LEDS->>LEDS: blink on received CAN message
        LEDS->>LEDS: blink on send CAN message
        LEDS->>LEDS: trigger LED for test CAN bus result
    end
    opt on message received
        CAN-->>DISPLAY: can_repository send received message [display_messages_queue]
        CAN-->>REST_API: can_repository send received message [rest_api_messages_queue]
        CAN-->>LEDS: can_repository trigger led [led_status_queue]
        DISPLAY->>DISPLAY: update display with received messages
        DISPLAY->>DISPLAY: update rest api with received messages
    end
    opt on message send
        CAN-->>LEDS: can_repository trigger led [led_status_queue]
    end
```