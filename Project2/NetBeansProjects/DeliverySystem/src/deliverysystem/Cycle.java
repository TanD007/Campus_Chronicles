/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package deliverysystem;

/**
 *
 * @author tandi
 */
public class Cycle extends DeliveryVehicle {
    private double distanceThreshold;

    public Cycle(double distanceThreshold, String riderName, double speed) {
        super(riderName, speed);
        this.distanceThreshold = distanceThreshold;
    }

    public double getDistanceThreshold() {
        return distanceThreshold;
    }

    public void setDistanceThreshold(double distanceThreshold) {
        this.distanceThreshold = distanceThreshold;
    }

    @Override
    public double getDeliveryTime(Customer customer) {
        if (customer.getDistance()>getDistanceThreshold()) {
            return 99999;
        } else {
            return customer.getDistance()/getSpeed();
        }
    }
    
}
