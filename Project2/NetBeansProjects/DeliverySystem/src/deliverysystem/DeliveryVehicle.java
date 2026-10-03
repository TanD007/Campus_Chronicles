/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package deliverysystem;

/**
 *
 * @author tandi
 */
public abstract class DeliveryVehicle {
    private String riderName;
    private double speed;

    public DeliveryVehicle(String riderName, double speed) {
        this.riderName = riderName;
        this.speed = speed;
    }

    public String getRiderName() {
        return riderName;
    }

    public void setRiderName(String riderName) {
        this.riderName = riderName;
    }

    public double getSpeed() {
        return speed;
    }

    public void setSpeed(double speed) {
        this.speed = speed;
    }
    public abstract double getDeliveryTime(Customer customer);
}
