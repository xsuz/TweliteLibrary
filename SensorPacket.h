#pragma once
#include <stdint.h>

/// @brief データ転送
namespace DeviceData
{
    /// @brief 各センサーの識別子
    /// @details 上位4bitで種類を識別、下位4bitで基板を識別
    enum DeviceID
    {
        MainBoard = 0x00, // メイン基板

        ServoController = 0x10, // 操舵基板

        Tachometer = 0x20, // 回転出力計

        Pitot = 0x30, // ピトー管

        IMU = 0x40, // IMU

        UltraSonic = 0x50, // 超音波式高度計

        GPS = 0x60, // GPS

        Vane = 0x70, // 風見

        Barometer = 0x90 // 気圧計

    };

    /// @brief 操舵基板用のDeviceData
    /// @note 先頭はid
    struct ServoData
    {
        /// @brief デバイス識別子
        uint8_t id;
        /// @brief 時刻
        uint32_t timestamp;
        /// @brief 操舵角(ラダー)
        float rudder;
        /// @brief 操舵角(エレベータ)
        float elevator;
        /// @brief バッテリー電圧
        float voltage;
        /// @brief サーボ電流(ラダー)
        float rudder_current;
        /// @brief サーボ電流(エレベータ)
        float elevator_current;
        /// @brief トリム値
        float trim;
        /// @brief ステータス
        uint8_t status;
    }; // IDは必ず先頭にする

    /// @brief 回転出力計のDeviceData
    /// @note 先頭はid
    struct TachometerData
    {
        /// @brief デバイス識別子
        uint8_t id;
        /// @brief 時刻
        uint32_t timestamp;
        /// @brief ひずみ値
        float strain;
        /// @brief 回転数
        float rpm;
    }; // IDは必ず先頭にする

    /// @brief ピトー管のDeviceData
    /// @note 先頭はid
    struct PitotData
    {
        /// @brief デバイス識別子
        uint8_t id;
        /// @brief 時刻
        uint32_t timestamp;
        /// @brief 圧力
        float pressure;
        /// @brief 温度
        float temperature;
        /// @brief 対気速度
        float velocity;
    }; // IDは必ず先頭にする

    /// @brief IMUのDeviceData
    /// @note 先頭はid
    struct IMUData
    {
        /// @brief デバイス識別子
        uint8_t id;
        /// @brief calib status
        uint16_t calib;
        /// @brief 時刻
        uint32_t timestamp;
        /// @brief quaternion
        short q[4];
        /// @brief magneto amplitude
        short m[3];
        /// @brief accel
        short a[3];
        /// @brief gyro
        short g[3];

        /// @brief quaternion
        short q1[4];
        /// @brief magneto amplitude
        short m1[3];
        /// @brief accel
        short a1[3];
        /// @brief gyro
        short g1[3];

        /// @brief quaternion
        short q2[4];
        /// @brief magneto amplitude
        short m2[3];
        /// @brief accel
        short a2[3];
        /// @brief gyro
        short g2[3];
    };

    /// @brief 高度計のDeviceData
    /// @note 先頭はid
    struct UltraSonicData
    {
        /// @brief デバイス識別子
        uint8_t id;
        /// @brief 時刻
        uint32_t timestamp;
        /// @brief 高度
        float altitude;
    };

    /// @brief GPSのDeviceData
    /// @note 先頭はid
    struct GPSData
    {
        /// @brief デバイス識別子
        uint8_t id;
        /// @brief 時刻
        uint32_t timestamp;
        /// @brief 緯度
        double latitude;
        /// @brief 経度
        double longitude;
        /// @brief 秒速m/s(東が正の方向)
        float vx;
        /// @brief 秒速m/s(北が正の方向)
        float vy;
    };

    /// @brief 風見のDeviceData
    /// @note 先頭はid
    struct VaneData{
        /// @brief デバイス識別子
        uint8_t id;
        /// @brief 時刻
        uint32_t timestamp;
        /// @brief 緯度
        float angle;
    };

    /// @brief 気圧センサのDeviceData
    /// @note 先頭はid
    struct BarometerData
    {
        /// @brief デバイス識別子
        uint8_t id;
        /// @brief 時刻
        uint32_t timestamp;
        /// @brief 圧力
        float pressure;
        /// @brief 温度
        float temperature;
    };
}