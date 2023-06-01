/*
 * ina219.cpp
 *
 *  Created on: Mar 12, 2023
 *      Author: ADMIN
 */

#include "ina219.h"

/*!
 *  @brief  Instantiates a new INA219 class
 *  @param addr the I2C address the device can be found on. Default is 0x40
 */
Ina219::Ina219(I2C_HandleTypeDef* hi2c, TIM_HandleTypeDef* htim, uint8_t addr) {
	this->hi2c = hi2c;
	this->addr = addr << 1;
	this->htim = htim;

	flagTxDone = HAL_OK;
	stage = 0;
	//	timeLife = 0;
	//	currentLife = 0;
	//
	//	pre_time = 0;

	flagSTART = HAL_ERROR;
	flagPOWER = HAL_ERROR;
}

//Ina219::Ina219(I2C_HandleTypeDef* hi2c, TIM_HandleTypeDef* htim, Terminal* terminal, uint8_t addr) {
//	this->hi2c = hi2c;
//	this->addr = addr << 1;
//	this->htim = htim;
//	this->terminal = terminal;
//
//	flagTxDone = HAL_OK;
//	stage = 0;
//}

/*!
 *  @brief INA219 class destructor
 */
Ina219::~Ina219() {
}
//
//HAL_StatusTypeDef Ina219::init(	INA219_Config_Bvoltagerange BusVolRange,
//		INA_219_Config_Gain PGA,
//		INA219_Config_Badcres BADC,
//		INA219_Config_Sadcres SADC,
//		INA219_Config_Mode OperaMode,
//		uint16_t Cal) {
//	if (stage == 0) {
//		if (HAL_I2C_IsDeviceReady(hi2c, addr, 10, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
//		stage++;
//	}
//
//	uint8_t pData[2];
//	if (stage == 1) {
//		pData[0] = 0x80;
//		if (HAL_I2C_Mem_Write(hi2c, addr, INA219_REG_CONFIG, I2C_MEMADD_SIZE_8BIT, pData, 1, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
//		Configuration = 0x399fU;
//		Calibration = 0x0000U;
//		pointer = INA219_REG_NO;
//		stage++;
//	}
//
//	if (stage == 2) {
//		uint16_t Configuration_Pre = Configuration;
//		if (BusVolRange != INA219_CONFIG_BVOLTAGERANGE_32V) Configuration_Pre = (Configuration_Pre & 0xdfff) | BusVolRange;
//		if (PGA != INA219_CONFIG_GAIN_8_320MV) Configuration_Pre = (Configuration_Pre & 0xe7ff) | PGA;
//		if (BADC != INA219_CONFIG_BADCRES_12BIT) Configuration_Pre = (Configuration_Pre & 0xf87f) | BADC;
//		if (SADC != INA219_CONFIG_SADCRES_12BIT_1S_532US) Configuration_Pre = (Configuration_Pre & 0xff87) | SADC;
//		if (OperaMode != INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS) Configuration_Pre = (Configuration_Pre & 0xfff8) | OperaMode;
//		if (Configuration_Pre != Configuration) {
//			pData[0] = Configuration_Pre >> 8;
//			pData[1] = Configuration_Pre;
//			if (HAL_I2C_Mem_Write(hi2c, addr, INA219_REG_CONFIG, I2C_MEMADD_SIZE_8BIT, pData, 2, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
//			Configuration = Configuration_Pre;
//		}
//		stage++;
//	}
//
//	if (stage == 3) {
//		if (Cal != Calibration) {
//			pData[0] = Cal >> 8;
//			pData[1] = Cal;
//			if (HAL_I2C_Mem_Write(hi2c, addr, INA219_REG_CALIBRATION, I2C_MEMADD_SIZE_8BIT, pData, 2, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
//			Calibration = Cal;
//		}
//		stage++;
//	}
//
//	if (stage == 4){
//		if (HAL_I2C_Mem_Read(hi2c, addr, INA219_REG_SHUNTVOLTAGE, I2C_MEMADD_SIZE_8BIT, pData, 2, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
//		ShuntVol = (((uint16_t)pData[0]) << 8) | ((uint16_t)pData[1]);
//		stage++;
//	}
//
//	if (stage == 5){
//		if (HAL_I2C_Mem_Read(hi2c, addr, INA219_REG_BUSVOLTAGE, I2C_MEMADD_SIZE_8BIT, pData, 2, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
//		if ((pData[1] & 0x01) == 0x01) return HAL_ERROR;
//		BusVol = (((uint16_t)pData[0]) << 8) | ((uint16_t)pData[1]);
//		stage++;
//	}
//
//	if (stage == 6){
//		if (HAL_I2C_Mem_Read(hi2c, addr, INA219_REG_POWER, I2C_MEMADD_SIZE_8BIT, pData, 2, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
//		Power = (((uint16_t)pData[0]) << 8) | ((uint16_t)pData[1]);
//		stage++;
//	}
//
//	if (stage == 7){
//		if (HAL_I2C_Mem_Read(hi2c, addr, INA219_REG_CURRENT, I2C_MEMADD_SIZE_8BIT, pData, 2, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
//		Current = (((uint16_t)pData[0]) << 8) | ((uint16_t)pData[1]);
//
//		uint32_t time = __HAL_TIM_GET_COUNTER(htim);
//		uint64_t time_space;
//		if (time < pre_time) time_space = time + (0xffffU - pre_time);
//		else time_space = time - pre_time;
//
//		timeLife += time_space;
//		currentLife += time_space * Current;
//		pre_time = time;
//		stage = 4;
//	}
//
//	return HAL_OK;
//}

