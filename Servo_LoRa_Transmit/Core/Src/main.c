/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 *
 *
 *RECEIVE RECEIVE RECEIVE RECEIVE RECEIVE RECEIVE RECEIVE RECEIVE RECEIVE RECEIVE
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#include "stdio.h"
#include "string.h"

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FIFO_OVERFLOW 1
#define NULL_POINTER 2
#define FIFO_SIZE 10      // Beispielgröße des FIFO-Puffers
#define PACKET_SIZE 1     // Beispielgröße eines Pakets

//typedef enum {
//    RADIO_TICK_SIZE_0015_US, // Beispiel für eine Zeitgröße, z. B. 0,015 Mikrosekunden
//    RADIO_TICK_SIZE_1_US,    // Beispiel für 1 Mikrosekunde
//    RADIO_TICK_SIZE_100_US,  // Beispiel für 100 Mikrosekunden
//    // Weitere Größen können hier hinzugefügt werden.
//} RadioTickSizes_t;
//
//typedef struct TickTime_s {
//    RadioTickSizes_t PeriodBase;   //!< Die Basiseinheit der Zeit (Tickgröße)
//    uint16_t PeriodBaseCount;      //!< Die Anzahl der PeriodBase-Einheiten
//} TickTime_t;


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
uint8_t txData_const = 0xC0;  // Beispiel: Opcode für GetStatus
uint8_t rxData_const = 0x00;  // Hier wird die Antwort gespeichert
char uart_buf[100];
int uart_buf_len;
uint8_t busy, nss, reset;
uint8_t tx_fifo[FIFO_SIZE][PACKET_SIZE];  // FIFO für Pakete
uint8_t *tx_eprt = tx_fifo[0];            // Zeiger auf das erste Paket
uint8_t tx_length = 0;                    // Anzahl der Pakete im FIFO
uint8_t tx_activated = 0;
uint8_t buf_out[100]; // Beispielgröße für buf_out, abhängig von deinem Bedarf
uint8_t rx_buffer[8]; // Empfangspuffer für Daten

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

void Erase_Flash(void) {
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SectorError;

	// Unlock the Flash
	HAL_FLASH_Unlock();

	// Configure the erase operation
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector = FLASH_SECTOR_0; // Specify sector to erase
	EraseInitStruct.NbSectors = 1;

	// Erase the sector
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
		// Handle error
	}

	// Lock the Flash
	HAL_FLASH_Lock();
}
void ResetChip(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
	HAL_Delay(20);
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
	HAL_Delay(20);
}

