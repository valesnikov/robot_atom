/**
 * @file activ.h
 * @brief Активные движения робота (удары, развороты и т.д.)
 *
 * Файл содержит набор готовых движений робота: удары рукой,
 * развороты и комбинированные действия с сервоприводами и моторами.
 */

#pragma once

#include "motors.h"
#include "servo.h"

/**
 * @brief Пространство имён с активными движениями робота.
 */
namespace mv {

/**
 * @brief Остановить все движения.
 *
 * Обнуляет моторы (жёсткая запись и целевые значения) и переводит
 * все сервоприводы в стандартное (нулевое) положение.
 */
inline void none() {
    motors.IntWrite(0, 0);
    motors.SetTarget(0, 0);
    servoBelt.set(0);
    servoRh.set(0);
    servoRv.set(0);
    servoLh.set(0);
    servoLv.set(0);
    servoNeck.set(0);
}

/**
 * @brief Удар правой рукой (хук справа).
 *
 * Выполняет замах и удар правой рукой с синхронным движением пояса.
 * Движение состоит из фаз: замах, удар, возврат в исходное положение.
 */
inline void r_huk() {
    servoBelt.set(-0.5);
    servoNeckLikeBelt.set(0.5);

    servoRh.set(0.85);
    servoRv.set(-1);

    delay(200);
    servoRv.set(0.75);
    servoBelt.set(1);
    servoNeckLikeBelt.set(-1);
    delay(400);
    servoBelt.set(0);
    servoNeckLikeBelt.set(0);
    servoRh.set(0);
    servoRv.set(0);
}

/**
 * @brief Удар левой рукой (хук слева).
 *
 * Выполняет замах и удар левой рукой с синхронным движением пояса.
 * Движение состоит из фаз: замах, удар, возврат в исходное положение.
 */
inline void l_huk() {
    servoBelt.set(0.5);
    servoNeckLikeBelt.set(-0.5);
    servoLh.set(0.85);
    servoLv.set(-1);

    delay(200);
    servoLv.set(0.75);
    servoBelt.set(-1);
    servoNeckLikeBelt.set(1);
    delay(400);
    servoBelt.set(0);
    servoNeckLikeBelt.set(0);
    servoLh.set(0);
    servoLv.set(0);
}

/**
 * @brief Удар правой рукой снизу (апперкот).
 *
 * Выполняет апперкот правой рукой с движением пояса.
 * Состоит из фаз: подготовка, удар вверх, возврат в исходное положение.
 */
inline void r_aperkot() {
    servoRv.set(-1);
    servoBelt.set(-0.15);
    servoNeckLikeBelt.set(0.15);
    delay(100);
    servoRv.set(1);
    servoBelt.set(0.6);
    servoNeckLikeBelt.set(-0.6);
    delay(400);
    servoRv.set(0);
    servoBelt.set(0);
    servoNeckLikeBelt.set(0);
}

/**
 * @brief Удар левой рукой снизу (апперкот).
 *
 * Выполняет апперкот левой рукой с движением пояса.
 * Состоит из фаз: подготовка, удар вверх, возврат в исходное положение.
 */
inline void l_aperkot() {
    servoLv.set(-1);
    servoBelt.set(0.15);
    servoNeckLikeBelt.set(-0.15);
    delay(100);
    servoLv.set(1);
    servoBelt.set(-0.6);
    servoNeckLikeBelt.set(0.6);
    delay(400);
    servoLv.set(0);
    servoBelt.set(0);
    servoNeckLikeBelt.set(0);
}

/**
 * @brief Движение "мельница" (вращение обеими руками с поворотом пояса).
 *
 * Поднимает обе руки, выполняет вращение пояса в обе стороны
 * и возвращает руки в исходное положение.
 */
inline void meln() {
    servoRh.set(1);
    servoLh.set(1);
    delay(300);
    servoBelt.set(-1);
    servoNeckLikeBelt.set(1);
    delay(300);
    servoBelt.set(1);
    servoNeckLikeBelt.set(-1);
    delay(600);
    servoBelt.set(0);
    servoNeckLikeBelt.set(0);
    delay(300);
    servoRh.set(0);
    servoLh.set(0);
}

/**
 * @brief Мощный разворот вправо (MAX).
 *
 * Выполняет резкий разворот робота вправо с плавным подъёмом правой руки.
 * Включает фазы: разгон, поворот на месте, остановка и возврат.
 */
inline void r_MAX() {
    motors.IntWrite(64, -64);
    for (float i = 0; i < 200; i++) {
        servoBelt.set(-(i / 200));
        servoRh.set(3 * (i / 200));
        servoRv.set(-(i / 200));
        delay(5);
    }
    motors.IntWrite(-255, 255);
    servoRv.set(1);
    servoBelt.set(1);
    servoNeckLikeBelt.set(-1);
    delay(400);
    motors.IntWrite(0, 0);
    delay(500);
    servoBelt.set(0);
    servoNeckLikeBelt.set(0);
    servoRh.set(0);
    servoRv.set(0);
    motors.IntWrite(64, -64);
    delay(1100);
    motors.IntWrite(0, 0);
}

/**
 * @brief Мощный разворот влево (MAX).
 *
 * Выполняет резкий разворот робота влево с плавным подъёмом левой руки.
 * Включает фазы: разгон, поворот на месте, остановка и возврат.
 */
inline void l_MAX() {
    motors.IntWrite(-64, 64);
    for (float i = 0; i < 200; i++) {
        servoBelt.set((i / 200));
        servoLh.set(3 * (i / 200));
        servoLv.set(-(i / 200));
        delay(5);
    }
    motors.IntWrite(255, -255);
    servoLv.set(1);
    servoBelt.set(-1);
    servoNeckLikeBelt.set(1);
    delay(400);
    motors.IntWrite(0, 0);
    delay(500);
    servoBelt.set(0);
    servoNeckLikeBelt.set(0);
    servoLh.set(0);
    servoLv.set(0);
    motors.IntWrite(-64, 64);
    delay(1100);
    motors.IntWrite(0, 0);
}

/**
 * @brief Повернуть робота вправо (голова и пояс).
 *
 * Вращает шею и пояс вправо.
 */
inline void set_right() {
    servoNeck.set(1);
    servoBelt.set(1);
}

/**
 * @brief Повернуть робота влево (голова и пояс).
 *
 * Вращает шею и пояс влево.
 */
inline void set_left() {
    servoNeck.set(-1);
    servoBelt.set(-1);
}

/**
 * @brief Выполнить следующий удар из списка по кругу.
 *
 * Каждый вызов выполняет одно из движений по очереди:
 * правый хук, левый апперкот, левый хук, правый апперкот и т.д.
 */
inline void punch() {
    static int i = 0;
    static void (*const mvs[])(void) = {
        r_huk,
        l_aperkot,
        l_huk,
        r_aperkot,
        l_aperkot,
        l_huk,
    };
    mvs[i]();
    i = (i + 1) % (sizeof(mvs) / sizeof(mvs[0]));
}

} // namespace mv