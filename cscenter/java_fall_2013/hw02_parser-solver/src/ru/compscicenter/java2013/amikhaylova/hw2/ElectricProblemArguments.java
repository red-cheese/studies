package ru.compscicenter.java2013.amikhaylova.hw2;

import ru.compscicenter.java2013.electro.ElectricQuantityType;

/**
 * Класс, описывающий задачу на закон Ома с нормализованными аргументами.
 *
 * @author Alexandra Mikhaylova mikhaylova.alexandra.a@gmail.com
 */
class ElectricProblemArguments {
    /**
     * Ток в А.
     */
    private double current;

    /**
     * Сопротивление в Ом.
     */
    private double resistance;

    /**
     * Напряжение в В.
     */
    private double voltage;

    /**
     * Неизвестная величина (тип).
     */
    private ElectricQuantityType x;

    public ElectricProblemArguments(double current,
                                    double resistance,
                                    double voltage,
                                    ElectricQuantityType x) {
        this.current = current;
        this.resistance = resistance;
        this.voltage = voltage;
        this.x = x;
    }

    public double getCurrent() {
        return current;
    }

    public double getResistance() {
        return resistance;
    }

    public double getVoltage() {
        return voltage;
    }

    public ElectricQuantityType getX() {
        return x;
    }
}