void Ina219::run_DMA(	INA219_Config_Bvoltagerange BusVolRange,
		INA_219_Config_Gain PGA,
		INA219_Config_Badcres BADC,
		INA219_Config_Sadcres SADC,
		INA219_Config_Mode OperaMode,
		uint16_t Cal) {
	static uint16_t Configuration = 0x0000U;

	if (stage == 0) {

		this->timer_clk = HAL_RCC_GetSysClockFreq() / (this->htim->Init.Prescaler + 1);

		if (HAL_I2C_IsDeviceReady(hi2c, addr, 10, HAL_MAX_DELAY) != HAL_OK) return;

		Configuration = Configuration | BusVolRange | PGA | BADC | SADC | OperaMode;

		pData[0] = Configuration >> 8;
		pData[1] = Configuration;

		stage++;
	}

	if (stage == 1) {
		HAL_I2C_Mem_Write_DMA(hi2c, addr, INA219_REG_CONFIG, I2C_MEMADD_SIZE_8BIT, pData, sizeof(pData));
		flagTxDone = HAL_BUSY;
		stage++;
	}

	if (stage == 2) {
		if (flagTxDone != HAL_OK) return;

		pData[0] = Cal >> 8;
		pData[1] = Cal;
		HAL_I2C_Mem_Write_DMA(hi2c, addr, INA219_REG_CALIBRATION, I2C_MEMADD_SIZE_8BIT, pData, sizeof(pData));
		flagTxDone = HAL_BUSY;
		stage++;
	}

	if (stage == 3) {
		if (flagTxDone != HAL_OK) return;
		stage++;
	}

	if (stage == 4) {
		if (flagSTART != HAL_OK) {
			funcStart();
		}

		if (flagPOWER == HAL_OK) {
			funcPower();
		}
	}
}

void Ina219::IRQHandler() {
	if (pRxListData.size() < 1) return;

	uint8_t* data = pRxListData[0].data();

	uint8_t start[6] = {'S', 'T', 'A', 'R', 'T', '='};
	uint8_t power[6] = {'P', 'O', 'W', 'E', 'R', '='};

	if (memcmp(data, start, 6) == 0) {
		flagSTART = HAL_OK;
		timeMax = (uint64_t)atoi((char* )data + 6) * timer_clk * 60;
		pRxListData.pop_front();

		std::vector<uint8_t> ok = { 'O', 'K', '\n'};
		pTxListData.push_back(ok);

		timePre = __HAL_TIM_GET_COUNTER(htim);
		timeCounter = 0;

		totalPower = 0;
	}

	if (memcmp(data, power, 6) == 0) {
		flagPOWER = HAL_OK;
	}
}

