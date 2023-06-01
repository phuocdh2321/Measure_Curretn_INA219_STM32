/*
 * ina219.h
 *
 *  Created on: Mar 12, 2023
 *      Author: ADMIN
 */

#ifndef INA219_H_
#define INA219_H_

#include "stm32f1xx_hal.h"

#include "terminal.h"
#include "deque"
#include "vector"
#include <cstring>
//#include "string.h"

/** calculated I2C address: 0 = GND, 1 = V+ **/

/* The address is controlled by the A0 and A1 inputs on the INA219:
 *
 * Calculated address: b100ABCD
 * A0 controls C and D: GND = 00, V+ = 01, SDA = 10, SCL = 11
 * A1 controls A and B: GND = 00, V+ = 01, SDA = 10, SCL = 11
 *
 * E.g. if A0 is tied to ground and A1 is tied to V+,
 * the resulting address is b1000100 = 0x44
 *
 * SDA and SCL options aren't implemented.
 */

enum {
	GND = 0x00U,
	VS 	= 0x01U,
	SDA	= 0x02U,
	SCL = 0x03U,
};

/** default I2C address **/
#define INA219_ADDRESS(__INA_ADDR0__, __INA_ADDR1__) (0x40U | __INA_ADDR0__ | (__INA_ADDR1__ << 2))

/** read **/
#define INA219_READ (0x01)

/*=========================================================================
    CONFIG REGISTER (R/W)
 **************************************************************************/
/* Summary of Register Set */
typedef enum {
	INA219_REG_CONFIG 							= 0x00U,	/** config register address **/
	INA219_REG_SHUNTVOLTAGE 					= 0x01U,	/** shunt voltage register **/
	INA219_REG_BUSVOLTAGE 						= 0x02U,	/** bus voltage register **/
	INA219_REG_POWER 							= 0x03U,	/** power register **/
	INA219_REG_CURRENT 							= 0x04U,	/** current register **/
	INA219_REG_CALIBRATION 						= 0x05U,	/** calibration register **/
} INA219_Reg;

/** reset bit **/
#define INA219_CONFIG_RESET						(0x8000U) // Reset Bit (Default values = 0)

/** bus voltage range values **/
typedef enum {
	INA219_CONFIG_BVOLTAGERANGE_16V 			= 0x0000U, // 0-16V Range
	INA219_CONFIG_BVOLTAGERANGE_32V 			= 0x2000U, // 0-32V Range (Default values)
}INA219_Config_Bvoltagerange;

/** values for gain bits **/
typedef enum {
	INA219_CONFIG_GAIN_1_40MV 					= 0x0000U, // Gain 1, 40mV Range
	INA219_CONFIG_GAIN_2_80MV 					= 0x0800U, // Gain 2, 80mV Range
	INA219_CONFIG_GAIN_4_160MV 					= 0x1000U, // Gain 4, 160mV Range
	INA219_CONFIG_GAIN_8_320MV 					= 0x1800U, // Gain 8, 320mV Range (Default values)
} INA_219_Config_Gain;

/** values for bus ADC resolution **/
typedef enum {
	INA219_CONFIG_BADCRES_9BIT 					= 0x0000U,  // 9-bit bus res = 0..511
	INA219_CONFIG_BADCRES_10BIT 				= 0x0080U, // 10-bit bus res = 0..1023
	INA219_CONFIG_BADCRES_11BIT 				= 0x0100U, // 11-bit bus res = 0..2047
	INA219_CONFIG_BADCRES_12BIT 				= 0x0180U, // 12-bit bus res = 0..4097 	(Default values)
	INA219_CONFIG_BADCRES_12BIT_2S_1060US 		= 0x0480U, // 2 x 12-bit bus samples averaged
	INA219_CONFIG_BADCRES_12BIT_4S_2130US 		= 0x0500U, // 4 x 12-bit bus samples averaged
	INA219_CONFIG_BADCRES_12BIT_8S_4260US 		= 0x0580U, // 8 x 12-bit bus samples averaged
	INA219_CONFIG_BADCRES_12BIT_16S_8510US 		= 0x0600U, // 16 x 12-bit bus samples averaged
	INA219_CONFIG_BADCRES_12BIT_32S_17MS 		= 0x0680U, // 32 x 12-bit bus samples averaged
	INA219_CONFIG_BADCRES_12BIT_64S_34MS 		= 0x0700U, // 64 x 12-bit bus samples averaged
	INA219_CONFIG_BADCRES_12BIT_128S_69MS 		= 0x0780U, // 128 x 12-bit bus samples averaged
} INA219_Config_Badcres;