void SelectChip(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState state) {
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, state);
	HAL_Delay(2); // Falls notwendig
}
void SPI_TransmitReceiveWithDebug(SPI_HandleTypeDef* hspi, uint8_t* txData, uint8_t* rxData, uint16_t length, const char* debugMessage) {
	HAL_SPI_TransmitReceive(hspi, txData, rxData, length, HAL_MAX_DELAY);
	if (debugMessage) {
		HAL_UART_Transmit(&huart2, (uint8_t*)debugMessage, strlen(debugMessage), 100);
	}
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t ProcessStatusByte(uint8_t* statusByte) {
	// Extrahiere die Bits 7:5 (Circuit Mode) und 4:2 (Command Status)
	uint8_t circuitMode = (*statusByte >> 5) & 0x07; // Maske 0x07 für 3 Bits
	uint8_t commandStatus = (*statusByte >> 2) & 0x07; // Maske 0x07 für 3 Bits

	// Debug-Ausgabe für UART
	char uart_buf[50];
	int uart_buf_len = sprintf(uart_buf, "Circuit Mode: %u, Command Status: %u\r\n", circuitMode, commandStatus);
	HAL_UART_Transmit(&huart2, (uint8_t *)uart_buf, uart_buf_len, 100);

	// Verarbeitung von Circuit Mode
	switch (circuitMode) {
	case 0x2:
		// STDBY_RC
		HAL_UART_Transmit(&huart2, (uint8_t *)"Mode: STDBY_RC\r\n", 17, 100);
		break;
	case 0x3:
		// STDBY_XOSC
		HAL_UART_Transmit(&huart2, (uint8_t *)"Mode: STDBY_XOSC\r\n", 19, 100);
		break;
	case 0x4:
		// FS
		HAL_UART_Transmit(&huart2, (uint8_t *)"Mode: FS\r\n", 10, 100);
		break;
	case 0x5:
		// Rx
		HAL_UART_Transmit(&huart2, (uint8_t *)"Mode: Rx\r\n", 10, 100);
		break;
	case 0x6:
		// Tx
		HAL_UART_Transmit(&huart2, (uint8_t *)"Mode: Tx\r\n", 10, 100);
		break;
	default:
		HAL_UART_Transmit(&huart2, (uint8_t *)"Mode: Unknown\r\n", 16, 100);


	}

	// Verarbeitung von Command Status
	switch (commandStatus) {
	case 0x1:
		HAL_UART_Transmit(&huart2, (uint8_t *)"Command Status: Success\r\n", 26, 100);
		break;
	case 0x2:
		HAL_UART_Transmit(&huart2, (uint8_t *)"Command Status: Data Available\r\n", 33, 100);
		break;
	case 0x3:
		HAL_UART_Transmit(&huart2, (uint8_t *)"Command Status: Timeout\r\n", 26, 100);
		break;
	case 0x4:
		HAL_UART_Transmit(&huart2, (uint8_t *)"Command Status: Error\r\n", 24, 100);
		break;
	case 0x5:
		HAL_UART_Transmit(&huart2, (uint8_t *)"Command Status: Failure\r\n", 26, 100);
		break;
	case 0x6:
		HAL_UART_Transmit(&huart2, (uint8_t *)"Command Status: Tx Done\r\n", 26, 100);
		break;
	default:
		HAL_UART_Transmit(&huart2, (uint8_t *)"Command Status: Unknown\r\n", 27, 100);
	}
	return circuitMode;
}



void SPI_WaitUntilReady(SPI_HandleTypeDef *hspi) {
	while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET) {
		busy=uint8_t(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9));
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET); // Select the chip
		nss = uint8_t(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6));

		HAL_Delay(5);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET); // Select the chip
		nss = uint8_t(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6));
		HAL_Delay(5);
	}
	busy=uint8_t(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9));


}




void SPI_WaitTransmit(SPI_HandleTypeDef *hspi) {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET); // Select the chip
	nss = uint8_t(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6));
	while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) != GPIO_PIN_SET) {
		busy=uint8_t(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9));
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET); // Select the chip
		nss = uint8_t(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6));

		HAL_Delay(5); // Small delay to ensure stability
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET); // Select the chip
		nss = uint8_t(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6));



	}
	busy=uint8_t(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9));
	HAL_Delay(10); // Small delay to ensure stability

}
void GetStatus() {
	uint8_t command = 0xC0;  // Nur Low Byte wird verwendet
	uint8_t response = 0;

	// Sende 16-Bit-Datenrahmen (High Byte wird ignoriert)
	HAL_SPI_TransmitReceive(&hspi1, &command, &response, 1, HAL_MAX_DELAY);
	ProcessStatusByte(&response);
	// Extrahiere nur das Low Byte aus der Antwort
	HAL_Delay(10);


}

void SetStandby() {
	uint8_t command[2] = {0x80, 0x01};
	//uint8_t response[2]= {0x00, 0x00};
	// Sende und empfange 16-Bit-Datenrahmen
	// Zuerst Daten senden
	HAL_SPI_Transmit(&hspi1, (uint8_t*)command, 2, HAL_MAX_DELAY);

	// Dann die Antwort empfangen
	//HAL_SPI_Receive(&hspi1, (uint8_t*)&response, 2, HAL_MAX_DELAY);
	HAL_Delay(10);

}

