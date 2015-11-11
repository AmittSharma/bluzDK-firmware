/**
 Copyright (c) 2015 MidAir Technology, LLC.  All rights reserved.
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation, either
 version 3 of the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "spark_wiring_bluetooth_le.h"
#include "core_hal.h"

BLEState BLEClass::getState()
{
    return (BLEState)HAL_BLE_GET_STATE();
}

void BLEClass::startAdvertising()
{
    HAL_BLE_Start_Advertising();
}

void BLEClass::stopAdvertising()
{
    HAL_BLE_Stop_Advertising();
}

int BLEClass::registerNotifications(void (*radio_callback)(bool radio_active))
{
    return HAL_BLE_Register_Radio_Notification(radio_callback);
}