/** values for shunt ADC resolution **/
typedef enum {
	INA219_CONFIG_SADCRES_9BIT_1S_84US 			= 0x0000U, // 1 x 9-bit shunt sample
	INA219_CONFIG_SADCRES_10BIT_1S_148US 		= 0x0008U, // 1 x 10-bit shunt sample
	INA219_CONFIG_SADCRES_11BIT_1S_276US 		= 0x0010U, // 1 x 11-bit shunt sample
	INA219_CONFIG_SADCRES_12BIT_1S_532US 		= 0x0018U, // 1 x 12-bit shunt sample	(Default values)
	INA219_CONFIG_SADCRES_12BIT_2S_1060US 		= 0x0048U, // 2 x 12-bit shunt samples averaged
	INA219_CONFIG_SADCRES_12BIT_4S_2130US 		= 0x0050U, // 4 x 12-bit shunt samples averaged
	INA219_CONFIG_SADCRES_12BIT_8S_4260US 		= 0x0058U, // 8 x 12-bit shunt samples averaged
	INA219_CONFIG_SADCRES_12BIT_16S_8510US 		= 0x0060U, // 16 x 12-bit shunt samples averaged
	INA219_CONFIG_SADCRES_12BIT_32S_17MS 		= 0x0068U, // 32 x 12-bit shunt samples averaged
	INA219_CONFIG_SADCRES_12BIT_64S_34MS 		= 0x0070U, // 64 x 12-bit shunt samples averaged
	INA219_CONFIG_SADCRES_12BIT_128S_69MS 		= 0x0078U, // 128 x 12-bit shunt samples averaged
} INA219_Config_Sadcres;

/** values for operating mode **/
typedef enum {
	INA219_CONFIG_MODE_POWERDOWN 				= 0x00U,	/**< power down */
	INA219_CONFIG_MODE_SVOLT_TRIGGERED 			= 0x01U,	/**< shunt voltage triggered */
	INA219_CONFIG_MODE_BVOLT_TRIGGERED 			= 0x02U, 	/**< bus voltage triggered */
	INA219_CONFIG_MODE_SANDBVOLT_TRIGGERED 		= 0x03U,    /**< shunt and bus voltage triggered */
	INA219_CONFIG_MODE_ADCOFF 					= 0x04U, 	/**< ADC off */
	INA219_CONFIG_MODE_SVOLT_CONTINUOUS 		= 0x05U, 	/**< shunt voltage continuous */
	INA219_CONFIG_MODE_BVOLT_CONTINUOUS 		= 0x06U, 	/**< bus voltage continuous */
	INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS 	= 0x07U, 	/**< shunt and bus voltage continuous  (Default values) */
} INA219_Config_Mode;

enum {
	MAX_STAGE_INA219 							= 0xff,
};

/*!
 *   @brief  Class that stores state and functions for interacting with INA219
 *  current/power monitor IC
 */

class Ina219 {
	I2C_HandleTypeDef* hi2c;
	TIM_HandleTypeDef* htim;
//	Terminal* terminal;

	uint32_t timer_clk;
	uint8_t addr;

	uint8_t pData[2];