// Funktion zum Aktivieren des LoRa Modus
void setLoRaMode() {
	//*(uint16_t*)tx = 0x8A | 0x01 << 8; // LoRa mode

	uint8_t tx[2] = {0x8A, 0x01}; // LoRa Mode aktivieren
	HAL_SPI_Transmit(&hspi1, tx, 2, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t *)"Set LoRa mode\r\n", 15, 100);
	HAL_Delay(10);

}
void getPacketType(SPI_HandleTypeDef* hspi) {
	//uint8_t txData;
	//*(uint32_t*)txData = 0x03 | 0x00 << 16; // LoRa mode
	uint8_t txData[3] = {0x03, 0x00, 0x00};  // Opcode = 0x03, NOP, NOP
	uint8_t rxData[3] = {0x00, 0x00, 0x00};

	// SPI-Übertragung und Empfang
	HAL_SPI_Transmit(&hspi1,txData, 3, HAL_MAX_DELAY);   // Lese die Antwort
	HAL_SPI_Receive(&hspi1, rxData, 3, HAL_MAX_DELAY);   // Lese die Antwort

	// SPI_TransmitReceiveWithDebug(hspi, txData[0], rxData, 3, "GetPacketType: ");
	// Der Pakettyp befindet sistatusch im dritten Byte (rxData[2])
	uint8_t packetType = rxData[2];

	// Debug-Ausgabe
	switch (packetType) {
	case 0x00:
		HAL_UART_Transmit(&huart2, (uint8_t *)"Packet Type: GFSK (0x00)\r\n", 30, 100);
		break;
	case 0x01:
		HAL_UART_Transmit(&huart2, (uint8_t *)"Packet Type: LoRa (0x01)\r\n", 30, 100);

		break;
	case 0x02:
		HAL_UART_Transmit(&huart2, (uint8_t *)"Packet Type: Ranging (0x02)\r\n", 30, 100);

		break;
	case 0x03:
		HAL_UART_Transmit(&huart2, (uint8_t *)"Packet Type: FLRC (0x03)\r\n", 30, 100);

		break;
	case 0x04:
		HAL_UART_Transmit(&huart2, (uint8_t *)"Packet Type: BLE (0x04)\r\n", 30, 100);

		break;
	default:
		HAL_UART_Transmit(&huart2, (uint8_t *)"Unknown Packet Type:", 20, 100);
		HAL_UART_Transmit(&huart2, &packetType, 1, 100);
		HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n", 4, 100);

		break;
	}
}


// Funktion zum Setzen der Frequenz
void setFrequency() {
	uint8_t tx[4] = {0x86, 0xB8, 0x9D, 0x89}; // Frequenzdaten
	uint8_t rx[4] = {0};
	SPI_TransmitReceiveWithDebug(&hspi1, tx, rx, 4, "Set Frequency\r\n");  // SPI mit Debugger
	HAL_Delay(10);

}

// Funktion zum Setzen der Basisadresse des Buffers
void setBufferBaseAddress() {
	uint8_t tx[3] = {0x8F, 0x80, 0x00}; // Basisadresse TODO
	uint8_t rx[3] = {0};
	SPI_TransmitReceiveWithDebug(&hspi1, tx, rx, 3, "Set Buffer Base Address\r\n");  // SPI mit Debugger
	HAL_Delay(10);
}

// Funktion zum Setzen der Modulationsparameter
void setModulationParams() {
	uint8_t tx[4] = {0x8B, 0x70, 0x26, 0x01}; // Modulationsparameter
	uint8_t rx[4] = {0};
	SPI_TransmitReceiveWithDebug(&hspi1, tx, rx, 4, "Set Modulation Params\r\n");  // SPI mit Debugger
	HAL_Delay(10);
}

// Funktion zum Setzen der Paketparameter
void setPacketParams() {
	uint8_t tx[8] = {0x8C, 0x0C, 0x80, 0x01, 0x20, 0x40, 0x00, 0x00}; // Paketparameter
	uint8_t rx[8] = {0};
	SPI_TransmitReceiveWithDebug(&hspi1, tx, rx, 8, "Set Packet Params\r\n");  // SPI mit Debugger
	HAL_Delay(10);
}

// Funktion zum Setzen der RX-Periode (Empfangsparameter)
void setTxParams() {
	uint8_t tx[3] = {0x8E, 0x1F, 0x04}; // TX Periode
	uint8_t rx[3] = {0};
	SPI_TransmitReceiveWithDebug(&hspi1, tx, rx, 3, "Set TX Period\r\n");  // SPI mit Debugger
	HAL_Delay(10);
}
void setTx() {
	uint8_t tx[4] = {0x83, 0x00, 0x00, 0x00}; // TX Periode
	uint8_t rx[4] = {0};
	SPI_TransmitReceiveWithDebug(&hspi1, tx, rx, 4, "Set TX\r\n");  // SPI mit Debugger
	HAL_Delay(10);
}
void ClrIrqStatus() {
	uint8_t tx[3] = {0x97, 0xFF, 0xFF}; // Opcode für ClrIrqStatus und Maske
	uint8_t rx[3] = {0};

	// Sende SPI-Befehl
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 3, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t *)"ClrIrqStatus sent.\r\n", 20, HAL_MAX_DELAY);
	HAL_Delay(10);
}

