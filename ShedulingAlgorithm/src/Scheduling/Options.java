package Scheduling;

public class Options {
    private int processNumber;
    private int averageDuration;
    private int deviation;
    private int runTime;
    private int quantum;
    private int increasingPriority;
    private boolean resetSessionCounter;
    private int quantumIncreaseBase;

    public Options() {
        runTime = 1000;
        deviation = 100;
        averageDuration = 1000;
        processNumber = 5;
        increasingPriority = 0;
        resetSessionCounter = false;
        quantumIncreaseBase = 2;
    }

    public void setProcessNumber(int value) {
        processNumber = value;
    }

    public int getProcessNumber() {
        return processNumber;
    }

    public void setAverageDuration(int value) {
        averageDuration = value;
    }

    public int getAverageDuration() {
        return averageDuration;
    }

    public void setDeviation(int value) {
        deviation = value;
    }

    public int getDeviation() {
        return deviation;
    }

    public void setRunTime(int value) {
        runTime = value;
    }

    public int getRunTime() {
        return runTime;
    }

    public void setQuantum(int value) {
        quantum = value;
    }

    public int getQuantum() {
        return quantum;
    }

    public void setQuantumIncreaseBase(int value) {
        quantumIncreaseBase = value;
    }

    public int getQuantumIncreaseBase() {
        return quantumIncreaseBase;
    }

    public void setIncreasingPriority(int value) {
        increasingPriority = value;
    }

    public int getIncreasingPriority() {
        return increasingPriority;
    }

    public void setResetSessionCounter(boolean value) {
        resetSessionCounter = value;
    }

    public boolean getResetSessionCounter() {
        return resetSessionCounter;
    }
}