void Ina219::funcStart() {

	uint32_t time = __HAL_TIM_GET_COUNTER(htim);

	std::vector<uint8_t> sendData;
	sendData.push_back('T');
	sendData.push_back('E');

	if (HAL_I2C_Mem_Read(hi2c, addr, INA219_REG_SHUNTVOLTAGE, I2C_MEMADD_SIZE_8BIT, pData, sizeof(pData), HAL_MAX_DELAY) != HAL_OK) return;
	ShuntVol = (((uint16_t)pData[0]) << 8) | ((uint16_t)pData[1]);

	if (HAL_I2C_Mem_Read(hi2c, addr, INA219_REG_BUSVOLTAGE, I2C_MEMADD_SIZE_8BIT, pData, sizeof(pData), HAL_MAX_DELAY) != HAL_OK) return;
	if ((pData[1] & 0x01) == 0x01) return;
	if ((pData[1] & 0x02) == 0x00) return;
	BusVol = (((uint16_t)pData[0]) << 5) | ((uint16_t)pData[1] >> 3);
	sendData.push_back(BusVol);
	sendData.push_back(BusVol >> 8);

	if (HAL_I2C_Mem_Read(hi2c, addr, INA219_REG_POWER, I2C_MEMADD_SIZE_8BIT, pData, 2, HAL_MAX_DELAY) != HAL_OK) return;
	Power = (((uint16_t)pData[0]) << 8) | ((uint16_t)pData[1]);
	sendData.push_back(pData[1]);
	sendData.push_back(pData[0]);

	if (HAL_I2C_Mem_Read(hi2c, addr, INA219_REG_CURRENT, I2C_MEMADD_SIZE_8BIT, pData, 2, HAL_MAX_DELAY) != HAL_OK) return;
	Current = (((uint16_t)pData[0]) << 8) | ((uint16_t)pData[1]);
	sendData.push_back(pData[1]);
	sendData.push_back(pData[0]);

	sendData.push_back('\r');
	sendData.push_back('\n');

	pTxListData.push_back(sendData);

	uint64_t time_space;
	if (time < timePre) time_space = time + (0xffffU - timePre);
	else time_space = time - timePre;
	timePre = time;

	timeCounter += time_space;
	totalPower += time_space * Power;
	if (timeCounter < timeMax) return;

	uint8_t total[80];
	//	sprintf((char* )total, "Total of POWER: 0x%016llX\n", totalPower);
	sprintf((char* )total, "Total of Power: %llu\nTotal of Time: %llu\n", totalPower, timeCounter);
	std::vector<uint8_t> total_v(total, total + strlen((char* ) total));
	//	pTxListData.push_back(total_v);

	flagSTART = HAL_ERROR;
}

void Ina219:: funcPower() {
	uint8_t total[80];
	sprintf((char* )total, "Total of Power: %llu\nTotal of Time: %llu\n", totalPower, timeCounter);
	std::vector<uint8_t> total_v(total, total + strlen((char* ) total));
	flagPOWER = HAL_ERROR;
}