//void setDioIrqParams(uint16_t irqMask, uint16_t dioMask, uint16_t dio1Mask, uint16_t dio2Mask) {
//	uint8_t tx[9] = {0};
//
//	// SPI-Befehl: SetDioIrqParams
//	tx[0] = 0x8D;               // Opcode für SetDioIrqParams
//	tx[1] = (irqMask >> 8);     // IRQ-Maske (High-Byte)
//	tx[2] = (irqMask & 0xFF);   // IRQ-Maske (Low-Byte)
//	tx[3] = (dioMask >> 8);     // DIO-Maske (High-Byte)
//	tx[4] = (dioMask & 0xFF);   // DIO-Maske (Low-Byte)
//	tx[5] = (dio1Mask >> 8);    // DIO1-Maske (High-Byte)
//	tx[6] = (dio1Mask & 0xFF);  // DIO1-Maske (Low-Byte)
//	tx[7] = (dio2Mask >> 8);    // DIO2-Maske (High-Byte)
//	tx[8] = (dio2Mask & 0xFF);  // DIO2-Maske (Low-Byte)
//
//	// Sende SPI-Befehl
//	HAL_SPI_Transmit(&hspi1, tx, 9, HAL_MAX_DELAY);
//	HAL_UART_Transmit(&huart2, (uint8_t *)"SetDioIrqParams sent.\r\n", 23, HAL_MAX_DELAY);
//	HAL_Delay(10);
//}

bool checkIrqStatus(uint16_t irqMask) {
	uint8_t tx[4] = {0x15, 0x00, 0x00, 0x00}; // Opcode für GetIrqStatus
	uint8_t rx[4] = {0};

	// SPI-Befehl senden und IRQ-Status abrufen
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 4, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t *)"GetIrqStatus received.\r\n", 25, HAL_MAX_DELAY);

	// Kombiniere die empfangenen Bytes in ein 16-Bit-Ergebnis
	uint16_t irqStatus = (rx[1] << 8) | rx[2];

	// Debug-Ausgabe
	char debugMsg[50];
	snprintf(debugMsg, sizeof(debugMsg), "IRQ Status: 0x%04X\r\n", irqStatus);
	HAL_UART_Transmit(&huart2, (uint8_t *)debugMsg, strlen(debugMsg), HAL_MAX_DELAY);

	// Überprüfe, ob die gewünschten IRQ-Bits gesetzt sind
	return (irqStatus & irqMask) != 0;
}
void DerTakt()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);   // NSS auf HIGH
	nss = uint8_t(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6));
	HAL_Delay(2);
	SPI_WaitUntilReady(&hspi1); // BUSY auf 0
	HAL_Delay(2);
	SPI_WaitTransmit(&hspi1); //  NSS auf 0, BUSY auf 1

}

void WriteBuffer(uint8_t offset, uint8_t *data, uint8_t length) {
	uint8_t txBuffer[length + 2];
	uint8_t rxBuffer[length + 2];  // Empfangspuffer für Status

	// Erstelle das Sendepaket
	txBuffer[0] = 0x1A;  // Opcode
	txBuffer[1] = offset;               // Offset
	for (uint8_t i = 0; i < length; i++) {
		txBuffer[i + 2] = data[i];      // Nutzdaten
	}

	// Sende das Paket und empfange Status
	HAL_SPI_Transmit(&hspi1, txBuffer, length + 2, HAL_MAX_DELAY);

	// Statusprüfung (optional)

	HAL_UART_Transmit(&huart2, (uint8_t*)"Buffer written\r\n", 15, HAL_MAX_DELAY);
	HAL_Delay(10);

}

