/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package deliverysystem;

import java.util.ArrayList;

/**
 *
 * @author tandi
 */
public class DeliveryManagement {
    private ArrayList<DeliveryVehicle>availableVehicle;

    public DeliveryManagement() {
        availableVehicle = new ArrayList<>();
    }
    public void addVehicle(DeliveryVehicle vehicle){
        availableVehicle.add(vehicle);
        System.out.println(vehicle.getRiderName() +" was added.");
    }
    
    
    public DeliveryVehicle getBestVehicle(Customer customer){
        DeliveryVehicle bestVehicle = null ;
        double shortestTime = Double.MAX_VALUE;
        for (DeliveryVehicle vehicle : availableVehicle) {
             if (vehicle.getDeliveryTime(customer)<shortestTime){

                bestVehicle =vehicle;
                shortestTime = vehicle.getDeliveryTime(customer);
            }
        }
        availableVehicle.remove(bestVehicle);
        return bestVehicle;
    }
}
