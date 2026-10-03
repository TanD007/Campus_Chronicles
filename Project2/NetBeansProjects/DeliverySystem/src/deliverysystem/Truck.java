package deliverysystem;

/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */

/**
 *
 * @author tandi
 */
public class Truck extends DeliveryVehicle{
    private double fuel;

    public Truck(double fuel, String riderName, double speed) {
        super(riderName, speed);
        this.fuel = fuel;
    }

    public double getFuel() {
        return fuel;
    }

    public void setFuel(double fuel) {
        this.fuel = fuel;
    }

    @Override
    public double getDeliveryTime(Customer customer) {
        if (getFuel()<1) {
            return 99999;
        } else {
            return customer.getDistance()/getSpeed();
            
        }
    }

    
    
}