void ReadBuffer(SPI_HandleTypeDef* hspi, UART_HandleTypeDef* huart, uint8_t payloadLength) {
	uint8_t tx[11] = {0x1B, 0x80, 0x00, 0x00, 0x00, 0x00 ,0x00, 0x00, 0x00, 0x00, 0x00}; // Kommando: ReadBuffer, Offset
	uint8_t rx[11] = {0x00, 0x00};

	char buf[50];
	// SPI-Übertragung
	HAL_SPI_TransmitReceive(hspi, tx, rx, payloadLength + 3, HAL_MAX_DELAY);
	int uart_buf_len = sprintf(buf, "Received Data: %0x , %0x, %0x, %0x, %0x, %0x, %0x, %0x\r\n", rx[3], rx[4], rx[5], rx[6], rx[7], rx[8], rx[9], rx[10] );
	HAL_UART_Transmit(huart, (uint8_t*)buf, uart_buf_len, HAL_MAX_DELAY);

	// Zeilenumbruch nach den Daten
}
void SendPayload( uint8_t offset, uint8_t *data, uint8_t length) {
	WriteBuffer(offset, data, length);
	DerTakt();
	ReadBuffer(&hspi1, &huart2, length);
	//	setTx();
	//    DerTakt();

}
void setDioIrqParamsForTxDone() {
	uint8_t tx[9] = {
			0x8D,       // Befehl: SetDioIrqParams
			0x40, 0x23, // IRQ-Masken: Aktiviert nur TxDone (Bit 3)
			0x00, 0x01, // Keine weiteren IRQs
			0x00, 0x02, // Keine weiteren IRQs
			0x40, 0x20  // DIO-Mapping: TxDone auf DIO1, andere DIOs Standard
	};
	uint8_t rx[9] = {0};

	SPI_TransmitReceiveWithDebug(&hspi1, tx, rx, 9, "Set DIO IRQ Params for TxDone\r\n");
	HAL_Delay(10);
}
// Die Funktion zum Einfügen eines Pakets
uint8_t PutPacket(uint8_t* in)
{
	uint8_t offset = 0x80;

	SendPayload(offset, in, PACKET_SIZE);

	// Verzögerung, um sicherzustellen, dass genug Zeit für die Übertragung bleibt
	HAL_Delay(100);  // Delay erhöht für eine stabilere Übertragung

	return 0;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	//Erase_Flash();

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
	//HAL_TIM_Base_Start(&htim1);
	ResetChip(GPIOC, GPIO_PIN_7);
	HAL_Delay(5);

	/*=======================================================================*/
	//NSSNSSNSSNSSNSSNSSNSSNSSNSSNSSNSSNSSNSSNSS
	/*=======================================================================*/

	// NSS Test
	SelectChip(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
	nss = uint8_t(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6));

	HAL_Delay(5);

	SelectChip(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	nss = uint8_t(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6));

	HAL_Delay(5);

	SelectChip(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
	nss = uint8_t(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6));
	//SPI_WaitUntilReady(&hspi1);

	SPI_WaitTransmit(&hspi1);


	SetStandby();
	HAL_Delay(5);

	DerTakt();


	uint8_t irq_status[4] = {0x15, 0x00, 0x00, 0x00};  // Read IRQ Status command
	uint8_t irq_response[4];
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		uint8_t data_180[PACKET_SIZE] = {30};  // Beispiel-Daten

		GetStatus();               // Status des Transceivers abrufen
		DerTakt();

		setLoRaMode();             // LoRa-Modus aktivieren
		DerTakt();

		getPacketType(&hspi1);     // Pakettyp abrufen
		DerTakt();

		setFrequency();            // Frequenz setzen
		DerTakt();

		setBufferBaseAddress();    // Basisadresse des Buffers setzen
		DerTakt();

		setModulationParams();     // Modulationsparameter konfigurieren
		DerTakt();

		setPacketParams();         // Paketparameter einstellen
		DerTakt();
		// Paketparameter setzen
		setTxParams();
		DerTakt();

		//while(1){
		PutPacket(data_180);
		DerTakt();

		setDioIrqParamsForTxDone();
		DerTakt();

		setTx();
		DerTakt();

		// Warte auf TxDone oder Timeout
		// Warte auf TxDone IRQ
		do {
		    HAL_SPI_TransmitReceive(&hspi1, irq_status, irq_response, sizeof(irq_status), HAL_MAX_DELAY);
		    char msg[50];
		    snprintf(msg, sizeof(msg), "IRQ: %02x\r\n", irq_response[3]); // Format the message
		    HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY); // Send the formatted message
			DerTakt();


		} while ((irq_response[3] & 0x01) == 0);   // RxTxTimeout bit

		// IRQ wurde erkannt
		HAL_UART_Transmit(&huart2, (uint8_t *)"Packet sent successfully.\r\n", 28, HAL_MAX_DELAY);
		// Lösche IRQ-Status
		ClrIrqStatus();
		DerTakt();

		HAL_Delay(5000); // Wiederhole nach 1 Sekunde
		HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY); // Debug-Ausgabe


	}

	//HAL_Delay(500);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