	uint16_t BusVol;
	uint16_t ShuntVol;
	uint16_t Power;
	uint16_t Current;

	/* Step in run	 */
	uint8_t stage;

	/* Life Time = timeLife / clk / 3600 (h)*/
//	uint64_t timeLife;

	/* Life Current = sum( Life Time * Current) / clc/ 3600 ( Ah) */
//	uint64_t currentLife;

//	uint16_t pre_time;

	/* START= X	 */
	uint8_t flagSTART, flagPOWER;
	uint16_t timePre;
	uint64_t timeMax, timeCounter, totalPower;

	HAL_StatusTypeDef flagTxDone;

	std::deque<std::vector<uint8_t>> pTxListData;
	std::deque<std::vector<uint8_t>> pRxListData;

public:
	Ina219(I2C_HandleTypeDef* hi2c, TIM_HandleTypeDef* htim1, uint8_t addr = INA219_ADDRESS(GND, GND));
//	Ina219(I2C_HandleTypeDef* hi2c, TIM_HandleTypeDef* htim1, Terminal* terminal, uint8_t addr = INA219_ADDRESS(GND, GND));
	~Ina219();

//	HAL_StatusTypeDef init(	INA219_Config_Bvoltagerange BusVolRange = INA219_CONFIG_BVOLTAGERANGE_32V,
//			INA_219_Config_Gain PGA = INA219_CONFIG_GAIN_8_320MV,
//			INA219_Config_Badcres BADC = INA219_CONFIG_BADCRES_12BIT,
//			INA219_Config_Sadcres SADC = INA219_CONFIG_SADCRES_12BIT_1S_532US,
//			INA219_Config_Mode OperaMode = INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS,
//			uint16_t Cal = 0x20c6U	);
	void run_DMA(	INA219_Config_Bvoltagerange BusVolRange = INA219_CONFIG_BVOLTAGERANGE_32V,
				INA_219_Config_Gain PGA = INA219_CONFIG_GAIN_8_320MV,
				INA219_Config_Badcres BADC = INA219_CONFIG_BADCRES_12BIT,
				INA219_Config_Sadcres SADC = INA219_CONFIG_SADCRES_12BIT_1S_532US,
				INA219_Config_Mode OperaMode = INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS,
				uint16_t Cal = 0x20c6U	);
	void IRQHandler();

	void funcStart();
	void funcPower();
//	HAL_StatusTypeDef setBusVolRange(INA219_Config_Bvoltagerange BusVolRange = INA219_CONFIG_BVOLTAGERANGE_32V);
//	HAL_StatusTypeDef setPGA(INA_219_Config_Gain PGA = INA219_CONFIG_GAIN_8_320MV);
//	HAL_StatusTypeDef setBADC(INA219_Config_Badcres BADC = INA219_CONFIG_BADCRES_12BIT);
//	HAL_StatusTypeDef setSADC(INA219_Config_Sadcres SADC = INA219_CONFIG_SADCRES_12BIT_1S_532US);
//	HAL_StatusTypeDef setOperaMode(INA219_Config_Mode OperaMode = INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS);

//	float getBusVol();
//	float getCurrent();

//	HAL_StatusTypeDef getBusVolReg();
//	HAL_StatusTypeDef getCurrentReg();

	void setListData(std::vector<uint8_t>* data);
	std::deque<std::vector<uint8_t>>* getListData();

	void memTxCpltCallback(I2C_HandleTypeDef *hi2c);
	void errorCallback(I2C_HandleTypeDef *hi2c);

private:
//	HAL_StatusTypeDef setCalibration(uint16_t Cal = 0x20c6U);
//	HAL_StatusTypeDef write(INA219_Reg mode, uint16_t pData, uint8_t size);
//	HAL_StatusTypeDef read(INA219_Reg mode, uint8_t* pData, uint8_t size);
//	HAL_StatusTypeDef reset();


};

#endif /* INA219_H_ */
