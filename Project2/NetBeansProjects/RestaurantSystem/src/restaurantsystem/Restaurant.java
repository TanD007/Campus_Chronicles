/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package restaurantsystem;

/**
 *
 * @author tandi
 */
public abstract class Restaurant {
    private String name;
    private double waitTime;

    public Restaurant(String name, double waitTime) {
        this.name = name;
        this.waitTime = waitTime;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public double getWaitTime() {
        return waitTime;
    }

    public void setWaitTime(double waitTime) {
        this.waitTime = waitTime;
    }
    public abstract double getPreparationTime(Item item);
}
