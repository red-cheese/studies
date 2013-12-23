package ru.compscicenter.java2013.amikhaylova.hw2;

import java.util.regex.Matcher;
import java.util.regex.Pattern;
import ru.compscicenter.java2013.electro.ElectricQuantityType;
import ru.compscicenter.java2013.electro.SolverException;

/**
 * Парсер текста задачи на закон Ома.
 * Для работы класса требуется Java 7.
 *
 * @author Alexandra Mikhaylova mikhaylova.alexandra.a@gmail.com
 */
class ElectricProblemParser {

    private static final Pattern CURRENT_PATTERN = Pattern.compile(
        ".*I\\s+=\\s+(\\d+(\\.\\d+)?)\\s+(к|М|Г|м|мк|н)?А.*");

    private static final Pattern RESISTANCE_PATTERN = Pattern.compile(
        ".*R\\s+=\\s+(\\d+(\\.\\d+)?)\\s+(к|М|Г|м|мк|н)?Ом.*");

    private static final Pattern VOLTAGE_PATTERN = Pattern.compile(
        ".*U\\s+=\\s+(\\d+(\\.\\d+)?)\\s+(к|М|Г|м|мк|н)?В.*");

    private double normalizeArgument(double value, String unit) {
        switch (unit) {
            case "к":
                return value * 1000;
            case "М":
                return value * 1000000;
            case "Г":
                return value * 1000000000;
            case "м":
                return value / 1000.0;
            case "мк":
                return value / 1000000.0;
            case "н":
                return value / 1000000000.0;
            default:
                return value;
        }
    }

    private double parseArgument(Matcher argumentMatcher) {
        double value = Double.NaN;

        if (argumentMatcher.find()) {
            value = Double.parseDouble(argumentMatcher.group(1));
            String unit = argumentMatcher.group(3);
            if (unit != null) {
                value = normalizeArgument(value, unit);
            }
        }

        return value;
    }

    private ElectricQuantityType findX(double current,
                                       double resistance,
                                       double voltage) throws SolverException {
        ElectricQuantityType type = null;
        int countNaN = 0;

        if (Double.isNaN(current)) {
            type = ElectricQuantityType.CURRENT;
            countNaN++;
        }

        if (Double.isNaN(resistance)) {
            type = ElectricQuantityType.RESISTANCE;
            countNaN++;
        }

        if (Double.isNaN(voltage)) {
            type = ElectricQuantityType.VOLTAGE;
            countNaN++;
        }

        if (countNaN == 1) {
            return type;
        } else if (countNaN > 1) {
            throw new SolverException("Недостаточно данных для решения.");
        } else {
            throw new SolverException("Неизвестных величин не найдено.");
        }
    }

    public ElectricProblemArguments parseArguments(String problemText)
            throws SolverException {
        double current = parseArgument(
            CURRENT_PATTERN.matcher(problemText));
        double resistance = parseArgument(
            RESISTANCE_PATTERN.matcher(problemText));
        double voltage = parseArgument(
            VOLTAGE_PATTERN.matcher(problemText));

        ElectricQuantityType type = findX(current, resistance, voltage);

        return new ElectricProblemArguments(current, resistance, voltage, type);
    }
}
