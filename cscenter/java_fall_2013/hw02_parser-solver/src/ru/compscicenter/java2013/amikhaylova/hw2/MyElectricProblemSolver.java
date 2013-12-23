package ru.compscicenter.java2013.amikhaylova.hw2;

import ru.compscicenter.java2013.electro.*;
import ru.compscicenter.java2013.amikhaylova.hw2.ElectricProblemArguments;
import ru.compscicenter.java2013.amikhaylova.hw2.ElectricProblemParser;

/**
 * @author Alexandra Mikhaylova mikhaylova.alexandra.a@gmail.com
 */
public class MyElectricProblemSolver implements ElectricProblemSolver {

    private ElectricProblemParser parser;

    public MyElectricProblemSolver() {
        this.parser = new ElectricProblemParser();
    }

    @Override
    public ElectricQuantity solve(String problemText) throws SolverException {
        ElectricProblemArguments args = parser.parseArguments(problemText);

        switch (args.getX()) {
            case CURRENT:
                return new ElectricQuantity(
                    ElectricQuantityType.CURRENT,
                    args.getVoltage() / args.getResistance());
            case RESISTANCE:
                return new ElectricQuantity(
                    ElectricQuantityType.RESISTANCE,
                    args.getVoltage() / args.getCurrent());
            case VOLTAGE:
                return new ElectricQuantity(
                    ElectricQuantityType.VOLTAGE,
                    args.getCurrent() * args.getResistance());
            default:
                throw new SolverException("Неизвестных величин не найдено.");
        }
    }

    /**
     * Пример работы.
     */
    public static void main(String args[]) {
        ElectricProblemSolver solver = new MyElectricProblemSolver();

        try {
            ElectricQuantity q = solver.solve(
                "Найдите сопротивление R участка цепи, если известно, что напряжение U = 10 кВ и сила тока I = 1 нА.");
//            q = solver.solve(
//                "R = 1 Ом I = 1 А U = 1 В");
            System.out.println(q.toString() + " " + q.getType().getUnit());
        } catch (SolverException e) {
            e.printStackTrace();
        }
    }
}
