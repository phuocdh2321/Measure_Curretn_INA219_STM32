/*
 * terminor.h
 *
 *  Created on: Apr 27, 2023
 *      Author: ADMIN
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_

#include "stm32f1xx_hal.h"
#include <string>
#include <deque>
#include <vector>


class Terminal {

	UART_HandleTypeDef* huart;

	uint8_t aRXBufferUser[10];

//	std::string data;
	std::vector<uint8_t> pTxdata_vec;

//	std::deque<std::string> listData;
	std::deque<std::vector<uint8_t>> pTxlistData_vec;
	std::deque<std::vector<uint8_t>> pRxlistData_vec;

	uint8_t stage;
	uint8_t flagTxDone;
public:
	Terminal(UART_HandleTypeDef* huart);
	~Terminal();

//	void setListData(const std::string &data);
	void setListData(std::vector<uint8_t>* data);
//	uint8_t getSizeRxListData();
	std::deque<std::vector<uint8_t>>* getRxListData();

//	void run();
	void run_vec();
	void TxCpltCallback(UART_HandleTypeDef *huart);
	void RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size, uint8_t* flagIDLE);
//	HAL_StatusTypeDef emptyListData();
};



#endif /* TERMINAL_H_ */