//HAL_StatusTypeDef Ina219::setBusVolRange(INA219_Config_Bvoltagerange BusVolRange){
//	uint16_t Configuration_Pre = (Configuration & 0xdfff) | BusVolRange;
//	if (write(INA219_REG_CONFIG, Configuration_Pre, 1) != HAL_OK) return HAL_ERROR;
//	return HAL_OK;
//}
//
//HAL_StatusTypeDef Ina219::setPGA(INA_219_Config_Gain PGA) {
//	uint16_t Configuration_Pre = (Configuration & 0xe7ff) | PGA;
//	if (write(INA219_REG_CONFIG, Configuration_Pre, 1) != HAL_OK) return HAL_ERROR;
//	return HAL_OK;
//}
//HAL_StatusTypeDef Ina219::setBADC(INA219_Config_Badcres BADC) {
//	uint16_t Configuration_Pre = (Configuration & 0xf87f) | BADC;
//	if (write(INA219_REG_CONFIG, Configuration_Pre, 2) != HAL_OK) return HAL_ERROR;
//	return HAL_OK;
//}
//HAL_StatusTypeDef Ina219::setSADC(INA219_Config_Sadcres SADC) {
//	uint16_t Configuration_Pre = (Configuration & 0xff87) | SADC;
//	if (write(INA219_REG_CONFIG, Configuration_Pre, 2) != HAL_OK) return HAL_ERROR;
//	return HAL_OK;
//}
//HAL_StatusTypeDef Ina219::setOperaMode(INA219_Config_Mode OperaMode) {
//	uint16_t Configuration_Pre = (Configuration & 0xfff8) | OperaMode;
//	if (write(INA219_REG_CONFIG, Configuration_Pre, 2) != HAL_OK) return HAL_ERROR;
//	return HAL_OK;
//}
//HAL_StatusTypeDef Ina219::write(INA219_Reg mode, uint16_t pData, uint8_t size) {
//	if (mode == INA219_REG_CONFIG) {
//		if (Configuration == pData) return HAL_OK;
//	} else if (mode == INA219_REG_CALIBRATION) {
//		if (Calibration == pData) return HAL_OK;
//	} else return HAL_ERROR;
//
//	uint8_t pTxData[size];
//	for (uint8_t i = 0; i < size; i++) pTxData[i] = pData >> ((size - 1 - i) * 8);
//
//	if (HAL_I2C_Mem_Write(hi2c, addr, mode, I2C_MEMADD_SIZE_8BIT, pTxData, size, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
//
//	if (mode == INA219_REG_CONFIG) Configuration = pData;
//	else Calibration = pData;
//
//	return HAL_OK;
//}
//
//HAL_StatusTypeDef Ina219::read(INA219_Reg mode, uint8_t* pData, uint8_t size){
//
//	/* Check address pointer	 */
//	if (pointer != mode){
//		if (HAL_I2C_Mem_Read(hi2c, addr, mode, I2C_MEMADD_SIZE_8BIT, pData, size, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
//		pointer = mode;
//	} else {
//		if (HAL_I2C_Master_Receive(hi2c, addr, pData, size, HAL_MAX_DELAY) != HAL_OK) return HAL_ERROR;
//	}
//	return HAL_OK;
//}

//HAL_StatusTypeDef Ina219::setCalibration(uint16_t Calibration) {
//	if (write(INA219_REG_CALIBRATION, Calibration, 2) != HAL_OK) return HAL_ERROR;
//	return HAL_OK;
//}

//HAL_StatusTypeDef Ina219::reset(){
//	uint8_t rst = 0x80;
//	/* Wait 		 */
//	while (flagTxDone == HAL_OK);
//	if (HAL_I2C_Mem_Write_DMA(hi2c, addr, INA219_REG_CONFIG, I2C_MEMADD_SIZE_8BIT, &rst, 1U) != HAL_OK) return HAL_ERROR;
//	flagTxDone = HAL_BUSY;
//
//	Configuration = 0x399fU;
//	Calibration = 0x0U;
//	pointer = INA219_REG_NO;
//	return HAL_OK;
//}

//HAL_StatusTypeDef Ina219::getBusVolReg() {
//	uint8_t pRxData[4];
//	HAL_Delay(100U);
//
//	if (read(INA219_REG_SHUNTVOLTAGE, pRxData + 2, 2) != HAL_OK) return HAL_ERROR;
//
//	if (read(INA219_REG_BUSVOLTAGE, pRxData + 4, 2) != HAL_OK) return HAL_ERROR;
//
//	if (read(INA219_REG_POWER, pRxData + 6, 2) != HAL_OK) return HAL_ERROR;
//
//	if (read(INA219_REG_CURRENT, pRxData + 8, 4) != HAL_OK) return HAL_ERROR;
//
//	//	if (read(INA219_REG_CALIBRATION, pRxData + 10, 2) != HAL_OK) return HAL_ERROR;
//	//	HAL_Delay(100U);
//	//	if ((pRxData[5] & 0x03) != 0x02) return HAL_ERROR;
//	//	BusVol = ((uint16_t)pRxData[0] << 8) | ((uint16_t)pRxData[1]);
//	//	if (read(INA219_REG_POWER, pRxData, 2) != HAL_OK) return HAL_ERROR;
//	return HAL_OK;
//}

