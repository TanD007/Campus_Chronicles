/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package ridesharing;

/**
 *
 * @author tandi
 */
public abstract class Vehicle {
    private String driverName;
    private double speed;

    public Vehicle(String driverName, double speed) {
        this.driverName = driverName;
        this.speed = speed;
    }

    public String getDriverName() {
        return driverName;
    }

    public void setDriverName(String driverName) {
        this.driverName = driverName;
    }

    public double getSpeed() {
        return speed;
    }

    public void setSpeed(double speed) {
        this.speed = speed;
    }
    public abstract double calcTravelTime(Customer customer);
            
}
