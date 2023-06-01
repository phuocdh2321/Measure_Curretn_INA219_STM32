/*
 * terminor.cpp
 *
 *  Created on: Apr 27, 2023
 *      Author: ADMIN
 */
#include "terminal.h"

Terminal::Terminal(UART_HandleTypeDef* huart){
	this->huart = huart;

	stage = 0;
	flagTxDone = HAL_OK;
}

Terminal::~Terminal(){

}

//void Terminal::run() {
//	if (stage == 0){
//		if (listData.size() == 0) return;
//		HAL_UART_Transmit_DMA(huart, (uint8_t* )listData[0].data(), listData[0].size());
//		flagTxDone = HAL_BUSY;
//
//		stage++;
//	}
//	if (stage == 1) {
//		if (flagTxDone != HAL_OK) return;
//		listData.pop_front();
//		stage = 0;
//	}
//}

void Terminal::run_vec() {
	if (stage == 0) {
		if (HAL_UARTEx_ReceiveToIdle_DMA(huart, aRXBufferUser, sizeof(aRXBufferUser)) != HAL_OK) return;
		stage++;
	}

	if (stage == 1){
		if (pTxlistData_vec.size() == 0) return;
		HAL_UART_Transmit_DMA(huart, pTxlistData_vec[0].data(), pTxlistData_vec[0].size());
		flagTxDone = HAL_BUSY;

		stage++;
	}
	if (stage == 2) {
		if (flagTxDone != HAL_OK) return;
		pTxlistData_vec.pop_front();
		stage = 1;
	}
}

//void Terminal::setListData(const std::string &data){
//	listData.push_back(data);
//}

void Terminal::setListData(std::vector<uint8_t>* data){
	pTxlistData_vec.push_back(*data);
}

//uint8_t Terminal::getSizeRxListData(){
//	return pRxlistData_vec.size();
//}

std::deque<std::vector<uint8_t>>* Terminal::getRxListData() {
	return &pRxlistData_vec;
}

void Terminal::TxCpltCallback(UART_HandleTypeDef *huart) {
	if (this->huart->Instance == huart->Instance) {
		flagTxDone = HAL_OK;
	}
}

//HAL_StatusTypeDef Terminal::emptyListData(){
//	if (listData.size() == 0) return HAL_OK;
//	return HAL_ERROR;
//}

void Terminal::RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size, uint8_t* flagIDLE) {
	if (this->huart->Instance != huart->Instance) return;

	static uint16_t old_pos = 0;
	static std::vector<uint8_t> pRxdata_vec;

	/* Check if number of received data in recpetion buffer has changed */
	if (Size != old_pos){

		/* Check if position of index in reception buffer has simply be increased
			   of if end of buffer has been reached */
		if (Size > old_pos){

			/* Current position is higher than previous one */
			/* Copy received data in "User" buffer for evacuation */
			pRxdata_vec.insert(pRxdata_vec.end(), aRXBufferUser + old_pos, aRXBufferUser + Size);
		}
		else {

			/* Current position is lower than previous one : end of buffer has been reached */
			/* First copy data from current position till end of buffer */
			pRxdata_vec.insert(pRxdata_vec.end(), aRXBufferUser, aRXBufferUser + Size);
		}
	}

	/* Check if IDLE flag is set */
	if (*flagIDLE == SET) {

		/* Clear IDLE flag in ISR */
		*flagIDLE = RESET;

		/* Insert Vec into rxBuffer_deque */
		pRxlistData_vec.push_back(pRxdata_vec);
		pRxdata_vec.clear();
	}

	/* Update old_pos as new reference of position in User Rx buffer that
			indicates position to which data have been processed */
	old_pos = Size;
}