//HAL_StatusTypeDef Ina219::getCurrentReg(){
//	uint8_t pRxData[2];
//	if (read(INA219_REG_BUSVOLTAGE, pRxData, 2) != HAL_OK) return HAL_ERROR;
//	if ((pRxData[1] & 0x01) == 0x01) return HAL_ERROR;
//	if (read(INA219_REG_CURRENT, pRxData, 2) != HAL_OK) return HAL_ERROR;
//	Current = ((uint16_t)pRxData[0] << 8) | ((uint16_t)pRxData[1]);
//	return HAL_OK;
//}

//float Ina219::getCurrent() {
//	return (float)Current * 4096U * 100 / Calibration;
//}

void Ina219::setListData(std::vector<uint8_t>* data) {
	pRxListData.push_back(*data);
}
std::deque<std::vector<uint8_t>>* Ina219::getListData(){
	return &pTxListData;
}

void Ina219::memTxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (this->hi2c->Instance != hi2c->Instance) return;
	flagTxDone = HAL_OK;
}

void Ina219::errorCallback(I2C_HandleTypeDef *hi2c) {
	if (this->hi2c->Instance != hi2c->Instance) return;
	if (stage <= 3) stage--;
}

/*!
 *  @brief  Configures to INA219 to be able to measure up to 32V and 2A
 *          of current.  Each unit of current corresponds to 100uA, and
 *          each unit of power corresponds to 2mW. Counter overflow
 *          occurs at 3.2A.
 *  @note   These calculations assume a 0.1 ohm resistor is present
 */

// By default we use a pretty huge range for the input voltage,
// which probably isn't the most appropriate choice for system
// that don't use a lot of power.  But all of the calculations
// are shown below if you want to change the settings.  You will
// also need to change any relevant register settings, such as
// setting the VBUS_MAX to 16V instead of 32V, etc.

// VBUS_MAX = 32V             (Assumes 32V, can also be set to 16V)
// VSHUNT_MAX = 0.32          (Assumes Gain 8, 320mV, can also be 0.16, 0.08,
// 0.04) RSHUNT = 0.1               (Resistor value in ohms)

// 1. Determine max possible current
// MaxPossible_I = VSHUNT_MAX / RSHUNT
// MaxPossible_I = 3.2A

// 2. Determine max expected current
// MaxExpected_I = 2.0A

// 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
// MinimumLSB = MaxExpected_I/32767
// MinimumLSB = 0.000061              (61uA per bit)
// MaximumLSB = MaxExpected_I/4096
// MaximumLSB = 0,000488              (488uA per bit)

// 4. Choose an LSB between the min and max values
//    (Preferrably a roundish number close to MinLSB)
// CurrentLSB = 0.0001 (100uA per bit)

// 5. Compute the calibration register
// Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
// Cal = 4096 (0x1000)

// 6. Calculate the power LSB
// PowerLSB = 20 * CurrentLSB
// PowerLSB = 0.002 (2mW per bit)

// 7. Compute the maximum current and shunt voltage values before overflow
//
// Max_Current = Current_LSB * 32767
// Max_Current = 3.2767A before overflow
//
// If Max_Current > Max_Possible_I then
//    Max_Current_Before_Overflow = MaxPossible_I
// Else
//    Max_Current_Before_Overflow = Max_Current
// End If
//
// Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
// Max_ShuntVoltage = 0.32V
//
// If Max_ShuntVoltage >= VSHUNT_MAX
//    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
// Else
//    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
// End If

// 8. Compute the Maximum Power
// MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
// MaximumPower = 3.2 * 32V
// MaximumPower = 102.4W
